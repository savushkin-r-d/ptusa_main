from __future__ import annotations

from typing import Any

import pyqtgraph as pg
from PySide6.QtCore import QMetaObject, Qt, QThread, Signal, Slot
from PySide6.QtGui import QAction, QCloseEvent
from PySide6.QtWidgets import (
    QAbstractItemView,
    QFileDialog,
    QHBoxLayout,
    QHeaderView,
    QLabel,
    QLineEdit,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QSpinBox,
    QSplitter,
    QStatusBar,
    QTableWidget,
    QTableWidgetItem,
    QVBoxLayout,
    QWidget,
)

from .history import (
    DEFAULT_HISTORY_LIMIT,
    MAX_HISTORY_LIMIT,
    merge_chart_data,
    trim_chart_data,
)
from .session_store import load_session, save_session
from .worker import DebuggerWorker


class MainWindow(QMainWindow):
    connect_requested = Signal(str, int)
    disconnect_requested = Signal()
    expressions_requested = Signal(list)
    interval_requested = Signal(int)
    evaluate_requested = Signal(str)
    clear_requested = Signal()

    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("ptusa Lua debugger")
        self.resize(1180, 760)
        self._connected = False
        self._last_chart_data: dict[str, Any] | None = None

        self._thread = QThread(self)
        self._worker = DebuggerWorker()
        self._worker.moveToThread(self._thread)
        self.connect_requested.connect(self._worker.connect_to)
        self.disconnect_requested.connect(self._worker.disconnect)
        self.expressions_requested.connect(self._worker.set_expressions)
        self.interval_requested.connect(self._worker.set_interval)
        self.evaluate_requested.connect(self._worker.evaluate)
        self.clear_requested.connect(self._worker.clear_chart_data)
        self._worker.connected.connect(self._on_connected)
        self._worker.disconnected.connect(self._on_disconnected)
        self._worker.chart_data.connect(self._on_chart_data)
        self._worker.evaluated.connect(self._on_evaluated)
        self._worker.error.connect(self._show_error)
        self._thread.start()

        self._build_ui()

    def _build_ui(self) -> None:
        file_menu = self.menuBar().addMenu("Сессия")
        load_action = QAction("Загрузить…", self)
        save_action = QAction("Сохранить…", self)
        load_action.triggered.connect(self._load_session)
        save_action.triggered.connect(self._save_session)
        file_menu.addAction(load_action)
        file_menu.addAction(save_action)

        self.host_edit = QLineEdit("127.0.0.1")
        self.port_spin = QSpinBox()
        self.port_spin.setRange(1, 65_535)
        self.port_spin.setValue(10_000)
        self.interval_spin = QSpinBox()
        self.interval_spin.setRange(100, 9_000)
        self.interval_spin.setSingleStep(100)
        self.interval_spin.setSuffix(" мс")
        self.interval_spin.setValue(500)
        self.interval_spin.valueChanged.connect(self.interval_requested)
        self.history_limit_spin = QSpinBox()
        self.history_limit_spin.setRange(1, MAX_HISTORY_LIMIT)
        self.history_limit_spin.setSingleStep(1_000)
        self.history_limit_spin.setValue(DEFAULT_HISTORY_LIMIT)
        self.history_limit_spin.setToolTip("Максимум точек на каждое выражение")
        self.history_limit_spin.valueChanged.connect(self._history_limit_changed)
        self.connect_button = QPushButton("Подключиться")
        self.connect_button.clicked.connect(self._toggle_connection)

        connection = QHBoxLayout()
        connection.addWidget(QLabel("Контроллер:"))
        connection.addWidget(self.host_edit, 1)
        connection.addWidget(QLabel("Порт:"))
        connection.addWidget(self.port_spin)
        connection.addWidget(QLabel("Опрос:"))
        connection.addWidget(self.interval_spin)
        connection.addWidget(QLabel("История:"))
        connection.addWidget(self.history_limit_spin)
        connection.addWidget(self.connect_button)

        self.expression_edit = QLineEdit()
        self.expression_edit.setPlaceholderText("Например: TE1:get_value()")
        self.expression_edit.returnPressed.connect(self._add_expression)
        add_button = QPushButton("Добавить")
        add_button.clicked.connect(self._add_expression)
        remove_button = QPushButton("Удалить")
        remove_button.clicked.connect(self._remove_expressions)
        apply_button = QPushButton("Применить")
        apply_button.clicked.connect(self._apply_expressions)
        clear_button = QPushButton("Очистить графики")
        clear_button.clicked.connect(self._clear_charts)

        expression_buttons = QHBoxLayout()
        expression_buttons.addWidget(self.expression_edit, 1)
        expression_buttons.addWidget(add_button)
        expression_buttons.addWidget(remove_button)
        expression_buttons.addWidget(apply_button)
        expression_buttons.addWidget(clear_button)

        self.variables = QTableWidget(0, 3)
        self.variables.setHorizontalHeaderLabels(["Lua-выражение", "Значение", "Состояние"])
        self.variables.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.variables.setSelectionMode(QAbstractItemView.ExtendedSelection)
        self.variables.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)
        self.variables.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.variables.horizontalHeader().setSectionResizeMode(2, QHeaderView.ResizeToContents)

        left = QWidget()
        left_layout = QVBoxLayout(left)
        left_layout.addLayout(expression_buttons)
        left_layout.addWidget(self.variables)

        pg.setConfigOptions(antialias=True)
        self.plot = pg.PlotWidget(background="#111827")
        self.plot.addLegend()
        self.plot.showGrid(x=True, y=True, alpha=0.2)
        self.plot.setLabel("bottom", "Время", units="s")

        splitter = QSplitter()
        splitter.addWidget(left)
        splitter.addWidget(self.plot)
        splitter.setSizes([470, 710])

        self.evaluate_edit = QLineEdit()
        self.evaluate_edit.setPlaceholderText("Разовое Lua-выражение")
        self.evaluate_edit.returnPressed.connect(self._evaluate_once)
        evaluate_button = QPushButton("Вычислить")
        evaluate_button.clicked.connect(self._evaluate_once)
        self.evaluate_result = QLabel("—")
        evaluation = QHBoxLayout()
        evaluation.addWidget(self.evaluate_edit, 1)
        evaluation.addWidget(evaluate_button)
        evaluation.addWidget(self.evaluate_result, 1)

        root = QWidget()
        root_layout = QVBoxLayout(root)
        root_layout.addLayout(connection)
        root_layout.addWidget(splitter, 1)
        root_layout.addLayout(evaluation)
        self.setCentralWidget(root)
        self.setStatusBar(QStatusBar())
        self.statusBar().showMessage("Не подключено")

    @Slot()
    def _toggle_connection(self) -> None:
        if self._connected:
            self.disconnect_requested.emit()
            return
        self.connect_button.setEnabled(False)
        self.statusBar().showMessage("Подключение…")
        self.interval_requested.emit(self.interval_spin.value())
        self.connect_requested.emit(self.host_edit.text().strip(), self.port_spin.value())

    @Slot(str)
    def _on_connected(self, session_id: str) -> None:
        self._connected = True
        self.connect_button.setEnabled(True)
        self.connect_button.setText("Отключиться")
        self.statusBar().showMessage(f"Подключено · сессия {session_id}")
        self._apply_expressions()

    @Slot(str)
    def _on_disconnected(self, reason: str) -> None:
        self._connected = False
        self.connect_button.setEnabled(True)
        self.connect_button.setText("Подключиться")
        self.statusBar().showMessage(reason or "Не подключено")

    @Slot()
    def _add_expression(self) -> None:
        expression = self.expression_edit.text().strip()
        if not expression:
            return
        if expression in self._expressions():
            self.expression_edit.clear()
            return
        row = self.variables.rowCount()
        self.variables.insertRow(row)
        self.variables.setItem(row, 0, QTableWidgetItem(expression))
        self.variables.setItem(row, 1, QTableWidgetItem("—"))
        self.variables.setItem(row, 2, QTableWidgetItem("ожидание"))
        self.expression_edit.clear()
        self._apply_expressions()

    @Slot()
    def _remove_expressions(self) -> None:
        rows = sorted({item.row() for item in self.variables.selectedItems()}, reverse=True)
        for row in rows:
            self.variables.removeRow(row)
        self._apply_expressions()

    def _expressions(self) -> list[str]:
        return [
            self.variables.item(row, 0).text()
            for row in range(self.variables.rowCount())
            if self.variables.item(row, 0)
        ]

    @Slot()
    def _apply_expressions(self) -> None:
        if self._connected:
            self.expressions_requested.emit(self._expressions())

    @Slot()
    def _evaluate_once(self) -> None:
        expression = self.evaluate_edit.text().strip()
        if expression:
            self.evaluate_requested.emit(expression)

    @Slot(str, dict)
    def _on_evaluated(self, expression: str, result: dict[str, Any]) -> None:
        if result.get("ok"):
            self.evaluate_result.setText(f"{result.get('type')}: {result.get('value')}")
        else:
            self.evaluate_result.setText(str(result.get("error", "Ошибка")))

    @Slot(dict)
    def _on_chart_data(self, data: dict[str, Any]) -> None:
        self._last_chart_data = merge_chart_data(
            self._last_chart_data, data, self.history_limit_spin.value()
        )
        by_expression = {
            item.get("expression"): item
            for item in self._last_chart_data.get("series", [])
        }
        for row, expression in enumerate(self._expressions()):
            series = by_expression.get(expression, {})
            samples = series.get("samples", [])
            if not samples:
                continue
            last = samples[-1]
            self.variables.setItem(row, 1, QTableWidgetItem(str(last.get("value"))))
            status = "OK" if last.get("ok") else str(last.get("value", "ошибка"))
            self.variables.setItem(row, 2, QTableWidgetItem(status))
        self._draw_chart(self._last_chart_data)

    @Slot(int)
    def _history_limit_changed(self, limit: int) -> None:
        self._last_chart_data = trim_chart_data(self._last_chart_data, limit)
        if self._last_chart_data is not None:
            self._draw_chart(self._last_chart_data)

    @Slot()
    def _clear_charts(self) -> None:
        self._last_chart_data = None
        self.plot.clear()
        if self._connected:
            self.clear_requested.emit()

    def _draw_chart(self, data: dict[str, Any]) -> None:
        self.plot.clear()
        server_time = int(data.get("server_time_ms", 0))
        prepared: list[tuple[dict[str, Any], list[dict[str, Any]]]] = []
        for series in data.get("series", []):
            numeric = [
                sample for sample in series.get("samples", [])
                if sample.get("ok") and sample.get("type") in {"number", "boolean"}
            ]
            if numeric:
                prepared.append((series, numeric))
        if not prepared:
            return

        base = int(prepared[0][1][0]["time_ms"])
        for index, (series, numeric) in enumerate(prepared):
            x_values = [((int(sample["time_ms"]) - base) & 0xFFFFFFFF) / 1000 for sample in numeric]
            y_values = [float(sample["value"]) for sample in numeric]
            current_x = ((server_time - base) & 0xFFFFFFFF) / 1000
            if current_x > x_values[-1]:
                x_values.append(current_x)
                y_values.append(y_values[-1])
            self.plot.plot(
                x_values,
                y_values,
                name=str(series.get("expression", "")),
                pen=pg.mkPen(pg.intColor(index), width=2),
                stepMode="left",
            )

    @Slot(str)
    def _show_error(self, message: str) -> None:
        self.statusBar().showMessage(message)
        QMessageBox.warning(self, "Lua debugger", message)

    @Slot()
    def _save_session(self) -> None:
        path, _ = QFileDialog.getSaveFileName(
            self, "Сохранить сессию", "", "Lua debugger session (*.ptlua.json)"
        )
        if not path:
            return
        try:
            save_session(
                path,
                host=self.host_edit.text().strip(),
                port=self.port_spin.value(),
                poll_interval_ms=self.interval_spin.value(),
                history_limit=self.history_limit_spin.value(),
                expressions=self._expressions(),
                chart_data=self._last_chart_data,
            )
        except OSError as exc:
            self._show_error(str(exc))

    @Slot()
    def _load_session(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self, "Загрузить сессию", "", "Lua debugger session (*.ptlua.json)"
        )
        if not path:
            return
        try:
            document = load_session(path)
            connection = document["connection"]
            self.host_edit.setText(str(connection.get("host", "127.0.0.1")))
            self.port_spin.setValue(int(connection.get("port", 10_000)))
            self.interval_spin.setValue(int(document["poll_interval_ms"]))
            self.history_limit_spin.setValue(int(document["history_limit"]))
            self.variables.setRowCount(0)
            for expression in document["expressions"]:
                row = self.variables.rowCount()
                self.variables.insertRow(row)
                self.variables.setItem(row, 0, QTableWidgetItem(expression))
                self.variables.setItem(row, 1, QTableWidgetItem("—"))
                self.variables.setItem(row, 2, QTableWidgetItem("ожидание"))
            chart_data = document.get("chart_data")
            self._last_chart_data = None
            if isinstance(chart_data, dict):
                self._on_chart_data(chart_data)
            self._apply_expressions()
        except (OSError, ValueError, TypeError) as exc:
            self._show_error(str(exc))

    def closeEvent(self, event: QCloseEvent) -> None:
        QMetaObject.invokeMethod(
            self._worker, "shutdown", Qt.BlockingQueuedConnection
        )
        self._thread.quit()
        self._thread.wait(3_000)
        event.accept()
