from __future__ import annotations

import os
import pytest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication

from ptusa_lua_debugger.window import DebuggerSessionWidget, MainWindow
from ptusa_lua_debugger.pulse_counter import PulseDefinition


@pytest.mark.parametrize("chart_type, expected_x, expected_y", [
    ("step_post", [0, 2, 2, 5, 5, 7, 7, 7], [0, 0, 1, 1, 0, 0, 0, 0]),
    ("step_pre", [0, 0, 0, 2, 2, 5, 5, 7], [0, 0, 1, 1, 0, 0, 0, 0]),
    ("step_mid", [0, 1, 1, 3.5, 3.5, 7], [0, 0, 1, 1, 0, 0]),
    ("line", [0, 2, 5, 7], [0, 1, 0, 0]),
    ("scatter", [0, 2, 5], [0, 1, 0]),
])
@pytest.mark.parametrize("single_sample", [False, True])
def test_chart_type_geometry_and_roundtrip(tmp_path, chart_type, expected_x,
                                          expected_y, single_sample) -> None:
    from copy import deepcopy
    from ptusa_lua_debugger.session_store import load_session, save_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        session._create_expression("x", history_enabled=True)
        session._create_expression("other", history_enabled=True)
        samples = [
            {"time_ms": t, "value": v, "type": "number", "ok": True}
            for t, v in [(1000, 0), (3000, 1), (6000, 0)]
        ]
        data = {"server_time_ms": 8000, "series": [
            {"expression": "x", "samples": samples[:1] if single_sample else samples}
        ]}
        session._on_chart_data(data)
        original = deepcopy(session._last_chart_data)
        root = session.variables.topLevelItem(0)
        selector = session.variables.itemWidget(root.child(9), 2)
        selector.setCurrentIndex(selector.findData(chart_type))
        curve = session.plot.listDataItems()[0]
        # Inspect the rendered geometry, not just the selected option.
        path = curve.curve.getPath()
        if chart_type != "scatter" and not single_sample:
            assert [path.elementAt(i).x for i in range(path.elementCount())] == expected_x
            assert [path.elementAt(i).y for i in range(path.elementCount())] == expected_y
        if chart_type == "scatter":
            assert list(curve.xData) == ([0] if single_sample else expected_x)
            assert curve.opts["pen"] is None
            assert curve.opts["symbol"] == "o"
        assert session._series_styles()["other"]["chart_type"] == "step_post"
        assert session._last_chart_data == original
        path = tmp_path / "styles.json"
        save_session(path, host="localhost", port=10000, poll_interval_ms=500,
                     history_limit=5000, expressions=session._expressions(),
                     history_expressions=session._history_expressions(),
                     chart_data=session._last_chart_data,
                     series_styles=session._series_styles())
        restored.load_document(load_session(path))
        assert restored._series_styles() == session._series_styles()
        assert restored.plot.listDataItems()[0].opts["stepMode"] == curve.opts["stepMode"]
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


def test_tabs_own_independent_workers_and_threads() -> None:
    application = QApplication.instance() or QApplication([])
    window = MainWindow()
    try:
        first = window.sessions.widget(0)
        second = window._add_session()

        assert isinstance(first, DebuggerSessionWidget)
        assert first is not second
        assert first._worker is not second._worker
        assert first._thread is not second._thread
        assert window.sessions.count() == 2

        window._close_session(window.sessions.indexOf(second))
        application.processEvents()
        assert window.sessions.count() == 1
        assert first._thread.isRunning()
    finally:
        window.close()
        application.processEvents()


