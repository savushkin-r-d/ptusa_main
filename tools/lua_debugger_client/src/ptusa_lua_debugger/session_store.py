from __future__ import annotations

import json
import math
import re
from pathlib import Path
from typing import Any

from .history import (
    DEFAULT_DISPLAY_SECONDS,
    DEFAULT_HISTORY_LIMIT,
    MAX_DISPLAY_SECONDS,
    MAX_HISTORY_LIMIT,
)


def save_session(
    path: str | Path,
    *,
    host: str,
    port: int,
    poll_interval_ms: int,
    history_limit: int,
    expressions: list[str],
    history_expressions: list[str],
    chart_data: dict[str, Any] | None,
    display_seconds: int = DEFAULT_DISPLAY_SECONDS,
    auto_follow: bool = True,
    statistics: dict[str, dict[str, Any]] | None = None,
    series_styles: dict[str, dict[str, Any]] | None = None,
) -> None:
    document = {
        "version": 1,
        "connection": {"host": host, "port": port},
        "poll_interval_ms": poll_interval_ms,
        "history_limit": history_limit,
        "display_seconds": display_seconds,
        "auto_follow": auto_follow,
        "statistics": statistics or {},
        "series_styles": series_styles or {},
        "expressions": expressions,
        "history_expressions": history_expressions,
        "chart_data": chart_data,
    }
    Path(path).write_text(
        json.dumps(document, ensure_ascii=False, indent=2), encoding="utf-8"
    )


def load_session(path: str | Path) -> dict[str, Any]:
    document = json.loads(Path(path).read_text(encoding="utf-8"))
    if document.get("version") != 1:
        raise ValueError("Неподдерживаемая версия файла сессии")
    connection = document.get("connection")
    expressions = document.get("expressions")
    history_expressions = document.get("history_expressions", expressions)
    interval = document.get("poll_interval_ms")
    history_limit = document.get("history_limit", DEFAULT_HISTORY_LIMIT)
    display_seconds = document.get("display_seconds", DEFAULT_DISPLAY_SECONDS)
    auto_follow = document.get("auto_follow", True)
    statistics = document.get("statistics", {})
    if not isinstance(connection, dict) or not isinstance(expressions, list):
        raise TypeError("Некорректный файл сессии")
    if not all(isinstance(item, str) for item in expressions):
        raise ValueError("Некорректный список выражений")
    if (
        not isinstance(history_expressions, list)
        or not all(isinstance(item, str) for item in history_expressions)
        or not set(history_expressions).issubset(expressions)
    ):
        raise ValueError("Некорректный список выражений с историей")
    if not isinstance(interval, int):
        raise TypeError("Некорректный интервал опроса")
    if (
        not isinstance(history_limit, int)
        or isinstance(history_limit, bool)
        or not 1 <= history_limit <= MAX_HISTORY_LIMIT
    ):
        raise TypeError("Некорректный лимит истории")
    document["history_limit"] = history_limit
    if (
        not isinstance(display_seconds, int)
        or isinstance(display_seconds, bool)
        or not 1 <= display_seconds <= MAX_DISPLAY_SECONDS
    ):
        raise TypeError("Некорректный интервал отображения")
    if not isinstance(auto_follow, bool):
        raise TypeError("Некорректный режим отображения")
    if not isinstance(statistics, dict) or any(
        not isinstance(expression, str) or not _valid_statistics_entry(entry)
        for expression, entry in statistics.items()
    ):
        raise TypeError("Некорректная статистика выражений")
    styles = document.get("series_styles", {})
    if not isinstance(styles, dict) or any(
        not isinstance(expression, str) or not _valid_series_style(style)
        for expression, style in styles.items()
    ):
        raise ValueError("Некорректные настройки линий графика")
    document["series_styles"] = styles
    document["display_seconds"] = display_seconds
    document["auto_follow"] = auto_follow
    document["statistics"] = statistics
    document["history_expressions"] = history_expressions
    return document


def _valid_statistics_entry(entry: Any) -> bool:
    if not isinstance(entry, dict):
        return False
    numeric = lambda value: isinstance(value, (int, float)) and not isinstance(
        value, bool
    )
    if set(entry) == {"min", "max"}:
        return numeric(entry["min"]) and numeric(entry["max"])

    expected = {
        "min",
        "max",
        "average",
        "median",
        "_sum",
        "_count",
        "_values",
        "_last_sample",
    }
    if set(entry) != expected:
        return False
    values = entry["_values"]
    count = entry["_count"]
    return (
        all(numeric(entry[key]) for key in ("min", "max", "average", "median", "_sum"))
        and isinstance(count, int)
        and not isinstance(count, bool)
        and count > 0
        and isinstance(values, list)
        and len(values) == count
        and all(numeric(value) for value in values)
        and isinstance(entry["_last_sample"], dict)
    )


def _valid_series_style(style: Any) -> bool:
    if not isinstance(style, dict):
        return False
    offset = style.get("offset", 0)
    return (
        isinstance(style.get("name", ""), str)
        and isinstance(style.get("color", "#ffffff"), str)
        and re.fullmatch(r"#[0-9a-fA-F]{6}", style.get("color", "#ffffff")) is not None
        and isinstance(offset, (int, float)) and not isinstance(offset, bool)
        and math.isfinite(offset) and abs(offset) <= 1e12
        and isinstance(style.get("points", False), bool)
    )
