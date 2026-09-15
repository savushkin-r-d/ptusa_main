from __future__ import annotations

from datetime import datetime
from typing import Any

from PySide6.QtCore import QAbstractTableModel, QModelIndex, Qt

from .history import HistoryRow, build_history_rows


class HistoryTableModel(QAbstractTableModel):
    def __init__(self) -> None:
        super().__init__()
        self.expressions: list[str] = []
        self.rows: list[HistoryRow] = []
        self.absolute_time = False

    def set_chart_data(
        self, data: dict[str, Any] | None, expressions: list[str]
    ) -> None:
        rows, absolute_time = build_history_rows(data, expressions)
        self.beginResetModel()
        self.expressions = list(expressions)
        self.rows = rows
        self.absolute_time = absolute_time
        self.endResetModel()

    def rowCount(self, _parent: QModelIndex = QModelIndex()) -> int:
        return len(self.rows)

    def columnCount(self, _parent: QModelIndex = QModelIndex()) -> int:
        return 1 + len(self.expressions)

    def data(self, index: QModelIndex, role: int = Qt.DisplayRole) -> Any:
        if not index.isValid() or role not in {Qt.DisplayRole, Qt.TextAlignmentRole}:
            return None
        if role == Qt.TextAlignmentRole:
            return int(Qt.AlignRight | Qt.AlignVCenter)

        row = self.rows[index.row()]
        if index.column() == 0:
            if self.absolute_time:
                return datetime.fromtimestamp(row.time_ms / 1000).strftime(
                    "%Y-%m-%d %H:%M:%S.%f"
                )[:-3]
            return f"{row.time_ms / 1000:.3f}"

        sample = row.samples.get(self.expressions[index.column() - 1])
        if sample is None:
            return None
        return str(sample.get("value", ""))

    def headerData(
        self, section: int, orientation: Qt.Orientation, role: int = Qt.DisplayRole
    ) -> Any:
        if role != Qt.DisplayRole:
            return None
        if orientation == Qt.Horizontal:
            return "Время контроллера" if section == 0 else self.expressions[section - 1]
        return section + 1
