from __future__ import annotations

import json
from datetime import datetime
from typing import Any

import pyqtgraph as pg
from PySide6.QtCore import QMetaObject, Qt, QThread, Signal, Slot
from PySide6.QtGui import QAction, QCloseEvent, QKeySequence
from PySide6.QtWidgets import (
    QAbstractItemView,
    QCheckBox,
    QComboBox,
    QColorDialog,
    QDoubleSpinBox,
    QDialog,
    QFormLayout,
    QTreeWidget,
    QTreeWidgetItem,
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
    QTableWidget,
    QTableWidgetItem,
    QTableView,
    QTabWidget,
    QToolButton,
    QVBoxLayout,
    QWidget,
)
from xlsxwriter.exceptions import XlsxWriterException

from .excel_export import export_history_xlsx
from .chart_styles import CHART_TYPES, DEFAULT_CHART_TYPE
from .history import (
    DEFAULT_DISPLAY_SECONDS,
    DEFAULT_HISTORY_LIMIT,
    MAX_DISPLAY_SECONDS,
    MAX_HISTORY_LIMIT,
    controller_timestamp_ms,
    merge_chart_data,
    merge_statistics,
    trim_chart_data,
)
from .history_table import HistoryTableModel
from .pulse_counter import PulseCounters, PulseDefinition
from .session_store import load_session, save_session
from .worker import DebuggerWorker

CONTROLLER_COMMANDS = (
    (103, "PHOENIX Modbus UDP: включить"),
    (104, "PHOENIX Modbus UDP: выключить"),
    (102, "Принудительно сохранить параметры"),
    (100, "Перезагрузить ограничения"),
    (101, "Сбросить параметры"),
    (0, "Очистить код результата"),
)


