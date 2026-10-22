"""Собственные проверки числового правила evaluate (Задание 2).

Все случаи синтетические (origin='synthetic_fixture') — это НЕ ответы модели.
Для каждого заранее записаны expected, claim, ожидаемая mark и причина.
Запуск: python3 -m pytest day11/test_evals.py -q
"""

import json
import math
from pathlib import Path

import pytest

from day11.evaluate import evaluate

# Синтетический набор: ожидаемая отметка и причина записаны ДО запуска кода.
SYNTHETIC_CASES = [
    # правильный: конечное число совпадает с независимым SQL-ожиданием
    {
        "id": "my_correct",
        "prompt": "Сколько всего строк участия в таблице participation?",
        "expected": 19,
        "claim": 19,
        "expected_mark": "прошёл",
        "expected_reason": "число из ответа совпадает с независимым SQL-счётом",
        "origin": "synthetic_fixture",
    },
    # ошибочный: другое конечное число
    {
        "id": "my_wrong",
        "prompt": "Сколько всего строк участия в таблице participation?",
        "expected": 19,
        "claim": 20,
        "expected_mark": "не прошёл",
        "expected_reason": "завышенный счёт: расхождение 1 сверх погрешности",
        "origin": "synthetic_fixture",
    },
    # спорный: утверждение отсутствует (null)
    {
        "id": "my_unparsed",
        "prompt": "Сколько всего строк участия в таблице participation?",
        "expected": 19,
        "claim": None,
        "expected_mark": "спорно",
        "expected_reason": "числовое утверждение не извлечено из ответа",
        "origin": "synthetic_fixture",
    },
    # спорный: bool не считается числовым утверждением (True != 1)
    {
        "id": "my_bool_claim",
        "prompt": "Правда ли, что участников 19?",
        "expected": 19,
        "claim": True,
        "expected_mark": "спорно",
        "expected_reason": "bool — не однозначное числовое утверждение",
        "origin": "synthetic_fixture",
    },
    # спорный: строка «девятнадцать» — число не извлечено
    {
        "id": "my_string_claim",
        "prompt": "Сколько строк участия?",
        "expected": 19,
        "claim": "девятнадцать",
        "expected_mark": "спорно",
        "expected_reason": "строка — не числовой claim по контракту",
        "origin": "synthetic_fixture",
    },
    # спорный: NaN/бесконечность не считаются числами
    {
        "id": "my_nan_claim",
        "prompt": "Сколько строк участия?",
        "expected": 19,
        "claim": math.nan,
        "expected_mark": "спорно",
        "expected_reason": "NaN — не конечное число",
        "origin": "synthetic_fixture",
    },
    # правильный с точностью: int против float после усреднения
    {
        "id": "my_tolerance",
        "prompt": "Среднее двух достоверных измерений температуры опыта 101?",
        "expected": 15.0,
        "claim": 15,
        "expected_mark": "прошёл",
        "expected_reason": "15 == 15.0 в пределах погрешности 1e-9",
        "origin": "synthetic_fixture",
    },
    # спорный: само основание expected не число — сравнение невозможно
    {
        "id": "my_bad_expected",
        "prompt": "Сколько осмотров у весов?",
        "expected": None,
        "claim": 2,
        "expected_mark": "спорно",
        "expected_reason": "expected не получено как конечное число из SQL",
        "origin": "synthetic_fixture",
    },
]


@pytest.mark.parametrize("case", SYNTHETIC_CASES, ids=lambda c: c["id"])
def test_synthetic_rule(case):
    result = evaluate(case)
    assert result["mark"] == case["expected_mark"], (
        f"{case['id']}: ожидалась отметка {case['expected_mark']!r}, "
        f"получена {result['mark']!r} (причина: {result['reason']})"
    )
    assert isinstance(result["reason"], str) and result["reason"], (
        f"{case['id']}: reason обязан быть непустой строкой"
    )


def test_contract_keys():
    """Возвращается ровно словарь {'mark', 'reason'} с допустимой отметкой."""
    result = evaluate({"expected": 3, "claim": 3})
    assert set(result) == {"mark", "reason"}
    assert result["mark"] in {"прошёл", "не прошёл", "спорно"}


def test_parity_with_issued_fixtures():
    """Выданные fixtures/eval_cases.json (только чтение) согласуются с правилом."""
    path = Path(__file__).resolve().parent.parent / "fixtures" / "eval_cases.json"
    cases = json.loads(path.read_text(encoding="utf-8"))
    for case in cases:
        result = evaluate(case)
        assert result["mark"] == case["expected_mark"], (
            f"fixture {case['id']}: ожидалась {case['expected_mark']!r}, "
            f"получена {result['mark']!r}"
        )
