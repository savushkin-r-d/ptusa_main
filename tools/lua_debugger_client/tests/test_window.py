from __future__ import annotations

import os
import pytest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication

from ptusa_lua_debugger.window import DebuggerSessionWidget, MainWindow
from ptusa_lua_debugger.pulse_counter import PulseDefinition


@pytest.mark.parametrize("chart_type, expected_x, expected_y", [
    ("step_post", [1, 3, 3, 6, 6, 8, 8, 8], [0, 0, 1, 1, 0, 0, 0, 0]),
    ("step_pre", [1, 1, 1, 3, 3, 6, 6, 8], [0, 0, 1, 1, 0, 0, 0, 0]),
    ("step_mid", [1, 2, 2, 4.5, 4.5, 8], [0, 0, 1, 1, 0, 0]),
    ("line", [1, 3, 6, 8], [0, 1, 0, 0]),
    ("scatter", [1, 3, 6], [0, 1, 0]),
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
            assert list(curve.xData) == ([1] if single_sample else expected_x)
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
        assert list(points.xData) == [1, 2]
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


def test_controller_commands_are_available_in_each_session() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.controller_command_requested.disconnect(
            session._worker.execute_controller_command
        )
        sent: list[int] = []
        session.controller_command_requested.connect(sent.append)
        assert [session.command_combo.itemData(i)
                for i in range(session.command_combo.count())
                ] == [103, 104, 102, 100, 101, 0]
        assert not session.command_button.isEnabled()

        session._connected = True
        session.command_button.setEnabled(True)
        session.command_combo.setCurrentIndex(session.command_combo.findData(102))
        session.command_button.click()
        assert sent == [102]
        assert not session.command_button.isEnabled()
        session._on_command_executed(102, {"ok": True, "queued": True})
        assert session.command_button.isEnabled()
        assert session.command_result.text() == "Сохранение запланировано"
    finally:
        session.shutdown()
        session.deleteLater()
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


def test_timeline_selector_switches_chart_time_axis() -> None:
    import pyqtgraph as pg
    from datetime import datetime
    from PySide6.QtCore import Qt

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._create_expression("x", history_enabled=True)
        session._on_chart_data(
            {
                "server_time_ms": 8000,
                "controller_time_unix_ms": 1_789_123_456_000,
                "controller_time_millisec": 1000,
                "client_time_unix_ms": 1_700_000_000_000,
                "client_time_millisec": 1000,
                "series": [
                    {
                        "expression": "x",
                        "samples": [
                            {"time_ms": 1000, "value": 0, "type": "number",
                             "ok": True},
                            {"time_ms": 3000, "value": 1, "type": "number",
                             "ok": True},
                            {"time_ms": 6000, "value": 0, "type": "number",
                             "ok": True},
                        ],
                    }
                ],
            }
        )

        model = session.history_model
        assert model.headerData(0, Qt.Horizontal) == "Реальное время"
        assert model.headerData(1, Qt.Horizontal) == "Время контроллера"
        assert model.headerData(2, Qt.Horizontal) == "x"
        assert model.data(model.index(0, 0)) == datetime.fromtimestamp(
            1_700_000_000_000 / 1000
        ).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
        assert model.data(model.index(0, 1)) == datetime.fromtimestamp(
            1_789_123_456_000 / 1000
        ).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]

        # Controller Unix-seconds axis is the default; the last point is the
        # held current-time value at server_time_ms.
        assert session.timeline_combo.currentData() == "controller"
        axis = session.plot.getAxis("bottom")
        assert isinstance(axis, pg.DateAxisItem)
        assert axis.labelText == "Время контроллера"
        curve = session.plot.listDataItems()[0]
        assert list(curve.xData) == [
            1_789_123_456.0,
            1_789_123_458.0,
            1_789_123_461.0,
            1_789_123_463.0,
        ]

        session.timeline_combo.setCurrentIndex(
            session.timeline_combo.findData("real")
        )
        axis = session.plot.getAxis("bottom")
        assert isinstance(axis, pg.DateAxisItem)
        assert axis.labelText == "Реальное время"
        curve = session.plot.listDataItems()[0]
        assert list(curve.xData) == [
            1_700_000_000.0,
            1_700_000_002.0,
            1_700_000_005.0,
            1_700_000_007.0,
        ]
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_periodic_logging_saves_and_starts_new_history(tmp_path) -> None:
    from copy import deepcopy
    from ptusa_lua_debugger.session_store import load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._create_expression("x", history_enabled=True)
        session._on_chart_data(
            {
                "server_time_ms": 8000,
                "controller_time_unix_ms": 1_789_123_456_000,
                "controller_time_millisec": 1000,
                "series": [
                    {
                        "expression": "x",
                        "samples": [
                            {"time_ms": 1000, "value": 0, "type": "number",
                             "ok": True},
                            {"time_ms": 3000, "value": 1, "type": "number",
                             "ok": True},
                            {"time_ms": 6000, "value": 0, "type": "number",
                             "ok": True},
                        ],
                    }
                ],
            }
        )
        saved_chart_data = deepcopy(session._last_chart_data)
        saved_statistics = deepcopy(session._statistics)
        session.logging_directory_edit.setText(str(tmp_path))
        session.logging_check.setChecked(True)
        assert session._logging_timer.isActive()
        assert session._logging_timer.interval() == 60 * 60_000

        session._rotate_log()

        files = list(tmp_path.glob("*.ptlua.json"))
        assert len(files) == 1
        document = load_session(files[0])
        assert document["chart_data"] == saved_chart_data
        assert document["statistics"] == saved_statistics
        assert session._last_chart_data is None
        assert session._statistics == {}
        assert session.history_model.rows == []
        assert session._logging_timer.isActive()
    finally:
        session.logging_check.setChecked(False)
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_failed_periodic_log_preserves_history_and_stops_logging(
    tmp_path, monkeypatch
) -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    errors: list[str] = []
    try:
        session._create_expression("x", history_enabled=True)
        session._on_chart_data(
            {
                "server_time_ms": 8000,
                "series": [
                    {
                        "expression": "x",
                        "samples": [
                            {"time_ms": 1000, "value": 0, "type": "number",
                             "ok": True},
                            {"time_ms": 3000, "value": 1, "type": "number",
                             "ok": True},
                        ],
                    }
                ],
            }
        )
        saved_chart_data = session._last_chart_data
        saved_statistics = session._statistics
        session.logging_directory_edit.setText(str(tmp_path))
        session.logging_check.setChecked(True)
        assert session._logging_timer.isActive()

        def fail_save(*args, **kwargs):
            raise OSError("disk full")

        monkeypatch.setattr(
            "ptusa_lua_debugger.window.save_session", fail_save
        )
        session._show_error = errors.append

        session._rotate_log()

        assert session._last_chart_data is saved_chart_data
        assert session._statistics is saved_statistics
        assert not session._logging_timer.isActive()
        assert not session.logging_check.isChecked()
        assert len(errors) == 1
        assert "disk full" in errors[0]
    finally:
        session.logging_check.setChecked(False)
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_auto_reconnect_backoff_progression() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.connect_requested.disconnect(session._worker.connect_to)
        attempts: list[tuple[str, int]] = []
        session.connect_requested.connect(
            lambda host, port: attempts.append((host, port))
        )
        session.host_edit.setText("10.0.0.5")
        session.port_spin.setValue(12_345)
        session.auto_reconnect_check.setChecked(True)

        expected = [1_000 + 5_000 * step for step in range(12)]
        expected += [60_000, 60_000]
        intervals = []
        session._on_disconnected("Обрыв связи")
        intervals.append(session._reconnect_timer.interval())
        assert session._reconnect_timer.isActive()
        assert session.connect_button.isEnabled()
        assert "Обрыв связи · повторное подключение через 1 с" \
            == session.status_label.text()
        while len(intervals) < len(expected):
            session._retry_connection()
            assert not session._reconnect_timer.isActive()
            session._on_disconnected("Обрыв связи")
            intervals.append(session._reconnect_timer.interval())

        assert intervals == expected
        assert attempts == [("10.0.0.5", 12_345)] * (len(expected) - 1)
        assert session._reconnect_timer.isActive()
        assert "повторное подключение через 60 с" in session.status_label.text()
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_auto_reconnect_reset_and_manual_cancellation() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.connect_requested.disconnect(session._worker.connect_to)
        session.disconnect_requested.disconnect(session._worker.disconnect)
        attempts: list[tuple[str, int]] = []
        disconnects: list[bool] = []
        session.connect_requested.connect(
            lambda host, port: attempts.append((host, port))
        )
        session.disconnect_requested.connect(lambda: disconnects.append(True))
        session.auto_reconnect_check.setChecked(True)

        # A scheduled retry is cancelled by a manual connect, which starts a
        # fresh backoff cycle.
        session._on_disconnected("Обрыв связи")
        assert session._reconnect_timer.isActive()
        session._toggle_connection()
        assert not session._reconnect_timer.isActive()
        assert session._connection_pending
        assert session._reconnect_delay_seconds == 1
        assert attempts == [("127.0.0.1", 10_000)]
        session._on_disconnected("Отказано")
        assert session._reconnect_timer.interval() == 1_000

        # A successful connection cancels the retry and resets the backoff.
        session._retry_connection()
        session._on_connected("session-1")
        assert not session._reconnect_timer.isActive()
        assert not session._connection_pending
        assert session._reconnect_delay_seconds == 1
        assert session._scheduled_reconnect_delay_seconds is None
        assert session._last_disconnect_reason == ""
        assert session.connect_button.text() == "Отключиться"

        # Manual disconnect emits an empty reason and never schedules a retry.
        session._toggle_connection()
        assert disconnects == [True]
        session._on_disconnected("")
        assert not session._reconnect_timer.isActive()
        assert session._last_disconnect_reason == ""
        assert session.status_label.text() == "Не подключено"
        assert session.connect_button.text() == "Подключиться"
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_auto_reconnect_toggle_preserves_pending_delay() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.connect_requested.disconnect(session._worker.connect_to)

        # A failure with the option off does not schedule anything; enabling
        # the option schedules the first applicable delay.
        session._on_disconnected("Обрыв связи")
        assert not session._reconnect_timer.isActive()
        session.auto_reconnect_check.setChecked(True)
        assert session._reconnect_timer.isActive()
        assert session._reconnect_timer.interval() == 1_000
        assert "повторное подключение через 1 с" in session.status_label.text()

        # Toggling the option off/on reuses the already chosen delay and does
        # not advance the backoff.
        for _ in range(2):
            session.auto_reconnect_check.setChecked(False)
            assert not session._reconnect_timer.isActive()
            assert session.status_label.text() == "Обрыв связи"
            session.auto_reconnect_check.setChecked(True)
            assert session._reconnect_timer.isActive()
            assert session._reconnect_timer.interval() == 1_000

        # The next failed attempt advances to the following delay exactly once.
        session._retry_connection()
        session._on_disconnected("Обрыв связи")
        assert session._reconnect_timer.interval() == 6_000
        assert "повторное подключение через 6 с" in session.status_label.text()
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_shutdown_stops_reconnect_timer() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    session.auto_reconnect_check.setChecked(True)
    session._on_disconnected("Обрыв связи")
    assert session._reconnect_timer.isActive()
    session.shutdown()
    assert not session._reconnect_timer.isActive()
    session.deleteLater()
    application.processEvents()