class DebuggerSessionWidget(QWidget):
    connect_requested = Signal(str, int)
    disconnect_requested = Signal()
    expressions_requested = Signal(list)
    interval_requested = Signal(int)
    evaluate_requested = Signal(str)
    controller_command_requested = Signal(int)
    clear_requested = Signal()
    title_changed = Signal(str)

    def __init__(self) -> None:
        super().__init__()
        self._connected = False
        self._shutting_down = False
        self._last_chart_data: dict[str, Any] | None = None
        self._statistics: dict[str, dict[str, Any]] = {}
        self._pulse_counters = PulseCounters()

        self._thread = QThread(self)
        self._worker = DebuggerWorker()
        self._worker.moveToThread(self._thread)
        self.connect_requested.connect(self._worker.connect_to)
        self.disconnect_requested.connect(self._worker.disconnect)
        self.expressions_requested.connect(self._worker.set_expressions)
        self.interval_requested.connect(self._worker.set_interval)
        self.evaluate_requested.connect(self._worker.evaluate)
        self.controller_command_requested.connect(
            self._worker.execute_controller_command
        )
        self.clear_requested.connect(self._worker.clear_chart_data)
        self._worker.connected.connect(self._on_connected)
        self._worker.disconnected.connect(self._on_disconnected)
        self._worker.chart_data.connect(self._on_chart_data)
        self._worker.messages.connect(self._on_messages)
        self._worker.evaluated.connect(self._on_evaluated)
        self._worker.command_executed.connect(self._on_command_executed)
        self._worker.error.connect(self._show_error)
        self._thread.start()

        self._build_ui()

    def _build_ui(self) -> None:
        self.host_edit = QLineEdit("127.0.0.1")
        self.host_edit.textChanged.connect(self._update_title)
        self.port_spin = QSpinBox()
        self.port_spin.setRange(1, 65_535)
        self.port_spin.setValue(10_000)
        self.port_spin.valueChanged.connect(self._update_title)
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
        self.display_seconds_spin = QSpinBox()
        self.display_seconds_spin.setRange(1, MAX_DISPLAY_SECONDS)
        self.display_seconds_spin.setSuffix(" с")
        self.display_seconds_spin.setValue(DEFAULT_DISPLAY_SECONDS)
        self.display_seconds_spin.setToolTip(
            "Интервал, отображаемый на графике"
        )
        self.display_seconds_spin.valueChanged.connect(self._display_settings_changed)
        self.auto_follow_check = QCheckBox("Авто")
        self.auto_follow_check.setChecked(True)
        self.auto_follow_check.setToolTip(
            "Автоматически показывать последние N секунд"
        )
        self.auto_follow_check.toggled.connect(self._display_settings_changed)
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
        connection.addWidget(QLabel("Окно:"))
        connection.addWidget(self.display_seconds_spin)
        connection.addWidget(self.auto_follow_check)
        connection.addWidget(self.connect_button)

        self.expression_edit = QLineEdit()
        self.expression_edit.setPlaceholderText("Например: TE1:get_value()")
        self.expression_edit.returnPressed.connect(self._add_expression)
        add_button = QPushButton("Добавить")
        add_button.clicked.connect(self._add_expression)
        pulse_button = QPushButton("Счётчик импульсов…")
        pulse_button.clicked.connect(self._add_pulse_counter)
        remove_button = QPushButton("Удалить")
        remove_button.clicked.connect(self._remove_expressions)
        apply_button = QPushButton("Применить")
        apply_button.clicked.connect(self._apply_expressions)
        clear_button = QPushButton("Очистить историю")
        clear_button.clicked.connect(self._clear_charts)

        expression_buttons = QHBoxLayout()
        expression_buttons.addWidget(add_button)
        expression_buttons.addWidget(pulse_button)
        expression_buttons.addWidget(remove_button)
        expression_buttons.addWidget(apply_button)
        expression_buttons.addWidget(clear_button)

        self.variables = QTreeWidget()
        self.variables.setColumnCount(4)
        self.variables.setHeaderLabels(
            ["Lua-выражение / свойство", "История", "Значение", "Состояние"]
        )
        self.variables.setSelectionMode(QAbstractItemView.ExtendedSelection)
        self.variables.header().setSectionResizeMode(0, QHeaderView.Interactive)
        self.variables.setColumnWidth(0, 300)
        for column in range(1, 4):
            self.variables.header().setSectionResizeMode(column, QHeaderView.ResizeToContents)
        self.variables.setMinimumWidth(420)
        self.variables.itemChanged.connect(self._history_changed)

        left = QWidget()
        left_layout = QVBoxLayout(left)
        left_layout.addWidget(self.expression_edit)
        left_layout.addLayout(expression_buttons)
        left_layout.addWidget(self.variables)

        pg.setConfigOptions(antialias=True, foreground="#CFD8DC")
        self.plot = pg.PlotWidget(
            background="#1C252A",
            axisItems={"bottom": pg.DateAxisItem(orientation="bottom")},
        )
        self._absolute_time_axis = True
        self.plot.addLegend()
        self.plot.showGrid(x=True, y=True, alpha=0.2)
        self.plot.setLabel("bottom", "Время контроллера")

        self.history_model = HistoryTableModel()
        self.history_table = QTableView()
        self.history_table.setModel(self.history_model)
        self.history_table.setAlternatingRowColors(True)
        self.history_table.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.history_table.setColumnWidth(0, 190)
        self.history_table.horizontalHeader().setStretchLastSection(True)
        export_button = QPushButton("Экспорт в Excel…")
        export_button.clicked.connect(self._export_history)
        history_page = QWidget()
        history_layout = QVBoxLayout(history_page)
        history_layout.addWidget(export_button, 0, Qt.AlignRight)
        history_layout.addWidget(self.history_table, 1)

        self.messages_table = QTableWidget(0, 4)
        self.messages_table.setHorizontalHeaderLabels(
            ["Время", "Источник", "Уровень", "Сообщение"]
        )
        self.messages_table.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.messages_table.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.messages_table.horizontalHeader().setSectionResizeMode(
            0, QHeaderView.ResizeToContents
        )
        self.messages_table.horizontalHeader().setSectionResizeMode(
            1, QHeaderView.ResizeToContents
        )
        self.messages_table.horizontalHeader().setSectionResizeMode(
            2, QHeaderView.ResizeToContents
        )
        self.messages_table.horizontalHeader().setSectionResizeMode(
            3, QHeaderView.Stretch
        )
        clear_messages_button = QPushButton("Очистить")
        clear_messages_button.clicked.connect(
            lambda: self.messages_table.setRowCount(0)
        )
        messages_page = QWidget()
        messages_layout = QVBoxLayout(messages_page)
        messages_layout.addWidget(clear_messages_button, 0, Qt.AlignRight)
        messages_layout.addWidget(self.messages_table, 1)

        self.output_tabs = QTabWidget()
        self.output_tabs.addTab(self.plot, "График")
        self.output_tabs.addTab(history_page, "История")
        self.output_tabs.addTab(messages_page, "Сообщения")

        splitter = QSplitter()
        splitter.addWidget(left)
        splitter.addWidget(self.output_tabs)
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

        self.command_combo = QComboBox()
        for command_id, label in CONTROLLER_COMMANDS:
            self.command_combo.addItem(label, command_id)
        self.command_button = QPushButton("Выполнить")
        self.command_button.setEnabled(False)
        self.command_button.clicked.connect(self._execute_controller_command)
        self.command_result = QLabel("—")
        commands = QHBoxLayout()
        commands.addWidget(QLabel("Команда контроллера:"))
        commands.addWidget(self.command_combo)
        commands.addWidget(self.command_button)
        commands.addWidget(self.command_result, 1)

        root_layout = QVBoxLayout(self)
        root_layout.addLayout(connection)
        root_layout.addWidget(splitter, 1)
        root_layout.addLayout(evaluation)
        root_layout.addLayout(commands)
        self.status_label = QLabel("Не подключено")
        root_layout.addWidget(self.status_label)

    @Slot()
    def _toggle_connection(self) -> None:
        if self._connected:
            self.disconnect_requested.emit()
            return
        self.connect_button.setEnabled(False)
        self.status_label.setText("Подключение…")
        self.interval_requested.emit(self.interval_spin.value())
        self.connect_requested.emit(self.host_edit.text().strip(), self.port_spin.value())

    @Slot(str)
    def _on_connected(self, session_id: str) -> None:
        self._connected = True
        self.connect_button.setEnabled(True)
        self.connect_button.setText("Отключиться")
        self.command_button.setEnabled(True)
        self.status_label.setText(f"Подключено · сессия {session_id}")
        self._update_title()
        self._apply_expressions()

    @Slot(str)
    def _on_disconnected(self, reason: str) -> None:
        self._connected = False
        self.connect_button.setEnabled(True)
        self.connect_button.setText("Подключиться")
        self.command_button.setEnabled(False)
        if self.command_result.text() == "Выполнение…":
            self.command_result.setText(reason or "Нет подключения")
        self.status_label.setText(reason or "Не подключено")
        self._update_title()

    @Slot()
    def _add_expression(self) -> None:
        expression = self.expression_edit.text().strip()
        if not expression:
            return
        if expression in [item.text(0) for item in self._expression_items()]:
            self.expression_edit.clear()
            return
        self._create_expression(expression)
        self.expression_edit.clear()
        self._apply_expressions()

    @Slot()
    def _add_pulse_counter(self) -> None:
        dialog = QDialog(self)
        dialog.setWindowTitle("Счётчик импульсов")
        form = QFormLayout(dialog)
        name = QLineEdit()
        name.setPlaceholderText("Например: левый датчик")
        source = QComboBox()
        dependent = QComboBox()
        for combo in (source, dependent):
            combo.setEditable(True)
            combo.addItems(self._expressions())
            combo.setCurrentText("")
        source_value = QLineEdit("1")
        dependent_value = QLineEdit("1")
        for edit in (source_value, dependent_value):
            edit.setToolTip('Число, true/false или строка в кавычках, например "RUN"')
        form.addRow("Имя", name)
        form.addRow("Считаемый датчик", source)
        form.addRow("Её значение", source_value)
        form.addRow("Ведомый датчик", dependent)
        form.addRow("Её значение", dependent_value)
        form.addRow(QLabel("Первый импульс ведомого датчика фиксирует число импульсов\n"
                           "считаемого датчика. График показывает итог последней серии.\n"
                           "Указывайте значения датчиков без сдвига линии по Y."))
        create = QPushButton("Создать")
        form.addRow(create)

        def accept() -> None:
            try:
                definition = PulseDefinition(
                    name.text().strip(), source.currentText().strip(),
                    dependent.currentText().strip(), json.loads(source_value.text()),
                    json.loads(dependent_value.text()),
                )
                if definition.expression in [item.text(0) for item in self._expression_items()]:
                    raise ValueError("Такое имя уже есть в списке величин")
                self._pulse_counters.add(definition)
            except (ValueError, json.JSONDecodeError) as exc:
                QMessageBox.warning(dialog, "Счётчик импульсов", str(exc))
                return
            for expression in (definition.source, definition.dependent):
                if expression not in self._expressions():
                    self._create_expression(expression, history_enabled=True)
            self._create_expression(definition.expression, history_enabled=True,
                                    style={"points": True})
            if self._last_chart_data is not None:
                self._pulse_counters.seed(
                    definition.expression, self._last_chart_data)
            self._apply_expressions()
            dialog.accept()

        create.clicked.connect(accept)
        dialog.exec()

    @Slot()
    def _remove_expressions(self) -> None:
        items = set()
        for item in self.variables.selectedItems():
            while item.parent() is not None:
                item = item.parent()
            items.add(item)
        removed = {item.text(0) for item in items}
        for expression, definition in self._pulse_counters.definitions.items():
            if definition.source in removed or definition.dependent in removed:
                for item in self._expression_items():
                    if item.text(0) == expression:
                        items.add(item)
                        break
        for item in items:
            self._pulse_counters.remove(item.text(0))
            self.variables.takeTopLevelItem(self.variables.indexOfTopLevelItem(item))
        self._history_changed(None)
        self._apply_expressions()

    def _expression_items(self) -> list[QTreeWidgetItem]:
        return [self.variables.topLevelItem(i)
                for i in range(self.variables.topLevelItemCount())]

    def _expressions(self) -> list[str]:
        return [item.text(0) for item in self._expression_items()
                if item.text(0) not in self._pulse_counters.definitions]

    def _create_expression(self, expression: str, *, history_enabled: bool = False,
                           style: dict[str, Any] | None = None) -> None:
        style = style or {}
        self.variables.blockSignals(True)
        try:
            item = QTreeWidgetItem(self.variables, [expression, "", "—", "ожидание"])
            item.setToolTip(0, expression)
            item.setFlags(item.flags() | Qt.ItemIsUserCheckable)
            item.setCheckState(1, Qt.Checked if history_enabled else Qt.Unchecked)
            for label in ("Предыдущее", "Min", "Max", "Среднее", "Медиана"):
                QTreeWidgetItem(item, [label, "", "—"])
            name_row = QTreeWidgetItem(item, ["Имя на графике"])
            name = QLineEdit(str(style.get("name", "")))
            name.setPlaceholderText(expression)
            self.variables.setItemWidget(name_row, 2, name)
            color_row = QTreeWidgetItem(item, ["Цвет линии"])
            color = QPushButton()
            initial_color = style.get("color") or pg.intColor(
                self.variables.topLevelItemCount() - 1).name()
            self._set_color_button(color, initial_color)
            self.variables.setItemWidget(color_row, 2, color)
            offset_row = QTreeWidgetItem(item, ["Сдвиг по Y"])
            offset = QDoubleSpinBox()
            offset.setRange(-1e12, 1e12)
            offset.setDecimals(6)
            offset.setValue(style.get("offset", 0.0))
            offset.setToolTip("На графике: исходное значение + сдвиг. История и статистика не меняются.")
            self.variables.setItemWidget(offset_row, 2, offset)
            points_row = QTreeWidgetItem(item, ["Точки на графике"])
            points = QCheckBox()
            points.setChecked(style.get("points", False))
            points.setToolTip("Показывать точки полученных изменений значения")
            self.variables.setItemWidget(points_row, 2, points)
            type_row = QTreeWidgetItem(item, ["Тип графика"])
            chart_type = QComboBox()
            for key, label in CHART_TYPES.items():
                chart_type.addItem(label, key)
            chart_type.setCurrentIndex(max(0, chart_type.findData(
                style.get("chart_type", DEFAULT_CHART_TYPE))))
            chart_type.setToolTip(
                "После точки: значение действует до следующего измерения.\n"
                "До точки: значение действует от предыдущего измерения.\n"
                "По середине: переход между значениями в середине интервала."
            )
            self.variables.setItemWidget(type_row, 2, chart_type)
            chart_type.currentIndexChanged.connect(self._redraw_chart)
            name.textChanged.connect(self._redraw_chart)
            color.clicked.connect(lambda: self._choose_color(color))
            offset.valueChanged.connect(self._redraw_chart)
            points.toggled.connect(self._redraw_chart)
        finally:
            self.variables.blockSignals(False)

    @staticmethod
    def _set_color_button(button: QPushButton, color: str) -> None:
        button.setProperty("lineColor", color)
        button.setText(color)
        button.setStyleSheet(f"QPushButton {{ border: 3px solid {color}; }}")

    def _choose_color(self, button: QPushButton) -> None:
        color = QColorDialog.getColor(pg.mkColor(button.property("lineColor")), self,
                                     "Цвет линии")
        if color.isValid():
            self._set_color_button(button, color.name())
            self._redraw_chart()

    def _series_styles(self) -> dict[str, dict[str, Any]]:
        styles = {}
        for item in self._expression_items():
            widget = lambda index: self.variables.itemWidget(item.child(index), 2)
            styles[item.text(0)] = {
                "name": widget(5).text(),
                "color": widget(6).property("lineColor"),
                "offset": widget(7).value(),
                "points": widget(8).isChecked(),
                "chart_type": widget(9).currentData(),
            }
        return styles

    def _redraw_chart(self, _value: object = None) -> None:
        if self._last_chart_data is not None:
            self._draw_chart(self._last_chart_data)

    def _history_expressions(self) -> list[str]:
        return [item.text(0) for item in self._expression_items()
                if item.checkState(1) == Qt.Checked]

    def _history_changed(self, item: QTreeWidgetItem | None, column: int = 1) -> None:
        if item is not None and (item.parent() is not None or column != 1):
            return
        self._last_chart_data = trim_chart_data(
            self._last_chart_data,
            self.history_limit_spin.value(),
            set(self._history_expressions()),
        )
        if self._last_chart_data is not None:
            self._draw_chart(self._last_chart_data)
        else:
            self.plot.clear()
        self._refresh_history_table()

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

    @Slot()
    def _execute_controller_command(self) -> None:
        if not self._connected:
            return
        command_id = int(self.command_combo.currentData())
        self.command_button.setEnabled(False)
        self.command_result.setText("Выполнение…")
        self.controller_command_requested.emit(command_id)

    @Slot(int, dict)
    def _on_command_executed(
        self, command_id: int, result: dict[str, Any]
    ) -> None:
        self.command_button.setEnabled(self._connected)
        if result.get("queued"):
            self.command_result.setText("Сохранение запланировано")
        else:
            self.command_result.setText(
                f"Команда {command_id} выполнена (код {result.get('result', 0)})"
            )

    @Slot(dict)
    def _on_chart_data(self, data: dict[str, Any]) -> None:
        if self._pulse_counters.definitions:
            data = {**data, "series": [*data.get("series", []),
                                     *self._pulse_counters.process(data)]}
        self._statistics = merge_statistics(self._statistics, data)
        self._last_chart_data = merge_chart_data(
            self._last_chart_data,
            data,
            self.history_limit_spin.value(),
            set(self._history_expressions()),
        )
        self._refresh_values()

    def _refresh_values(self) -> None:
        if self._last_chart_data is None:
            return
        by_expression = {
            item.get("expression"): item
            for item in self._last_chart_data.get("series", [])
        }
        for item in self._expression_items():
            samples = by_expression.get(item.text(0), {}).get("samples", [])
            last = samples[-1] if samples else None
            previous = samples[-2] if len(samples) > 1 else None
            item.setText(2, "—" if last is None else str(last.get("value")))
            item.child(0).setText(2, "—" if previous is None else str(previous.get("value")))
            status = "—" if last is None else (
                "OK" if last.get("ok") else str(last.get("value", "ошибка")))
            item.setText(3, status)
        self._refresh_table_statistics()
        self._draw_chart(self._last_chart_data)
        self._refresh_history_table()

    @Slot(dict)
    def _on_messages(self, data: dict[str, Any]) -> None:
        priority_names = {
            0: "EMERG",
            1: "ALERT",
            2: "CRIT",
            3: "ERROR",
            4: "WARNING",
            5: "NOTICE",
            6: "INFO",
            7: "DEBUG",
        }
        dropped = int(data.get("dropped", 0) or 0)
        if dropped:
            self.status_label.setText(
                f"Пропущено сообщений отладчика: {dropped}"
            )
        for message in data.get("messages", []):
            if not isinstance(message, dict):
                continue
            row = self.messages_table.rowCount()
            self.messages_table.insertRow(row)
            timestamp = controller_timestamp_ms(
                data, int(message.get("time_ms", 0))
            )
            time_text = (
                datetime.fromtimestamp(timestamp / 1000).strftime(
                    "%Y-%m-%d %H:%M:%S.%f"
                )[:-3]
                if timestamp is not None
                else str(message.get("time_ms", ""))
            )
            priority = int(message.get("priority", 6))
            values = (
                time_text,
                str(message.get("source", "")),
                priority_names.get(priority, str(priority)),
                str(message.get("text", "")),
            )
            for column, value in enumerate(values):
                self.messages_table.setItem(
                    row, column, QTableWidgetItem(value)
                )
        while self.messages_table.rowCount() > 5_000:
            self.messages_table.removeRow(0)
        if data.get("messages"):
            self.messages_table.scrollToBottom()

    def _refresh_table_statistics(self) -> None:
        for item in self._expression_items():
            statistics = self._statistics.get(item.text(0), {})
            for index, key in enumerate(("min", "max", "average", "median"), start=1):
                item.child(index).setText(2, self._format_stat(statistics.get(key)))

    @staticmethod
    def _format_stat(value: float | None) -> str:
        return "—" if value is None else f"{value:.12g}"

    @Slot(int)
    def _history_limit_changed(self, limit: int) -> None:
        self._last_chart_data = trim_chart_data(
            self._last_chart_data, limit, set(self._history_expressions())
        )
        if self._last_chart_data is not None:
            self._refresh_table_statistics()
            self._draw_chart(self._last_chart_data)
            self._refresh_history_table()

    def _display_settings_changed(self, _value: int | bool) -> None:
        if self._last_chart_data is not None:
            self._refresh_table_statistics()
            self._draw_chart(self._last_chart_data)

    @Slot()
    def _clear_charts(self) -> None:
        self._last_chart_data = None
        self._statistics = {}
        self._pulse_counters.reset()
        self.plot.clear()
        self._refresh_history_table()
        self._refresh_table_statistics()
        if self._connected:
            self.clear_requested.emit()

    def _draw_chart(self, data: dict[str, Any]) -> None:
        self.plot.clear()
        server_time = int(data.get("server_time_ms", 0))
        prepared: list[tuple[dict[str, Any], list[dict[str, Any]]]] = []
        history_expressions = set(self._history_expressions())
        for series in data.get("series", []):
            if series.get("expression") not in history_expressions:
                continue
            numeric = [
                sample for sample in series.get("samples", [])
                if sample.get("ok") and sample.get("type") in {"number", "boolean"}
            ]
            if numeric:
                prepared.append((series, numeric))
        if not prepared:
            return

        base = int(prepared[0][1][0]["time_ms"])
        absolute_time = controller_timestamp_ms(data, base) is not None
        self._set_time_axis(absolute_time)
        max_x: float | None = None
        styles = self._series_styles()
        for series, numeric in prepared:
            expression = str(series.get("expression", ""))
            style = styles[expression]
            if absolute_time:
                timestamps = [
                    controller_timestamp_ms(data, int(sample["time_ms"]))
                    for sample in numeric
                ]
                x_values = [
                    int(timestamp) / 1000
                    for timestamp in timestamps
                    if timestamp is not None
                ]
            else:
                x_values = [
                    ((int(sample["time_ms"]) - base) & 0xFFFFFFFF) / 1000
                    for sample in numeric
                ]
            y_values = [float(sample["value"]) + style["offset"] for sample in numeric]
            point_x, point_y = x_values.copy(), y_values.copy()
            current_time = controller_timestamp_ms(data, server_time)
            current_x = (
                current_time / 1000
                if current_time is not None
                else ((server_time - base) & 0xFFFFFFFF) / 1000
            )
            if current_x > x_values[-1]:
                x_values.append(current_x)
                y_values.append(y_values[-1])
            max_x = x_values[-1] if max_x is None else max(max_x, x_values[-1])
            chart_type = style["chart_type"]
            step_mode = {"step_post": "right", "step_pre": "left"}.get(chart_type)
            if chart_type == "step_mid":
                # Center mode needs N+1 bin edges for N values. Keep actual
                # sample positions for markers, including the single-sample case.
                x_values = [x_values[0]] + [
                    left + (right - left) / 2
                    for left, right in zip(point_x, point_x[1:])
                ] + [x_values[-1]]
                y_values = point_y
                step_mode = "center"
            elif chart_type == "scatter":
                x_values, y_values = point_x, point_y
            self.plot.plot(
                x_values,
                y_values,
                name=style["name"].strip() or expression,
                pen=None if chart_type == "scatter" else pg.mkPen(style["color"], width=2),
                stepMode=step_mode,
                symbol="o" if chart_type == "scatter" else None,
                symbolSize=6,
                symbolBrush=style["color"],
                symbolPen=style["color"],
            )
            if style["points"] and chart_type != "scatter":
                self.plot.plot(point_x, point_y, pen=None, symbol="o", symbolSize=6,
                               symbolBrush=style["color"], symbolPen=style["color"])
        if self.auto_follow_check.isChecked() and max_x is not None:
            left = max_x - self.display_seconds_spin.value()
            if not absolute_time:
                left = max(0.0, left)
            self.plot.setXRange(left, max_x, padding=0)

    def _set_time_axis(self, absolute_time: bool) -> None:
        if absolute_time == self._absolute_time_axis:
            return
        axis = (
            pg.DateAxisItem(orientation="bottom")
            if absolute_time
            else pg.AxisItem(orientation="bottom")
        )
        self.plot.setAxisItems({"bottom": axis})
        self.plot.setLabel(
            "bottom",
            "Время контроллера" if absolute_time else "Время",
            units=None if absolute_time else "s",
        )
        self._absolute_time_axis = absolute_time

    def _refresh_history_table(self) -> None:
        self.history_model.set_chart_data(
            self._last_chart_data, self._history_expressions()
        )

    @Slot()
    def _export_history(self) -> None:
        if not self.history_model.rows:
            QMessageBox.information(
                self, "Lua debugger", "Нет накопленной истории для экспорта"
            )
            return
        path, _ = QFileDialog.getSaveFileName(
            self, "Экспорт истории", "debugger_history.xlsx", "Excel (*.xlsx)"
        )
        if not path:
            return
        if not path.lower().endswith(".xlsx"):
            path += ".xlsx"
        try:
            export_history_xlsx(
                path,
                self.history_model.rows,
                self.history_model.expressions,
                self.history_model.absolute_time,
            )
            self.status_label.setText(f"История экспортирована: {path}")
        except (OSError, ValueError, XlsxWriterException) as exc:
            self._show_error(str(exc))

    @Slot(str)
    def _show_error(self, message: str) -> None:
        self.command_button.setEnabled(self._connected)
        if self.command_result.text() == "Выполнение…":
            self.command_result.setText(f"Ошибка: {message}")
        self.status_label.setText(message)
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
                history_expressions=self._history_expressions(),
                chart_data=self._last_chart_data,
                display_seconds=self.display_seconds_spin.value(),
                auto_follow=self.auto_follow_check.isChecked(),
                statistics=self._statistics,
                series_styles=self._series_styles(),
                pulse_definitions=[vars(definition) for definition in
                                   self._pulse_counters.definitions.values()],
                pulse_state=self._pulse_counters.snapshot(),
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
            self.load_document(load_session(path))
        except (OSError, ValueError, TypeError) as exc:
            self._show_error(str(exc))

    def load_document(self, document: dict[str, Any]) -> None:
        connection = document["connection"]
        self.host_edit.setText(str(connection.get("host", "127.0.0.1")))
        self.port_spin.setValue(int(connection.get("port", 10_000)))
        self.interval_spin.setValue(int(document["poll_interval_ms"]))
        self.history_limit_spin.setValue(int(document["history_limit"]))
        self.display_seconds_spin.setValue(int(document["display_seconds"]))
        self.auto_follow_check.setChecked(bool(document["auto_follow"]))
        self._statistics = {
            expression: dict(extrema)
            for expression, extrema in document["statistics"].items()
        }
        history_expressions = set(document["history_expressions"])
        self._pulse_counters = PulseCounters()
        for values in document.get("pulse_definitions", []):
            self._pulse_counters.add(PulseDefinition(**values))
        self.variables.clear()
        for expression in [*document["expressions"], *self._pulse_counters.definitions]:
            self._create_expression(
                expression, history_enabled=expression in history_expressions,
                style=document.get("series_styles", {}).get(expression),
            )
        chart_data = document.get("chart_data")
        self._last_chart_data = None
        if isinstance(chart_data, dict):
            self._last_chart_data = chart_data
            self._pulse_counters.restore(
                document.get("pulse_state", {}),
                (chart_data.get("controller_time_unix_ms"),
                 chart_data.get("controller_time_millisec")),
            )
            self._refresh_values()
        else:
            self.plot.clear()
            self._refresh_history_table()
            self._refresh_table_statistics()
        self._apply_expressions()
        self._update_title()

    def _update_title(self, _value: object = None) -> None:
        marker = "● " if self._connected else ""
        host = self.host_edit.text().strip() or "новая сессия"
        self.title_changed.emit(f"{marker}{host}:{self.port_spin.value()}")

    def shutdown(self) -> None:
        if self._shutting_down:
            return
        self._shutting_down = True
        QMetaObject.invokeMethod(
            self._worker, "shutdown", Qt.BlockingQueuedConnection
        )
        self._thread.quit()
        self._thread.wait(3_000)

    def closeEvent(self, event: QCloseEvent) -> None:
        self.shutdown()
        event.accept()


class MainWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("ptusa Lua debugger")
        self.resize(1180, 760)

        self.sessions = QTabWidget()
        self.sessions.setDocumentMode(True)
        self.sessions.setMovable(True)
        self.sessions.setTabsClosable(True)
        self.sessions.tabCloseRequested.connect(self._close_session)
        self.setCentralWidget(self.sessions)

        add_button = QToolButton()
        add_button.setText("+")
        add_button.setToolTip("Новая сессия (Ctrl+T)")
        add_button.clicked.connect(self._add_session)
        self.sessions.setCornerWidget(add_button, Qt.TopRightCorner)

        session_menu = self.menuBar().addMenu("Сессия")
        new_action = QAction("Новая вкладка", self)
        new_action.setShortcut(QKeySequence.StandardKey.AddTab)
        new_action.triggered.connect(self._add_session)
        open_action = QAction("Открыть в новой вкладке…", self)
        open_action.setShortcut(QKeySequence.StandardKey.Open)
        open_action.triggered.connect(self._open_session)
        save_action = QAction("Сохранить текущую…", self)
        save_action.setShortcut(QKeySequence.StandardKey.Save)
        save_action.triggered.connect(self._save_current_session)
        close_action = QAction("Закрыть вкладку", self)
        close_action.setShortcut(QKeySequence.StandardKey.Close)
        close_action.triggered.connect(self._close_current_session)
        session_menu.addActions(
            [new_action, open_action, save_action, close_action]
        )

        self._add_session()

    @Slot()
    def _add_session(self) -> DebuggerSessionWidget:
        session = DebuggerSessionWidget()
        index = self.sessions.addTab(session, self._session_title(session))
        session.title_changed.connect(
            lambda title, current=session: self._set_session_title(current, title)
        )
        self.sessions.setCurrentIndex(index)
        return session

    @staticmethod
    def _session_title(session: DebuggerSessionWidget) -> str:
        host = session.host_edit.text().strip() or "новая сессия"
        return f"{host}:{session.port_spin.value()}"

    def _set_session_title(
        self, session: DebuggerSessionWidget, title: str
    ) -> None:
        index = self.sessions.indexOf(session)
        if index >= 0:
            self.sessions.setTabText(index, title)

    @Slot()
    def _open_session(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self, "Открыть сессию", "", "Lua debugger session (*.ptlua.json)"
        )
        if not path:
            return
        try:
            document = load_session(path)
        except (OSError, ValueError, TypeError) as exc:
            QMessageBox.warning(self, "Lua debugger", str(exc))
            return
        self._add_session().load_document(document)

    @Slot()
    def _save_current_session(self) -> None:
        session = self.sessions.currentWidget()
        if isinstance(session, DebuggerSessionWidget):
            session._save_session()

    @Slot()
    def _close_current_session(self) -> None:
        index = self.sessions.currentIndex()
        if index >= 0:
            self._close_session(index)

    @Slot(int)
    def _close_session(self, index: int) -> None:
        session = self.sessions.widget(index)
        if not isinstance(session, DebuggerSessionWidget):
            return
        self.sessions.removeTab(index)
        session.shutdown()
        session.deleteLater()
        if self.sessions.count() == 0:
            self._add_session()

    def closeEvent(self, event: QCloseEvent) -> None:
        for index in range(self.sessions.count()):
            session = self.sessions.widget(index)
            if isinstance(session, DebuggerSessionWidget):
                session.shutdown()
        event.accept()
