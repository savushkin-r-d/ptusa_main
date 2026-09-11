import json
import struct

from ptusa_lua_debugger.protocol import Command, DebuggerProtocol


def response(packet_id: int, document: dict) -> bytes:
    payload = json.dumps(document).encode() + b"\0"
    return struct.pack(">cBBH", b"s", 12, packet_id, len(payload)) + payload


class FakeSocket:
    def __init__(self, incoming: bytes) -> None:
        self.incoming = bytearray(incoming)
        self.sent = bytearray()
        self.closed = False

    def settimeout(self, timeout: float) -> None:
        pass

    def recv(self, size: int) -> bytes:
        result = bytes(self.incoming[:size])
        del self.incoming[:size]
        return result

    def sendall(self, data: bytes) -> None:
        self.sent.extend(data)

    def shutdown(self, how: int) -> None:
        pass

    def close(self) -> None:
        self.closed = True


def test_connect_creates_session(monkeypatch) -> None:
    fake = FakeSocket(
        b"PAC accept"
        + response(
            1,
            {"ok": True, "session_id": "0123456789abcdef", "timeout_ms": 10_000},
        )
    )
    monkeypatch.setattr("socket.create_connection", lambda *args, **kwargs: fake)

    client = DebuggerProtocol()
    assert client.connect("127.0.0.1") == "0123456789abcdef"
    assert client.connected

    net_id, service, frame_type, packet_id, length = struct.unpack(
        ">cBBBH", fake.sent[:6]
    )
    assert (net_id, service, frame_type, packet_id) == (b"s", 2, 1, 1)
    assert fake.sent[6 : 6 + length] == bytes((Command.CREATE_SESSION,))


def test_request_contains_session_id() -> None:
    fake = FakeSocket(response(1, {"ok": True, "count": 2}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"

    client.set_expressions(["TE1:get_value()", "M1:get_state()"])

    _, service, frame_type, packet_id, length = struct.unpack(">cBBBH", fake.sent[:6])
    assert (service, frame_type, packet_id) == (2, 1, 1)
    assert fake.sent[6 : 6 + length] == (
        bytes((Command.SET_CHART_EXPRESSIONS,))
        + b"session1\nTE1:get_value()\nM1:get_state()"
    )
