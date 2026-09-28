from zipfile import ZipFile

from ptusa_lua_debugger.excel_export import export_history_xlsx
from ptusa_lua_debugger.history import HistoryRow


def test_exports_sparse_history_to_xlsx(tmp_path) -> None:
    path = tmp_path / "history.xlsx"
    rows = [
        HistoryRow(
            1_000,
            1_789_123_456_000,
            {"x": {"ok": True, "type": "number", "value": 1.5}},
        ),
        HistoryRow(
            2_000,
            None,
            {"y": {"ok": True, "type": "boolean", "value": True}},
        ),
    ]

    export_history_xlsx(path, rows, ["x", "y"])

    assert path.stat().st_size > 0
    with ZipFile(path) as workbook:
        assert "xl/workbook.xml" in workbook.namelist()
        sheet = workbook.read("xl/worksheets/sheet1.xml")
        strings = (
            workbook.read("xl/sharedStrings.xml")
            if "xl/sharedStrings.xml" in workbook.namelist()
            else b""
        )
        text = sheet + strings
        # Both time columns and the two expression headers exist.
        for label in ("Реальное время", "Время контроллера, с", "x", "y"):
            assert label.encode("utf-8") in text
        # Real time written only when available, controller time always.
        assert b'<c r="A2"' in sheet
        assert b'<c r="B2"' in sheet
        assert b'<c r="B3"' in sheet
        assert b'<c r="A3"' not in sheet
        # Expression cells moved one column right; sparse cells stay absent.
        assert b'<c r="C2"' in sheet
        assert b'<c r="D3"' in sheet
        assert b'<c r="D2"' not in sheet
        assert b'<c r="C3"' not in sheet
