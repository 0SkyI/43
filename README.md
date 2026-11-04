# Stack with CMake & Google Test

Проект реализует шаблонный стек фиксированной вместимости `TStack<T>`
с подключением [Google Test](https://github.com/google/googletest) через CMake.

## Структура проекта

```
├── CMakeLists.txt          # сборка: приложение + тесты (GoogleTest через FetchContent)
├── include/
│   └── stack.hpp           # только определение класса TStack<T>
├── src/
│   ├── main.cpp            # вся реализация методов TStack<T> + демо-функция run_demo()
│   └── app_main.cpp        # точка входа приложения stack_app
└── tests/
    └── stack_test.cpp      # 22 юнит-теста Google Test (каждый с комментарием)
```

Так как `TStack<T>` — шаблон, тела методов должны быть видны в точке
инстанцирования: `include/stack.hpp` содержит только объявление класса,
а реализация из `src/main.cpp` подключается в конце заголовка через
`#include "../src/main.cpp"`.

## Требования

- CMake ≥ 3.14
- Компилятор с поддержкой C++17
- Доступ в интернет при первой конфигурации (Google Test скачивается автоматически)

## Сборка и запуск

```bash
cmake -S . -B build
cmake --build build
./build/stack_app
```

## Тесты

```bash
cd build
ctest --output-on-failure
```

Все 22 теста должны пройти. Каждый тест в `tests/stack_test.cpp` снабжён
комментарием, поясняющим, что именно он проверяет.

Отключить сборку тестов: `cmake -S . -B build -DBUILD_TESTS=OFF`

## API стека

| Метод | Описание |
|---|---|
| `TStack(size_t capacity)` | создаёт пустой стек заданной вместимости |
| `push(const T&)` | кладёт элемент на вершину; бросает `std::overflow_error` при переполнении |
| `pop()` | снимает верхний элемент; бросает `std::underflow_error` на пустом стеке |
| `top()` / `top() const` | доступ к верхнему элементу; бросает `std::underflow_error` на пустом стеке |
| `size()` | количество элементов |
| `cap()` | вместимость |
| `empty()` | пуст ли стек |
| конструктор копирования / `operator=` | глубокое копирование, безопасны при самоприсваивании |
