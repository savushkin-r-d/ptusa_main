from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from .history import DEFAULT_HISTORY_LIMIT, MAX_HISTORY_LIMIT


def save_session(
    path: str | Path,
    *,
    host: str,
    port: int,
    poll_interval_ms: int,
    history_limit: int,
    expressions: list[str],
    chart_data: dict[str, Any] | None,
) -> None:
    document = {
        "version": 1,
        "connection": {"host": host, "port": port},
        "poll_interval_ms": poll_interval_ms,
        "history_limit": history_limit,
        "expressions": expressions,
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
    interval = document.get("poll_interval_ms")
    history_limit = document.get("history_limit", DEFAULT_HISTORY_LIMIT)
    if not isinstance(connection, dict) or not isinstance(expressions, list):
        raise TypeError("Некорректный файл сессии")
    if not all(isinstance(item, str) for item in expressions):
        raise ValueError("Некорректный список выражений")
    if not isinstance(interval, int):
        raise TypeError("Некорректный интервал опроса")
    if (
        not isinstance(history_limit, int)
        or isinstance(history_limit, bool)
        or not 1 <= history_limit <= MAX_HISTORY_LIMIT
    ):
        raise TypeError("Некорректный лимит истории")
    document["history_limit"] = history_limit
    return document
