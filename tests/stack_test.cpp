// Юнит-тесты класса TStack<T> (фреймворк Google Test).
// Весь класс (определение + реализация) находится в main.cpp —
// подключаем его сюда через #include. Макрос STACK_SKIP_MAIN
// отключает демонстрационный main() из main.cpp, чтобы не было
// конфликта с точкой входа Google Test внизу этого файла.
#include <gtest/gtest.h>
#define STACK_SKIP_MAIN
#include "../main.cpp"

#include <stdexcept>
#include <string>

// ==================== Базовое поведение ====================

// Тест: стек, созданный конструктором по умолчанию (без аргументов),
// должен быть пустым: empty() == true, size() == 0.
TEST(TStackTest, DefaultConstructedIsEmpty) {
    TStack<int> s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
}

// Тест: push() увеличивает размер стека, а top() возвращает
// именно последний добавленный элемент (последним кладём 3 — его и ждём).
TEST(TStackTest, PushIncreasesSizeAndTopReturnsLast) {
    TStack<int> s(5);
    s.push(1);
    s.push(2);
    s.push(3);

    EXPECT_EQ(s.size(), 3u);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.top(), 3);
}

// Тест: pop() удаляет верхний элемент — после удаления 20 на вершине
// оказывается 10, а после второго pop() стек полностью пуст.
TEST(TStackTest, PopRemovesTopElement) {
    TStack<int> s(5);
    s.push(10);
    s.push(20);

    s.pop();
    EXPECT_EQ(s.top(), 10);
    EXPECT_EQ(s.size(), 1u);

    s.pop();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
}

// Тест порядка LIFO (last in, first out): загоняем в стек слово "hello"
// по буквам и извлекаем через top()+pop() — на выходе должно получиться
// обратное слово "olleh", то есть элементы отдаются в обратном порядке.
TEST(TStackTest, LifoOrder) {
    TStack<char> s(10);
    const std::string word = "hello";
    for (char c : word) {
        s.push(c);
    }

    // Извлекаем в обратном порядке
    std::string out;
    while (!s.empty()) {
        out += s.top();
        s.pop();
    }
    EXPECT_EQ(out, "olleh");
}

// Тест: неконстантная версия top() возвращает модифицируемую ссылку —
// можно изменить верхний элемент напрямую через присваивание s.top() = 42.
TEST(TStackTest, TopMutableReferenceAllowsModification) {
    TStack<int> s(3);
    s.push(1);
    s.top() = 42;
    EXPECT_EQ(s.top(), 42);
}

// Тест: const-версия top() доступна для константной ссылки на стек
// и корректно возвращает верхний элемент для чтения.
TEST(TStackTest, ConstTopWorks) {
    TStack<int> s(3);
    s.push(7);
    const TStack<int>& cs = s;
    EXPECT_EQ(cs.top(), 7);
}

// ==================== Исключения ====================

// Тест: push() в полный стек (вместимость 2, уже лежат 2 элемента)
// бросает std::overflow_error, при этом состояние стека не меняется:
// размер остаётся 2, верхний элемент — 2 (неудачная вставка безопасна).
TEST(TStackTest, PushOnFullStackThrowsOverflow) {
    TStack<int> s(2);
    s.push(1);
    s.push(2);
    EXPECT_THROW(s.push(3), std::overflow_error);
    // Стек не изменился после неудачного push
    EXPECT_EQ(s.size(), 2u);
    EXPECT_EQ(s.top(), 2);
}

// Тест: стек нулевой вместимости (создан без размера) сразу переполнен —
// любой push() обязан бросить std::overflow_error.
TEST(TStackTest, ZeroCapacityStackThrowsOnPush) {
    TStack<int> s;
    EXPECT_THROW(s.push(1), std::overflow_error);
}

// Тест: pop() на пустом стеке бросает std::underflow_error
// (удалять нечего — операция запрещена).
TEST(TStackTest, PopOnEmptyStackThrowsUnderflow) {
    TStack<int> s(5);
    EXPECT_THROW(s.pop(), std::underflow_error);
}

// Тест: top() на пустом стеке бросает std::underflow_error — как до
// единственного push(), так и после того, как единственный элемент удалён.
TEST(TStackTest, TopOnEmptyStackThrowsUnderflow) {
    TStack<int> s(5);
    EXPECT_THROW(s.top(), std::underflow_error);

    s.push(1);
    s.pop();
    EXPECT_THROW(s.top(), std::underflow_error);
}

// Тест: const-версия top() на пустом стеке тоже бросает
// std::underflow_error (проверяется та же защита для константного доступа).
TEST(TStackTest, ConstTopOnEmptyStackThrowsUnderflow) {
    TStack<int> s(5);
    const TStack<int>& cs = s;
    EXPECT_THROW(cs.top(), std::underflow_error);
}

// ==================== Копирование и присваивание ====================

// Тест: конструктор копирования переносит все элементы — копия содержит
// те же 3 элемента, что и оригинал (размер 3, на вершине 3).
TEST(TStackTest, CopyConstructorCopiesElements) {
    TStack<int> s(5);
    s.push(1);
    s.push(2);
    s.push(3);

    TStack<int> copy(s);
    EXPECT_EQ(copy.size(), 3u);
    EXPECT_EQ(copy.top(), 3);
}

