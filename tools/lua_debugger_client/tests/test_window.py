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
