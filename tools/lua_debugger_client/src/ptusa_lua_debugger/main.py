from __future__ import annotations

import sys

from PySide6.QtWidgets import QApplication
from qt_material import apply_stylesheet

from ptusa_lua_debugger.window import MainWindow


def main() -> int:
    application = QApplication(sys.argv)
    application.setApplicationName("ptusa Lua debugger")
    apply_stylesheet(application, theme="dark_teal.xml")
    window = MainWindow()
    window.show()
    return application.exec()


if __name__ == "__main__":
    raise SystemExit(main())