// Тест глубокого копирования: после добавления 99 в копию оригинал
// обязан остаться неизменным (верхний элемент 2, размер 2). Если бы
// копирование было поверхностным (общий буфер), эти проверки упали бы.
TEST(TStackTest, CopyIsDeep) {
    TStack<int> s(5);
    s.push(1);
    s.push(2);

    TStack<int> copy(s);
    copy.push(99);

    EXPECT_EQ(s.top(), 2);       // оригинал не изменился
    EXPECT_EQ(copy.top(), 99);
    EXPECT_EQ(s.size(), 2u);
    EXPECT_EQ(copy.size(), 3u);
}

// Тест: копирование пустого (но с ненулевой вместимостью) стека даёт
// корректный пустой стек — size() == 0, empty() == true.
TEST(TStackTest, CopyOfEmptyStack) {
    TStack<int> s(5);
    TStack<int> copy(s);
    EXPECT_TRUE(copy.empty());
    EXPECT_EQ(copy.size(), 0u);
}

// Тест: копирование стека нулевой вместимости не приводит к ошибке
// (не должно быть разыменования nullptr при копировании) — копия тоже
// пуста и отвергает любой push() через std::overflow_error.
TEST(TStackTest, CopyOfZeroCapacityStack) {
    TStack<int> s;
    TStack<int> copy(s);
    EXPECT_TRUE(copy.empty());
    EXPECT_THROW(copy.push(1), std::overflow_error);
}

// Тест: оператор присваивания копирует состояние — b после `b = a`
// содержит элементы a. Дополнительно проверяются: новая вместимость b
// равна вместимости источника (push(3) проходит, хотя раньше b имел
// вместимость 1) и глубина копирования (a не изменился).
TEST(TStackTest, AssignmentOperatorCopiesState) {
    TStack<int> a(5);
    a.push(1);
    a.push(2);

    TStack<int> b(1);
    b = a;

    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.top(), 2);

    // После присваивания вместимость равна вместимости источника
    b.push(3);
    EXPECT_EQ(b.top(), 3);
    EXPECT_EQ(a.top(), 2);  // a не изменился — копирование глубокое
}

// Тест защиты от самоприсваивания (`s = s`): реализация сверяет адреса
// this и other и ничего не освобождает — данные остаются целыми.
// Без этой защиты delete[] собственного буфера привёл бы к UB.
TEST(TStackTest, SelfAssignmentIsSafe) {
    TStack<int> s(3);
    s.push(1);
    s.push(2);

    s = s;

    EXPECT_EQ(s.size(), 2u);
    EXPECT_EQ(s.top(), 2);
}

// Тест: присваивание пустого стека (вместимость 0) непустому — b
// полностью принимает состояние a: становится пустым и больше не
// принимает элементы (push бросает std::overflow_error).
TEST(TStackTest, AssignEmptyToNonEmpty) {
    TStack<int> a;             // нулевая вместимость
    TStack<int> b(5);
    b.push(1);

    b = a;
    EXPECT_TRUE(b.empty());
    EXPECT_EQ(b.size(), 0u);
    EXPECT_THROW(b.push(1), std::overflow_error);
}

// ==================== Работа с другими типами ====================

// Тест: шаблон корректно работает с не- POD типом std::string —
// push/pop/top сохраняют строки в правильном порядке.
TEST(TStackTest, WorksWithStrings) {
    TStack<std::string> s(3);
    s.push("alpha");
    s.push("beta");

    EXPECT_EQ(s.top(), "beta");
    s.pop();
    EXPECT_EQ(s.top(), "alpha");
}

// Тест: работа с вещественным типом double — значения хранятся точно
// (сравнение EXPECT_DOUBLE_EQ учитывает погрешность чисел с плавающей точкой).
TEST(TStackTest, WorksWithDoubles) {
    TStack<double> s(2);
    s.push(3.14);
    s.push(2.71);
    EXPECT_DOUBLE_EQ(s.top(), 2.71);
}

// ==================== Интеграционные тесты (сценарий из main) ====================

// Тест: пошаговое воспроизведение того же сценария, что и в демо-программе
// main() в main.cpp, но с явной проверкой каждого этапа: push×3 → size/top →
// pop → конструктор копирования (глубина) → оператор присваивания.
TEST(TStackTest, MainScenario) {
    TStack<int> s(5);
    s.push(10);
    s.push(20);
    s.push(30);

    ASSERT_EQ(s.size(), 3u);
    ASSERT_EQ(s.top(), 30);

    s.pop();
    EXPECT_EQ(s.top(), 20);

    TStack<int> s2 = s;
    EXPECT_EQ(s2.top(), 20);

    s2.push(99);
    EXPECT_EQ(s.top(), 20);
    EXPECT_EQ(s2.top(), 99);

    TStack<int> s3(2);
    s3 = s;
    EXPECT_EQ(s3.size(), 2u);
    EXPECT_EQ(s3.top(), 20);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
