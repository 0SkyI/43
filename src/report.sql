-- Задание 6. Запросы отчёта к src/results.db (только чтение).
-- Единица подсчёта основной серии — одиночная попытка этапа main, оцененная
-- по ОСНОВНОЙ стороне качества её случая (dimension_id = cases.dimension_id).
-- Пары устойчивости и ответы на сокращённую формулировку в пункты 2-4 не входят.

-- [1] Число разных случаев, формулировок и ответов (вся основная серия main).
SELECT
    (SELECT COUNT(DISTINCT case_id) FROM cases) AS cases_total,
    (SELECT COUNT(DISTINCT prompt_id) FROM prompts) AS prompts_total,
    (SELECT COUNT(*) FROM attempts WHERE phase = 'main') AS answers_main;

-- [2] Отметки по каждой стороне качества (основные попытки main, без shortened).
SELECT
    c.dimension_id,
    d.title,
    SUM(CASE WHEN a.verdict = 'pass'    THEN 1 ELSE 0 END) AS passed,
    SUM(CASE WHEN a.verdict = 'fail'    THEN 1 ELSE 0 END) AS failed,
    SUM(CASE WHEN a.verdict = 'unclear' THEN 1 ELSE 0 END) AS unclear,
    SUM(CASE WHEN a.verdict = 'na'      THEN 1 ELSE 0 END) AS na,
    GROUP_CONCAT('a' || t.attempt_id || ':' || a.verdict) AS refs
FROM assessments a
JOIN attempts t USING (attempt_id)
JOIN prompts p USING (prompt_id)
JOIN cases c USING (case_id)
JOIN dimensions d USING (dimension_id)
WHERE t.phase = 'main' AND p.label <> 'shortened'
  AND a.dimension_id = c.dimension_id          -- только основная сторона случая
GROUP BY c.dimension_id, d.title
ORDER BY c.dimension_id;

-- [3] Дополнительные (неосновные) отметки у основных попыток — показать отдельно.
SELECT a.attempt_id, a.dimension_id AS extra_dim, a.verdict, c.dimension_id AS base_dim
FROM assessments a
JOIN attempts t USING (attempt_id)
JOIN prompts p USING (prompt_id)
JOIN cases c USING (case_id)
WHERE t.phase = 'main' AND p.label <> 'shortened'
  AND a.dimension_id <> c.dimension_id;

-- [4а] Обычные (normal) случаи: отметки основной стороны, без shortened.
SELECT
    c.kind,
    SUM(CASE WHEN a.verdict = 'pass'    THEN 1 ELSE 0 END) AS passed,
    SUM(CASE WHEN a.verdict = 'fail'    THEN 1 ELSE 0 END) AS failed,
    SUM(CASE WHEN a.verdict = 'unclear' THEN 1 ELSE 0 END) AS unclear,
    SUM(CASE WHEN a.verdict = 'na'      THEN 1 ELSE 0 END) AS na,
    GROUP_CONCAT('a' || t.attempt_id || ':' || a.verdict) AS refs
FROM assessments a
JOIN attempts t USING (attempt_id)
JOIN prompts p USING (prompt_id)
JOIN cases c USING (case_id)
WHERE t.phase = 'main' AND p.label <> 'shortened'
  AND a.dimension_id = c.dimension_id
GROUP BY c.kind
ORDER BY c.kind;

-- [4б] Устойчивость: пары, у которых обе попытки этапа main и без shortened.
SELECT
    pr.pair_id,
    a.verdict,
    pr.left_attempt_id,
    pr.right_attempt_id
FROM pairs pr
JOIN assessments a ON a.pair_id = pr.pair_id
JOIN attempts l ON l.attempt_id = pr.left_attempt_id
JOIN attempts r ON r.attempt_id = pr.right_attempt_id
JOIN prompts lp ON lp.prompt_id = l.prompt_id
JOIN prompts rp ON rp.prompt_id = r.prompt_id
WHERE l.phase = 'main' AND r.phase = 'main'
  AND lp.label <> 'shortened' AND rp.label <> 'shortened'
ORDER BY pr.pair_id;

-- [5] Два повторённых вопроса задания 5 (prompt_id=4 и prompt_id=8):
-- число ответов и число ошибок по основной стороне качества.
SELECT
    p.prompt_id,
    c.case_id,
    c.dimension_id,
    COUNT(*) AS answers,
    SUM(CASE WHEN a.verdict = 'fail' THEN 1 ELSE 0 END) AS fails,
    GROUP_CONCAT('a' || t.attempt_id || ':' || COALESCE(a.verdict,'нет оценки')) AS refs
FROM attempts t
JOIN prompts p USING (prompt_id)
JOIN cases c USING (case_id)
LEFT JOIN assessments a ON a.attempt_id = t.attempt_id
                       AND a.dimension_id = c.dimension_id
WHERE t.phase = 'main' AND p.prompt_id IN (4, 8)
GROUP BY p.prompt_id, c.case_id, c.dimension_id;

-- [5б] Сокращённая формулировка (prompt_id=13) считается отдельно.
SELECT
    p.prompt_id,
    c.case_id,
    c.dimension_id,
    COUNT(*) AS answers,
    SUM(CASE WHEN a.verdict = 'fail' THEN 1 ELSE 0 END) AS fails,
    GROUP_CONCAT('a' || t.attempt_id || ':' || COALESCE(a.verdict,'нет оценки')) AS refs
FROM attempts t
JOIN prompts p USING (prompt_id)
JOIN cases c USING (case_id)
LEFT JOIN assessments a ON a.attempt_id = t.attempt_id
                       AND a.dimension_id = c.dimension_id
WHERE t.phase = 'main' AND p.label = 'shortened'
GROUP BY p.prompt_id, c.case_id, c.dimension_id;

-- [6] Обмен (peer): попытки и их отметки — выводятся отдельно.
SELECT t.attempt_id, p.prompt_id, c.case_id, a.verdict, a.dimension_id, a.assessed_by
FROM attempts t
JOIN prompts p USING (prompt_id)
JOIN cases c USING (case_id)
LEFT JOIN assessments a ON a.attempt_id = t.attempt_id
WHERE t.phase = 'peer'
ORDER BY t.attempt_id;

-- [7] Защита (defense): попытки и их отметки — выводятся отдельно.
SELECT t.attempt_id, p.prompt_id, c.case_id, a.verdict, a.dimension_id, a.assessed_by
FROM attempts t
JOIN prompts p USING (prompt_id)
JOIN cases c USING (case_id)
LEFT JOIN assessments a ON a.attempt_id = t.attempt_id
WHERE t.phase = 'defense'
ORDER BY t.attempt_id;
