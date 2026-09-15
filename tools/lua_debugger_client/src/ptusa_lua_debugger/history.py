from __future__ import annotations

from dataclasses import dataclass
from typing import Any

DEFAULT_HISTORY_LIMIT = 5_000
MAX_HISTORY_LIMIT = 1_000_000
DEFAULT_DISPLAY_SECONDS = 60
MAX_DISPLAY_SECONDS = 86_400
TIME_ANCHOR_FIELDS = ("controller_time_unix_ms", "controller_time_millisec")


@dataclass
class HistoryRow:
    time_ms: int
    samples: dict[str, dict[str, Any]]


def controller_timestamp_ms(data: dict[str, Any], millisec: int) -> int | None:
    """Convert a wrapping PAC millisecond counter to controller Unix time."""
    unix_ms = data.get("controller_time_unix_ms")
    anchor_millisec = data.get("controller_time_millisec")
    if (
        not isinstance(unix_ms, int)
        or isinstance(unix_ms, bool)
        or not isinstance(anchor_millisec, int)
        or isinstance(anchor_millisec, bool)
    ):
        return None
    elapsed_ms = (int(millisec) - anchor_millisec) & 0xFFFFFFFF
    return unix_ms + elapsed_ms


def _overlap_size(
    previous: list[dict[str, Any]], current: list[dict[str, Any]]
) -> int:
    """Return the largest previous suffix matching the current prefix."""
    for size in range(min(len(previous), len(current)), 0, -1):
        if previous[-size:] == current[:size]:
            return size
    return 0


def merge_chart_data(
    previous: dict[str, Any] | None,
    current: dict[str, Any],
    limit: int,
    history_expressions: set[str] | None = None,
) -> dict[str, Any]:
    """Merge the server's rolling cache into the longer client-side history."""
    limit = max(1, min(int(limit), MAX_HISTORY_LIMIT))
    if previous is not None and any(
        previous.get(field) != current.get(field) for field in TIME_ANCHOR_FIELDS
    ):
        previous = None
    previous_by_expression = {
        item.get("expression"): item
        for item in (previous or {}).get("series", [])
        if isinstance(item, dict)
    }
    merged_series: list[dict[str, Any]] = []

    for item in current.get("series", []):
        if not isinstance(item, dict):
            continue
        expression = item.get("expression")
        if not isinstance(expression, str):
            continue
        old_item = previous_by_expression.get(expression, {})
        old_samples = list(old_item.get("samples", []))
        new_samples = list(item.get("samples", []))
        overlap = _overlap_size(old_samples, new_samples)
        expression_limit = (
            limit
            if history_expressions is None or expression in history_expressions
            else 2
        )
        samples = (old_samples + new_samples[overlap:])[-expression_limit:]
        merged_series.append({"expression": expression, "samples": samples})

    result = {
        "ok": current.get("ok", True),
        "server_time_ms": current.get("server_time_ms", 0),
        "series": merged_series,
    }
    for field in TIME_ANCHOR_FIELDS:
        if field in current:
            result[field] = current[field]
    return result


def trim_chart_data(
    data: dict[str, Any] | None,
    limit: int,
    history_expressions: set[str] | None = None,
) -> dict[str, Any] | None:
    if data is None:
        return None
    limit = max(1, min(int(limit), MAX_HISTORY_LIMIT))
    trimmed_series: list[dict[str, Any]] = []
    for item in data.get("series", []):
        if not isinstance(item, dict):
            continue
        expression_limit = (
            limit
            if history_expressions is None
            or item.get("expression") in history_expressions
            else 2
        )
        trimmed_series.append(
            {**item, "samples": list(item.get("samples", []))[-expression_limit:]}
        )
    return {
        **data,
        "series": trimmed_series,
    }


def build_history_rows(
    data: dict[str, Any] | None, expressions: list[str]
) -> tuple[list[HistoryRow], bool]:
    """Build a sparse event table with one column per selected expression."""
    if not data or not expressions:
        return [], False

    selected = set(expressions)
    series = [
        item
        for item in data.get("series", [])
        if isinstance(item, dict) and item.get("expression") in selected
    ]
    first_sample = next(
        (
            sample
            for item in series
            for sample in item.get("samples", [])
            if isinstance(sample, dict) and "time_ms" in sample
        ),
        None,
    )
    if first_sample is None:
        return [], False

    base_millisec = int(first_sample["time_ms"])
    absolute_time = controller_timestamp_ms(data, base_millisec) is not None
    rows_by_time: dict[int, HistoryRow] = {}
    for item in series:
        expression = str(item["expression"])
        for sample in item.get("samples", []):
            if not isinstance(sample, dict) or "time_ms" not in sample:
                continue
            millisec = int(sample["time_ms"])
            timestamp = controller_timestamp_ms(data, millisec)
            row_time = (
                timestamp
                if timestamp is not None
                else (millisec - base_millisec) & 0xFFFFFFFF
            )
            row = rows_by_time.setdefault(row_time, HistoryRow(row_time, {}))
            row.samples[expression] = sample

    return [rows_by_time[key] for key in sorted(rows_by_time)], absolute_time


def merge_statistics(
    previous: dict[str, dict[str, float]], data: dict[str, Any]
) -> dict[str, dict[str, float]]:
    """Accumulate numeric extrema independently from the trimmed chart history."""
    result = {expression: dict(extrema) for expression, extrema in previous.items()}
    for series in data.get("series", []):
        if not isinstance(series, dict):
            continue
        expression = series.get("expression")
        if not isinstance(expression, str):
            continue
        values = [
            float(sample["value"])
            for sample in series.get("samples", [])
            if sample.get("ok") and sample.get("type") in {"number", "boolean"}
        ]
        if not values:
            continue
        extrema = result.get(expression)
        minimum = min(values)
        maximum = max(values)
        if extrema is not None:
            minimum = min(minimum, extrema["min"])
            maximum = max(maximum, extrema["max"])
        result[expression] = {"min": minimum, "max": maximum}
    return result
