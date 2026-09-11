from ptusa_lua_debugger.history import merge_chart_data, trim_chart_data


def chart(*values: int) -> dict:
    return {
        "ok": True,
        "server_time_ms": values[-1] if values else 0,
        "series": [
            {
                "expression": "x",
                "samples": [
                    {"time_ms": value, "ok": True, "type": "number", "value": value}
                    for value in values
                ],
            }
        ],
    }


def test_merge_rolling_server_cache_without_duplicates() -> None:
    result = merge_chart_data(chart(1, 2, 3), chart(2, 3, 4), 5_000)

    assert [sample["value"] for sample in result["series"][0]["samples"]] == [
        1,
        2,
        3,
        4,
    ]


def test_history_is_limited_per_expression() -> None:
    result = merge_chart_data(chart(1, 2, 3), chart(3, 4, 5), 3)
    assert [sample["value"] for sample in result["series"][0]["samples"]] == [3, 4, 5]

    trimmed = trim_chart_data(result, 2)
    assert [sample["value"] for sample in trimmed["series"][0]["samples"]] == [4, 5]
