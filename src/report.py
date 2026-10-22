"""Задание 6. Отчёт по журналу src/results.db (только чтение, стандартная библиотека).

Запуск из корня проекта:  python3 src/report.py [путь_к_журналу]

Программа выполняет запросы из src/report.sql и печатает отчёт.
Ничего не оценивает и не меняет — только подсчитывает уже поставленные отметки.
Единица подсчёта основной серии — одиночная попытка этапа main; одна попытка
не превращается в несколько строк (оценка берётся по основной стороне качества
её случая; пары устойчивости считаются как одна оценка на пару).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent / 'partner'))
from common import JOURNAL, readonly, split_sql  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
SQL_FILE = ROOT / 'src' / 'report.sql'

VERDICT_RU = {'pass': 'прошёл', 'fail': 'не прошёл',
              'unclear': 'спорно', 'na': 'не применимо'}


def run_queries(path):
    """Выполнить все запросы из report.sql; вернуть список списков строк."""
    con = readonly(path)
    try:
        results = []
        for statement in split_sql(SQL_FILE.read_text(encoding='utf-8')):
            cur = con.execute(statement)
            results.append(cur.fetchall())
        return results
    finally:
        con.close()


def pct(fails, passes):
    """Доля ошибок: fail / (pass + fail); без данных — «нет данных»."""
    base = fails + passes
    if base == 0:
        return 'нет данных (0 наблюдений)'
    return '%d%% (по %d наблюдению: прошёл %d, не прошёл %d)' % (
        round(100.0 * fails / base), base, passes, fails)


def line(text=''):
    print(text)


def report(rows):
    (cases_total, prompts_total, answers_main), = rows[0]
    line('ОТЧЁТ ПО ЖУРНАЛУ (основной прогон — этап main)')
    line('=' * 60)

    # 1. Общее число объектов.
    line()
    line('1. Разных случаев: %d; формулировок: %d; ответов этапа main: %d.'
         % (cases_total, prompts_total, answers_main))

    # 2. Отметки по каждой стороне качества.
    line()
    line('2. Отметки по сторонам качества (одиночные попытки main,'
         '\n   без сокращённой формулировки, по основной стороне случая):')
    dim_totals = {}
    for dimension_id, title, passed, failed, unclear, na, refs in rows[1]:
        dim_totals[dimension_id] = (passed, failed)
        line('   %-12s %-28s прошёл %d | не прошёл %d | спорно %d | не применимо %d'
             % (dimension_id, title, passed, failed, unclear, na))
        line('       ссылки: %s' % refs)
    if not rows[1]:
        line('   нет данных (пустой журнал)')
    extra = rows[2]
    line('   Дополнительные (неосновные) отметки у тех же попыток — отдельно:')
    if extra:
        for attempt_id, dim, verdict, base_dim in extra:
            line('     попытка a%s: %s=%s (основная сторона случая — %s)'
                 % (attempt_id, dim, VERDICT_RU.get(verdict, verdict), base_dim))
    else:
        line('     нет')

    # 3. Доля ошибок по сторонам.
    line()
    line('3. Доля ошибок = «не прошёл» / («прошёл» + «не прошёл»),'
         '\n   «спорно» и «не применимо» в долю не входят:')
    if dim_totals:
        for dimension_id, (passed, failed) in sorted(dim_totals.items()):
            line('   %-12s %s' % (dimension_id, pct(failed, passed)))
    else:
        line('   нет данных')

    # 4. Обычные и трудные случаи; устойчивость отдельным знаменателем.
    line()
    line('4. Результаты по типу случая (без shortened; устойчивость — отдельно,'
         '\n   её знаменатель с одиночными ответами не складывается):')
    kind_names = {'normal': 'обычные', 'challenge': 'трудные'}
    for kind, passed, failed, unclear, na, refs in rows[3]:
        if kind == 'pair':
            continue
        line('   %-8s прошёл %d | не прошёл %d | спорно %d | не применимо %d;'
             ' доля ошибок: %s' %
             (kind_names.get(kind, kind), passed, failed, unclear, na,
              pct(failed, passed)))
        line('       ссылки: %s' % refs)
    pairs = rows[4]
    pair_pass = sum(1 for _, v, _, _ in pairs if v == 'pass')
    pair_fail = sum(1 for _, v, _, _ in pairs if v == 'fail')
    line('   устойчивость: оценённых пар (обе попытки main, без shortened): %d;'
         ' прошло %d, не прошло %d' % (len(pairs), pair_pass, pair_fail))
    for pair_id, verdict, left_id, right_id in pairs:
        line('     пара p%s: %s (попытки a%s + a%s)'
             % (pair_id, VERDICT_RU.get(verdict, verdict), left_id, right_id))

    # 5. Повторённые вопросы задания 5 и сокращённая формулировка.
    line()
    line('5. Повторённые вопросы задания 5 (номера формулировок взяты из заметки'
         '\n   «Повтор и сокращение»): число ответов и число ошибок по основной'
         '\n   стороне качества; сокращённая формулировка — отдельно:')
    for prompt_id, case_id, dimension_id, answers, fails, refs in rows[5]:
        line('   формулировка P%d (случай C%d, сторона %s): ответов %d, ошибок %d (%s)'
             % (prompt_id, case_id, dimension_id, answers, fails, refs))
    for prompt_id, case_id, dimension_id, answers, fails, refs in rows[6]:
        line('   СОКРАЩЁННАЯ формулировка P%d (случай C%d, сторона %s):'
             ' ответов %d, ошибок %d (%s) — в пункты 2-4 не входит'
             % (prompt_id, case_id, dimension_id, answers, fails, refs))

    # 6-7. Peer и defense — отдельно от основного отчёта.
    for title, block in (('6. Обмен (peer) — отдельно:', rows[7]),
                         ('7. Защита (defense) — отдельно:', rows[8])):
        line()
        line(title)
        if block:
            for attempt_id, prompt_id, case_id, verdict, dim, by in block:
                line('   попытка a%s (P%d, случай C%d): %s [%s] кем: %s'
                     % (attempt_id, prompt_id, case_id,
                        VERDICT_RU.get(verdict, verdict) if verdict else 'без оценки',
                        dim or '-', by or '-'))
        else:
            line('   нет')
    line()
    line('Все числа можно сверить по указанным номерам случаев (C), формулировок'
         '\n(P), попыток (a) и пар (p).')


def main():
    path = Path(sys.argv[1]) if len(sys.argv) > 1 else JOURNAL
    report(run_queries(path))


if __name__ == '__main__':
    main()
