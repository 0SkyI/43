-- Задание 1. Запросы на чтение к базе data/station.db.
-- Проверка: python3 src/partner/query.py data/station.db --file src/explore.sql

-- Пункт 1. Завершённые опыты: кто руководил и когда опыт закончился.
-- Одна строка результата = один завершённый опыт (status='completed')
-- с фамилией руководителя (experiments.leader_id -> people.person_id)
-- и моментом завершения finished_at (UTC). Отменённые и идущие опыты сюда не входят.
SELECT e.experiment_id,
       e.title,
       p.person_id  AS leader_id,
       p.name       AS leader_name,
       p.specialty  AS leader_specialty,
       e.finished_at
FROM experiments e
JOIN people p ON p.person_id = e.leader_id
WHERE e.status = 'completed'
ORDER BY e.finished_at;

-- Пункт 1а (уточнение к п.1). Тот же вопрос, но только про опыты, завершённые в июле 2026.
-- Одна строка = один завершённый опыт, у которого finished_at попадает в июль
-- (с 2026-07-01 00:00:00 включительно до 2026-08-01 00:00:00 не включительно).
-- Опыт 106 «Тепловой режим» завершён 2026-08-01 00:00:00 — это уже НЕ июль.
SELECT e.experiment_id,
       e.title,
       p.name       AS leader_name,
       e.finished_at
FROM experiments e
JOIN people p ON p.person_id = e.leader_id
WHERE e.status = 'completed'
  AND e.finished_at >= '2026-07-01 00:00:00'
  AND e.finished_at <  '2026-08-01 00:00:00'
ORDER BY e.finished_at;

-- Пункт 2а. Участники опыта 101 (таблица participation).
-- Одна строка = участие одного человека в опыте 101 с его ролью.
-- Это сведения о человеке в опыте, а не об самом опыте. Руководитель (person_id=1)
-- здесь учтён отдельно: руководство и непосредственное участие связаны через разные поля.
SELECT pa.person_id,
       p.name,
       p.specialty,
       pa.role
FROM participation pa
JOIN people p ON p.person_id = pa.person_id
WHERE pa.experiment_id = 101
ORDER BY pa.person_id;

-- Пункт 2б. Измерения опыта 101 (таблица measurements).
-- Одна строка = одна запись измерения, относящаяся к опыту 101.
-- value IS NULL при quality='missing' означает «число не получено»; это не ноль.
SELECT m.measurement_id,
       m.metric,
       m.value,
       m.unit,
       m.measured_at,
       m.quality
FROM measurements m
WHERE m.experiment_id = 101
ORDER BY m.measured_at;

-- Пункт 3. Приборы, по которым нет ни одной записи об осмотре.
-- Одна строка = один прибор (equipment), у которого отсутствует любая строка
-- в inspections (LEFT JOIN + проверка на NULL по ключу осмотра).
-- Отсутствие осмотра = состояние прибора неизвестно; это НЕ равно исправности (ok).
SELECT eq.equipment_id,
       eq.name,
       eq.serial
FROM equipment eq
LEFT JOIN inspections i ON i.equipment_id = eq.equipment_id
WHERE i.inspection_id IS NULL
ORDER BY eq.equipment_id;
