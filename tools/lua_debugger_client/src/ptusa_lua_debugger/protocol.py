from __future__ import annotations

import json
import socket
import struct
from enum import IntEnum
from typing import Any


class Command(IntEnum):
    CREATE_SESSION = 1
    EVALUATE = 2
    SET_CHART_EXPRESSIONS = 3
    GET_CHART_DATA = 4
    CLEAR_CHART_DATA = 5
    CLOSE_SESSION = 6
    KEEP_ALIVE = 7


class ProtocolError(RuntimeError):
    pass


class DebuggerProtocol:
    SERVICE_ID = 2
    FRAME_SINGLE = 1
    ACK_ERROR = 7
    ACK_OK = 12
    ACCEPT_MESSAGE = b"PAC accept"

    def __init__(self, timeout: float = 3.0) -> None:
        self.timeout = timeout
        self.session_id: str | None = None
        self.session_timeout_ms = 10_000
        self._socket: socket.socket | None = None
        self._packet_id = 0

    @property
    def connected(self) -> bool:
        return self._socket is not None and self.session_id is not None

    def connect(self, host: str, port: int = 10_000) -> str:
        self.disconnect(send_close=False)
        connection = socket.create_connection((host, port), timeout=self.timeout)
        connection.settimeout(self.timeout)
        try:
            greeting = self._recv_exact(connection, len(self.ACCEPT_MESSAGE))
            if greeting != self.ACCEPT_MESSAGE:
                raise ProtocolError("Контроллер не прислал приветствие PAC accept")
            self._socket = connection
            response = self._request(Command.CREATE_SESSION)
            self._ensure_ok(response)
            self.session_id = str(response["session_id"])
            self.session_timeout_ms = int(response.get("timeout_ms", 10_000))
            return self.session_id
        except Exception:
            connection.close()
            self._socket = None
            raise

    def disconnect(self, *, send_close: bool = True) -> None:
        connection = self._socket
        if connection is None:
            self.session_id = None
            return
        if send_close and self.session_id:
            try:
                self._request(Command.CLOSE_SESSION)
            except (OSError, ProtocolError):
                pass
        try:
            connection.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass
        connection.close()
        self._socket = None
        self.session_id = None

    def evaluate(self, expression: str) -> dict[str, Any]:
        return self._request(Command.EVALUATE, expression)

    def set_expressions(self, expressions: list[str]) -> dict[str, Any]:
        response = self._request(
            Command.SET_CHART_EXPRESSIONS, "\n".join(expressions)
        )
        self._ensure_ok(response)
        return response

    def get_chart_data(self) -> dict[str, Any]:
        response = self._request(Command.GET_CHART_DATA)
        self._ensure_ok(response)
        return response

    def clear_chart_data(self) -> None:
        self._ensure_ok(self._request(Command.CLEAR_CHART_DATA))

    def keep_alive(self) -> None:
        self._ensure_ok(self._request(Command.KEEP_ALIVE))

    def _request(self, command: Command, text: str = "") -> dict[str, Any]:
        connection = self._socket
        if connection is None:
            raise ProtocolError("Нет подключения к контроллеру")

        if command is not Command.CREATE_SESSION:
            if not self.session_id:
                raise ProtocolError("Сессия отладчика не создана")
            text = f"{self.session_id}\n{text}"

        payload = bytes((int(command),)) + text.encode("utf-8")
        if len(payload) > 0xFFFF:
            raise ProtocolError("Запрос слишком большой")

        self._packet_id = self._packet_id % 255 + 1
        frame = struct.pack(
            ">cBBBH",
            b"s",
            self.SERVICE_ID,
            self.FRAME_SINGLE,
            self._packet_id,
            len(payload),
        ) + payload
        connection.sendall(frame)

        header = self._recv_exact(connection, 5)
        net_id, acknowledgement, packet_id, length = struct.unpack(">cBBH", header)
        if net_id != b"s" or packet_id != self._packet_id:
            raise ProtocolError("Получен ответ от другого пакета")
        response_payload = self._recv_exact(connection, length)
        if acknowledgement == self.ACK_ERROR:
            code = response_payload[0] if response_payload else -1
            raise ProtocolError(f"Ошибка протокола PAC: {code}")
        if acknowledgement != self.ACK_OK:
            raise ProtocolError(f"Неизвестный тип ответа PAC: {acknowledgement}")

        try:
            return json.loads(response_payload.rstrip(b"\0").decode("utf-8"))
        except (UnicodeDecodeError, json.JSONDecodeError) as error:
            raise ProtocolError("Некорректный JSON в ответе контроллера") from error

    @staticmethod
    def _recv_exact(connection: socket.socket, size: int) -> bytes:
        chunks = bytearray()
        while len(chunks) < size:
            chunk = connection.recv(size - len(chunks))
            if not chunk:
                raise ProtocolError("Контроллер закрыл соединение")
            chunks.extend(chunk)
        return bytes(chunks)

    @staticmethod
    def _ensure_ok(response: dict[str, Any]) -> None:
        if not response.get("ok"):
            raise ProtocolError(str(response.get("error", "Неизвестная ошибка")))
