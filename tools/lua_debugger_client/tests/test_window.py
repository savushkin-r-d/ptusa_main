from __future__ import annotations

import os

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication

from ptusa_lua_debugger.window import DebuggerSessionWidget, MainWindow


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