def test_session_displays_debugger_messages() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._on_messages(
            {
                "controller_time_unix_ms": 1_789_123_456_789,
                "controller_time_millisec": 123_456,
                "dropped": 0,
                "messages": [
                    {
                        "id": 1,
                        "time_ms": 123_500,
                        "source": "set_err_msg",
                        "priority": 3,
                        "text": "Тестовая авария",
                    }
                ],
            }
        )

        assert session.messages_table.rowCount() == 1
        assert session.messages_table.item(0, 1).text() == "set_err_msg"
        assert session.messages_table.item(0, 2).text() == "ERROR"
        assert session.messages_table.item(0, 3).text() == "Тестовая авария"
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_tree_chart_styles_and_session_roundtrip(tmp_path) -> None:
    from copy import deepcopy
    from ptusa_lua_debugger.session_store import save_session, load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        style = {"name": "Ступень", "color": "#32aaff", "offset": 2.5, "points": True}
        session._create_expression("x", history_enabled=True, style=style)
        data = {"server_time_ms": 3000, "series": [{"expression": "x", "samples": [
            {"time_ms": 1000, "value": 0, "type": "number", "ok": True},
            {"time_ms": 2000, "value": 1, "type": "number", "ok": True},
        ]}]}
        original = deepcopy(data)
        session._on_chart_data(data)
        root = session.variables.topLevelItem(0)
        assert root.childCount() == 10
        assert not root.isExpanded()
        assert root.text(2) == "1"
        assert root.child(0).text(2) == "0"
        assert root.child(1).text(2) == "0"
        assert root.child(2).text(2) == "1"
        line, points = session.plot.listDataItems()
        assert line.opts["stepMode"] == "right"
        assert line.name() == "Ступень"
        assert line.opts["pen"].color().name() == "#32aaff"
        assert list(line.yData) == [2.5, 3.5, 3.5]
        assert list(points.yData) == [2.5, 3.5]
        assert list(points.xData) == [0, 1]
        assert data == original
        assert session._statistics["x"]["max"] == 1
        path = tmp_path / "session.json"
        save_session(path, host="localhost", port=10000, poll_interval_ms=500,
                     history_limit=5000, expressions=["x"], history_expressions=["x"],
                     chart_data=session._last_chart_data, statistics=session._statistics,
                     series_styles=session._series_styles())
        restored.load_document(load_session(path))
        assert restored._series_styles() == {"x": {**style, "chart_type": "step_post"}}
        assert list(restored.plot.listDataItems()[0].yData) == [2.5, 3.5, 3.5]
        # Editing presentation settings redraws immediately without altering samples.
        session.variables.itemWidget(root.child(7), 2).setValue(-1)
        assert list(session.plot.listDataItems()[0].yData) == [-1, 0, 0]
        session.variables.itemWidget(root.child(8), 2).setChecked(False)
        assert len(session.plot.listDataItems()) == 1
        # Removing a selected property removes its owning expression.
        root.child(1).setSelected(True)
        session._remove_expressions()
        assert session._expressions() == []
        assert session.plot.listDataItems() == []
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


def test_pulse_counter_is_plotted_and_restored(tmp_path) -> None:
    from ptusa_lua_debugger.session_store import load_session, save_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        definition = PulseDefinition("Партия", "left", "right", 1, 1)
        session._pulse_counters.add(definition)
        session._create_expression("left", history_enabled=True, style={"offset": -0.5})
        session._create_expression("right", history_enabled=True, style={"offset": 1.5})
        session._create_expression(definition.expression, history_enabled=True)
        data = {
            "server_time_ms": 1040,
            "controller_time_unix_ms": 10_000,
            "controller_time_millisec": 1000,
            "series": [
                {"expression": "left", "samples": [
                    {"time_ms": time, "ok": True, "type": "number", "value": value}
                    for time, value in [(1000, 0), (1010, 1), (1020, 0), (1030, 1)]
                ]},
                {"expression": "right", "samples": [
                    {"time_ms": 1000, "ok": True, "type": "number", "value": 0},
                    {"time_ms": 1040, "ok": True, "type": "number", "value": 1},
                ]},
            ],
        }
        session._on_chart_data(data)
        assert session._expressions() == ["left", "right"]
        assert [s["value"] for s in session._last_chart_data["series"][-1]["samples"]] == [2]
        assert session.variables.topLevelItem(2).text(2) == "2"
        assert definition.expression in session.history_model.expressions
        assert any(curve.name() == definition.expression for curve in session.plot.listDataItems())
        path = tmp_path / "pulse.ptlua.json"
        save_session(path, host="localhost", port=10000, poll_interval_ms=500,
                     history_limit=5000, expressions=session._expressions(),
                     history_expressions=session._history_expressions(),
                     chart_data=session._last_chart_data, statistics=session._statistics,
                     series_styles=session._series_styles(),
                     pulse_definitions=[vars(definition)],
                     pulse_state=session._pulse_counters.snapshot())
        restored.load_document(load_session(path))
        assert restored._pulse_counters.states[definition.expression].count == 0
        assert restored._expressions() == ["left", "right"]
        assert any(curve.name() == definition.expression for curve in restored.plot.listDataItems())
        # Removing a source also removes the computed value that uses it.
        restored.variables.topLevelItem(0).setSelected(True)
        restored._remove_expressions()
        assert definition.expression not in restored._pulse_counters.definitions
        assert restored._expressions() == ["right"]
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()
