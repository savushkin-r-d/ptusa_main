from __future__ import annotations

from PySide6.QtCore import QObject, QTimer, Signal, Slot

from .protocol import DebuggerProtocol, ProtocolError


class DebuggerWorker(QObject):
    connected = Signal(str)
    disconnected = Signal(str)
    chart_data = Signal(dict)
    evaluated = Signal(str, dict)
    error = Signal(str)

    def __init__(self) -> None:
        super().__init__()
        self._client = DebuggerProtocol()
        self._timer = QTimer(self)
        self._timer.timeout.connect(self.poll)
        self._interval_ms = 500

    @Slot(str, int)
    def connect_to(self, host: str, port: int) -> None:
        try:
            session_id = self._client.connect(host, port)
            self._timer.start(self._interval_ms)
            self.connected.emit(session_id)
            self.poll()
        except (OSError, ProtocolError) as exc:
            self._client.disconnect(send_close=False)
            self.disconnected.emit(str(exc))

    @Slot()
    def disconnect(self) -> None:
        self._timer.stop()
        self._client.disconnect()
        self.disconnected.emit("")

    @Slot(int)
    def set_interval(self, interval_ms: int) -> None:
        self._interval_ms = max(100, min(interval_ms, 9_000))
        if self._timer.isActive():
            self._timer.start(self._interval_ms)

    @Slot(list)
    def set_expressions(self, expressions: list[str]) -> None:
        if not self._client.connected:
            return
        try:
            self._client.set_expressions(expressions)
            self.poll()
        except (OSError, ProtocolError) as exc:
            self.error.emit(str(exc))

    @Slot(str)
    def evaluate(self, expression: str) -> None:
        if not self._client.connected:
            self.error.emit("Нет подключения к контроллеру")
            return
        try:
            self.evaluated.emit(expression, self._client.evaluate(expression))
        except (OSError, ProtocolError) as exc:
            self.error.emit(str(exc))

    @Slot()
    def clear_chart_data(self) -> None:
        if not self._client.connected:
            return
        try:
            self._client.clear_chart_data()
            self.poll()
        except (OSError, ProtocolError) as exc:
            self.error.emit(str(exc))

    @Slot()
    def poll(self) -> None:
        if not self._client.connected:
            return
        try:
            self.chart_data.emit(self._client.get_chart_data())
        except (OSError, ProtocolError) as exc:
            self._timer.stop()
            self._client.disconnect(send_close=False)
            self.disconnected.emit(str(exc))

    @Slot()
    def shutdown(self) -> None:
        self._timer.stop()
        self._client.disconnect()
