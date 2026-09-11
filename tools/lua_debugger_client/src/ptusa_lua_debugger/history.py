from __future__ import annotations

from typing import Any

DEFAULT_HISTORY_LIMIT = 5_000
MAX_HISTORY_LIMIT = 1_000_000


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
) -> dict[str, Any]:
    """Merge the server's rolling cache into the longer client-side history."""
    limit = max(1, min(int(limit), MAX_HISTORY_LIMIT))
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
        samples = (old_samples + new_samples[overlap:])[-limit:]
        merged_series.append({"expression": expression, "samples": samples})

    return {
        "ok": current.get("ok", True),
        "server_time_ms": current.get("server_time_ms", 0),
        "series": merged_series,
    }


def trim_chart_data(data: dict[str, Any] | None, limit: int) -> dict[str, Any] | None:
    if data is None:
        return None
    limit = max(1, min(int(limit), MAX_HISTORY_LIMIT))
    return {
        **data,
        "series": [
            {**item, "samples": list(item.get("samples", []))[-limit:]}
            for item in data.get("series", [])
            if isinstance(item, dict)
        ],
    }
