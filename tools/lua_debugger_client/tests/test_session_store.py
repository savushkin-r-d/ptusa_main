from ptusa_lua_debugger.session_store import load_session, save_session


def test_session_roundtrip(tmp_path) -> None:
    path = tmp_path / "line.ptlua.json"
    save_session(
        path,
        host="192.168.1.10",
        port=10_000,
        poll_interval_ms=250,
        history_limit=7_500,
        expressions=["TE1:get_value()"],
        history_expressions=["TE1:get_value()"],
        chart_data={"ok": True, "series": []},
        display_seconds=120,
        auto_follow=False,
        statistics={
            "TE1:get_value()": {
                "min": 1.5,
                "max": 8.0,
                "average": 4.75,
                "median": 4.75,
                "_sum": 9.5,
                "_count": 2,
                "_values": [1.5, 8.0],
                "_last_sample": {
                    "time_ms": 2,
                    "ok": True,
                    "type": "number",
                    "value": 8.0,
                },
            }
        },
    )

    document = load_session(path)
    assert document["connection"] == {"host": "192.168.1.10", "port": 10_000}
    assert document["poll_interval_ms"] == 250
    assert document["history_limit"] == 7_500
    assert document["display_seconds"] == 120
    assert document["auto_follow"] is False
    assert document["statistics"]["TE1:get_value()"]["average"] == 4.75
    assert document["statistics"]["TE1:get_value()"]["median"] == 4.75
    assert document["statistics"]["TE1:get_value()"]["_count"] == 2
    assert document["expressions"] == ["TE1:get_value()"]
    assert document["history_expressions"] == ["TE1:get_value()"]


def test_old_session_uses_default_history_limit(tmp_path) -> None:
    path = tmp_path / "old.ptlua.json"
    path.write_text(
        '{"version":1,"connection":{},"poll_interval_ms":500,"expressions":[]}',
        encoding="utf-8",
    )

    document = load_session(path)
    assert document["history_limit"] == 5_000
    assert document["display_seconds"] == 60
    assert document["auto_follow"] is True
    assert document["statistics"] == {}
    assert document["history_expressions"] == []


def test_old_session_enables_history_for_existing_expressions(tmp_path) -> None:
    path = tmp_path / "old-with-expression.ptlua.json"
    path.write_text(
        '{"version":1,"connection":{},"poll_interval_ms":500,'
        '"expressions":["x"]}',
        encoding="utf-8",
    )

    document = load_session(path)

    assert document["history_expressions"] == ["x"]


def test_old_session_accepts_legacy_min_max_statistics(tmp_path) -> None:
    path = tmp_path / "old-statistics.ptlua.json"
    path.write_text(
        '{"version":1,"connection":{},"poll_interval_ms":500,'
        '"expressions":["x"],"statistics":{"x":{"min":1,"max":9}}}',
        encoding="utf-8",
    )

    document = load_session(path)

    assert document["statistics"] == {"x": {"min": 1, "max": 9}}
