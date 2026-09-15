from zipfile import ZipFile

from ptusa_lua_debugger.excel_export import export_history_xlsx
from ptusa_lua_debugger.history import HistoryRow


def test_exports_sparse_history_to_xlsx(tmp_path) -> None:
    path = tmp_path / "history.xlsx"
    rows = [
        HistoryRow(
            1_789_123_456_000,
            {"x": {"ok": True, "type": "number", "value": 1.5}},
        ),
        HistoryRow(
            1_789_123_457_000,
            {"y": {"ok": True, "type": "boolean", "value": True}},
        ),
    ]

    export_history_xlsx(path, rows, ["x", "y"], absolute_time=True)

    assert path.stat().st_size > 0
    with ZipFile(path) as workbook:
        assert "xl/workbook.xml" in workbook.namelist()
        sheet = workbook.read("xl/worksheets/sheet1.xml")
        assert b'<c r="B2"' in sheet
        assert b'<c r="C3"' in sheet
        assert b'<c r="C2"' not in sheet
