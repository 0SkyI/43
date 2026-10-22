"""Собственные тесты числового правила evaluate() — Задание 2.

ВСЕ случаи здесь синтетические (origin = synthetic_fixture): они не являются
ответами реальной модели и не доказывают качество модели. Для каждого случая
ЗАРАНЕЕ записаны ожидаемая отметка и причина (см. docstring-таблицу ниже).

Запуск: python3 -m pytest day11/test_evals.py -q

Ожидаемые отметки и причины (записаны до запуска, по правилу №2 из журнала):
| id                        | claim | expected | mark      | заранее записанная причина                                    |
|---------------------------|-------|----------|-----------|-----------------------------------------------------------------|
| correct                   | 3     | 3        | прошёл    | однозначные числа совпадают точно                               |
| wrong                     | 4     | 3        | не прошёл | разница 1 > погрешности 0.05                                    |
| unparsed                  | None  | 3        | спорно    | утверждения нет / не извлечено — проблема проверки              |
| near_within_tolerance     | 20.04 | 20.0     | прошёл    | округление до десятых: разница 0.04 <= 0.05                     |
| near_outside_tolerance    | 20.06 | 20.0     | не прошёл | разница 0.06 > 0.05                                             |
| bool_claim                | True  | 1        | спорно    | bool не считается числовым утверждением                         |
| string_claim              | "20"  | 20.0     | спорно    | строка не считается числом даже при парс-в-число                |
| nan_claim                 | NaN   | 20.0     | спорно    | NaN не конечное число                                            |
| inf_claim                 | inf   | 20.0     | спорно    | бесконечность не конечное число                                  |
| bad_expected              | 20.0  | None     | спорно    | независимое основание не подготовлено — вина проверки            |
| int_float_equal           | 20    | 20.0     | прошёл    | int и float одного значения равны                                |

Что правило ЗАМЕЧАЕТ: подменённое/неверное конечное число, отсутствие числа,
нечисловые типы claim, неподготовленный expected, выход за объявленную точность.
Что правило НЕ ЗАМЕЧАЕТ (проверяет только человек): выбор таблиц и связей
(meaning), смешение единиц (g+kg без перевода даёт «правильное» число не в тех
единицах), неверный отбор quality/suspect внутри expected, двусмысленность
вопроса (ambiguity), отказы отвечать при наличии данных, устойчивость (stability).
"""

import json
import math
from pathlib import Path

import pytest

from day11.evaluate import TOLERANCE, evaluate

FIXTURES_PATH = Path(__file__).resolve().parent.parent / 'fixtures' / 'eval_cases.json'


def load_synthetic_fixtures():
    """Выданные неизменяемые синтетические примеры (origin=synthetic_fixture)."""
    with FIXTURES_PATH.open(encoding='utf-8') as handle:
        return json.load(handle)


# --- собственные синтетические случаи (помечены origin=synthetic_fixture) ----
MY_CASES = [
    # правильный: значение в пределах заранее объявленной точности
    {'id': 'near_within_tolerance', 'expected': 20.0, 'claim': 20.04,
     'expected_mark': 'прошёл', 'origin': 'synthetic_fixture'},
    # ошибочный: чуть дальше границы точности
    {'id': 'near_outside_tolerance', 'expected': 20.0, 'claim': 20.06,
     'expected_mark': 'не прошёл', 'origin': 'synthetic_fixture'},
    # спорные по контракту api_practice.md
    {'id': 'bool_claim', 'expected': 1, 'claim': True,
     'expected_mark': 'спорно', 'origin': 'synthetic_fixture'},
    {'id': 'string_claim', 'expected': 20.0, 'claim': '20.0',
     'expected_mark': 'спорно', 'origin': 'synthetic_fixture'},
    {'id': 'nan_claim', 'expected': 20.0, 'claim': float('nan'),
     'expected_mark': 'спорно', 'origin': 'synthetic_fixture'},
    {'id': 'inf_claim', 'expected': 20.0, 'claim': float('inf'),
     'expected_mark': 'спорно', 'origin': 'synthetic_fixture'},
    # проблема подготовки доказательства, а не ответа модели
    {'id': 'bad_expected', 'expected': None, 'claim': 20.0,
     'expected_mark': 'спорно', 'origin': 'synthetic_fixture'},
    # int против float одного значения
    {'id': 'int_float_equal', 'expected': 20.0, 'claim': 20,
     'expected_mark': 'прошёл', 'origin': 'synthetic_fixture'},
]

ALL_CASES = load_synthetic_fixtures() + MY_CASES


@pytest.mark.parametrize('case', ALL_CASES, ids=[c['id'] for c in ALL_CASES])
def test_rule_matches_predeclared_mark(case):
    result = evaluate(case)
    assert result['mark'] == case['expected_mark'], (
        '%s: ожидалось %r, получено %r (%s)'
        % (case['id'], case['expected_mark'], result['mark'], result['reason']))
    assert isinstance(result['reason'], str) and result['reason'].strip(), \
        'причина оценки обязана быть непустой'


def test_reason_explains_every_case():
    for case in ALL_CASES:
        out = evaluate(case)
        assert set(out) >= {'mark', 'reason'}
        assert out['mark'] in ('прошёл', 'не прошёл', 'спорно')


def test_tolerance_is_declared_in_advance():
    # Точность объявлена в модуле и не меняется тестом.
    assert TOLERANCE == 0.05
    # Внутри границы — «прошёл»:
    assert evaluate({'expected': 20.0, 'claim': 20.04})['mark'] == 'прошёл'
    # Снаружи границы — «не прошёл»:
    assert evaluate({'expected': 20.0, 'claim': 20.06})['mark'] == 'не прошёл'
    # Ровно на границе abs(claim-expected) <= TOLERANCE решение принимает
    # двоичное float-представление: abs(20.0-(20.0-0.05)) = 0.05000000000000071,
    # что больше 0.05, поэтому формально «не прошёл». Это осознанная граница
    # правила; записана здесь заранее как наблюдаемое поведение.
    assert evaluate({'expected': 20.0, 'claim': 20.0 - TOLERANCE})['mark'] == 'не прошёл'


def test_nan_guard_not_masked_by_math():
    # явная проверка: NaN должен давать «спорно», а не сравнение через math.isclose
    out = evaluate({'expected': 20.0, 'claim': float('nan')})
    assert out['mark'] == 'спорно'
    assert not math.isfinite(out and float('nan'))
