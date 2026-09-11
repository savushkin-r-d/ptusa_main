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
        chart_data={"ok": True, "series": []},
    )

    document = load_session(path)
    assert document["connection"] == {"host": "192.168.1.10", "port": 10_000}
    assert document["poll_interval_ms"] == 250
    assert document["history_limit"] == 7_500
    assert document["expressions"] == ["TE1:get_value()"]


def test_old_session_uses_default_history_limit(tmp_path) -> None:
    path = tmp_path / "old.ptlua.json"
    path.write_text(
        '{"version":1,"connection":{},"poll_interval_ms":500,"expressions":[]}',
        encoding="utf-8",
    )

    assert load_session(path)["history_limit"] == 5_000
