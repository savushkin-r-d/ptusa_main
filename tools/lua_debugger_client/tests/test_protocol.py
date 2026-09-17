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

    def setsockopt(self, level: int, option: int, value: int) -> None:
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
            {
                "ok": True,
                "session_id": "0123456789abcdef",
                "timeout_ms": 10_000,
                "controller_time_unix_ms": 1_789_123_456_789,
                "controller_time_millisec": 123_456,
            },
        )
    )
    monkeypatch.setattr("socket.create_connection", lambda *args, **kwargs: fake)

    client = DebuggerProtocol()
    assert client.connect("127.0.0.1") == "0123456789abcdef"
    assert client.connected
    assert client.controller_time_unix_ms == 1_789_123_456_789
    assert client.controller_time_millisec == 123_456

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


def test_chart_data_contains_session_time_anchor() -> None:
    fake = FakeSocket(response(1, {"ok": True, "server_time_ms": 12, "series": []}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    client.controller_time_unix_ms = 1_789_123_456_789
    client.controller_time_millisec = 123_456

    data = client.get_chart_data()

    assert data["controller_time_unix_ms"] == 1_789_123_456_789
    assert data["controller_time_millisec"] == 123_456


def test_messages_contain_session_time_anchor() -> None:
    fake = FakeSocket(
        response(
            1,
            {
                "ok": True,
                "dropped": 0,
                "messages": [
                    {
                        "id": 1,
                        "time_ms": 123_500,
                        "source": "log",
                        "priority": 6,
                        "text": "ready",
                    }
                ],
            },
        )
    )
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    client.controller_time_unix_ms = 1_789_123_456_789
    client.controller_time_millisec = 123_456

    data = client.get_messages()

    assert data["controller_time_unix_ms"] == 1_789_123_456_789
    assert data["controller_time_millisec"] == 123_456
    assert data["messages"][0]["text"] == "ready"
    assert fake.sent[6] == Command.GET_MESSAGES


def test_poll_uses_one_request_for_chart_and_messages() -> None:
    fake = FakeSocket(response(1, {
        "ok": True, "series": [],
        "events": {"ok": True, "messages": [], "dropped": 0},
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    client.controller_time_unix_ms = 1000
    client.controller_time_millisec = 50
    data = client.poll()
    assert fake.sent[6] == Command.POLL
    assert len(fake.sent) == 6 + 1 + len("session1\n")
    assert data["events"]["controller_time_unix_ms"] == 1000
    assert data["controller_time_millisec"] == 50
