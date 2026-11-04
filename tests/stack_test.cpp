#include <gtest/gtest.h>
#include "stack.hpp"

#include <stdexcept>
#include <string>

// ==================== Базовое поведение ====================

TEST(TStackTest, DefaultConstructedIsEmpty) {
    TStack<int> s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
}

TEST(TStackTest, PushIncreasesSizeAndTopReturnsLast) {
    TStack<int> s(5);
    s.push(1);
    s.push(2);
    s.push(3);

    EXPECT_EQ(s.size(), 3u);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.top(), 3);
}

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

TEST(TStackTest, TopMutableReferenceAllowsModification) {
    TStack<int> s(3);
    s.push(1);
    s.top() = 42;
    EXPECT_EQ(s.top(), 42);
}

TEST(TStackTest, ConstTopWorks) {
    TStack<int> s(3);
    s.push(7);
    const TStack<int>& cs = s;
    EXPECT_EQ(cs.top(), 7);
}

// ==================== Исключения ====================

TEST(TStackTest, PushOnFullStackThrowsOverflow) {
    TStack<int> s(2);
    s.push(1);
    s.push(2);
    EXPECT_THROW(s.push(3), std::overflow_error);
    // Стек не изменился после неудачного push
    EXPECT_EQ(s.size(), 2u);
    EXPECT_EQ(s.top(), 2);
}

TEST(TStackTest, ZeroCapacityStackThrowsOnPush) {
    TStack<int> s;
    EXPECT_THROW(s.push(1), std::overflow_error);
}

TEST(TStackTest, PopOnEmptyStackThrowsUnderflow) {
    TStack<int> s(5);
    EXPECT_THROW(s.pop(), std::underflow_error);
}

TEST(TStackTest, TopOnEmptyStackThrowsUnderflow) {
    TStack<int> s(5);
    EXPECT_THROW(s.top(), std::underflow_error);

    s.push(1);
    s.pop();
    EXPECT_THROW(s.top(), std::underflow_error);
}

TEST(TStackTest, ConstTopOnEmptyStackThrowsUnderflow) {
    TStack<int> s(5);
    const TStack<int>& cs = s;
    EXPECT_THROW(cs.top(), std::underflow_error);
}

// ==================== Копирование и присваивание ====================

TEST(TStackTest, CopyConstructorCopiesElements) {
    TStack<int> s(5);
    s.push(1);
    s.push(2);
    s.push(3);

    TStack<int> copy(s);
    EXPECT_EQ(copy.size(), 3u);
    EXPECT_EQ(copy.top(), 3);
}

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

TEST(TStackTest, CopyOfEmptyStack) {
    TStack<int> s(5);
    TStack<int> copy(s);
    EXPECT_TRUE(copy.empty());
    EXPECT_EQ(copy.size(), 0u);
}

TEST(TStackTest, CopyOfZeroCapacityStack) {
    TStack<int> s;
    TStack<int> copy(s);
    EXPECT_TRUE(copy.empty());
    EXPECT_THROW(copy.push(1), std::overflow_error);
}

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

TEST(TStackTest, SelfAssignmentIsSafe) {
    TStack<int> s(3);
    s.push(1);
    s.push(2);

    s = s;

    EXPECT_EQ(s.size(), 2u);
    EXPECT_EQ(s.top(), 2);
}

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

TEST(TStackTest, WorksWithStrings) {
    TStack<std::string> s(3);
    s.push("alpha");
    s.push("beta");

    EXPECT_EQ(s.top(), "beta");
    s.pop();
    EXPECT_EQ(s.top(), "alpha");
}

TEST(TStackTest, WorksWithDoubles) {
    TStack<double> s(2);
    s.push(3.14);
    s.push(2.71);
    EXPECT_DOUBLE_EQ(s.top(), 2.71);
}

// ==================== Интеграционный тест (сценарий из main) ====================

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
