#include <gtest/gtest.h>
#include <memory_resource>
#include "../include/list.h"
#include "../include/allocator.h"

class SingleLinkedListTest : public ::testing::Test {
protected:
    void SetUp() override { //Создает кастомный аллокатор перед каждым тестом
        resource = std::make_unique<CustomMemoryResource>();
    }

    void TearDown() override { // Очищает аллокатор после каждого теста
        resource.reset();
    }

    std::unique_ptr<CustomMemoryResource> resource;
};

// Базовые операции
TEST_F(SingleLinkedListTest, DefaultConstructor) {
    SingleLinkedList<int> list(resource.get());
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST_F(SingleLinkedListTest, PushFrontAndFront) {
    SingleLinkedList<int> list(resource.get());
    
    list.push_front(10);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 10);
    
    list.push_front(20);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), 20);
}

TEST_F(SingleLinkedListTest, PopFront) {
    SingleLinkedList<int> list(resource.get());
    
    list.push_front(10);
    list.push_front(20);
    
    list.pop_front();
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 10);
    
    list.pop_front();
    EXPECT_TRUE(list.empty());
}

TEST_F(SingleLinkedListTest, PopFrontEmptyList) {
    SingleLinkedList<int> list(resource.get());
    EXPECT_THROW(list.pop_front(), std::logic_error);
}

// Итераторы
TEST_F(SingleLinkedListTest, IteratorOperations) {
    SingleLinkedList<int> list(resource.get());
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    
    auto it = list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, list.end());
}

TEST_F(SingleLinkedListTest, ConstIterator) {
    SingleLinkedList<int> list(resource.get());
    list.push_front(42);
    
    const auto& const_list = list;
    auto it = const_list.begin();
    EXPECT_EQ(*it, 42);
    EXPECT_EQ(it, const_list.cbegin());
}

TEST_F(SingleLinkedListTest, RangeBasedFor) {
    SingleLinkedList<int> list(resource.get());
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    
    std::vector<int> result;
    for (const auto& item : list) {
        result.push_back(item);
    }
    
    EXPECT_EQ(result, std::vector<int>({1, 2, 3}));
}

// Вставка и удаление
TEST_F(SingleLinkedListTest, InsertAfter) {
    SingleLinkedList<int> list(resource.get());
    list.push_front(1);
    list.push_front(3); 
    
    auto it = list.begin();
    list.insert_after(it, 2); 
    
    std::vector<int> result;
    for (const auto& item : list) {
        result.push_back(item);
    }
    EXPECT_EQ(result, std::vector<int>({3, 2, 1}));
}

TEST_F(SingleLinkedListTest, InsertAfterBegin) {
    SingleLinkedList<int> list(resource.get());
    
    list.insert_after(list.before_begin(), 1);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.size(), 1);
}

TEST_F(SingleLinkedListTest, EraseAfter) {
    SingleLinkedList<int> list(resource.get());
    list.push_front(1);
    list.push_front(2);
    list.push_front(3); 
    
    auto it = list.begin(); 
    list.erase_after(it); 
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), 3);
    
    auto next_it = ++list.begin();
    EXPECT_EQ(*next_it, 1);
}

TEST_F(SingleLinkedListTest, EraseAfterInvalid) {
    SingleLinkedList<int> list(resource.get());
    list.push_front(1);
    
    auto it = list.begin();
    EXPECT_THROW(list.erase_after(it), std::logic_error); 
}

// Копирование и перемещение
TEST_F(SingleLinkedListTest, CopyConstructor) {
    SingleLinkedList<int> original(resource.get());
    original.push_front(2);
    original.push_front(1);
    
    SingleLinkedList<int> copy(original);
    
    EXPECT_EQ(original.size(), copy.size());
    
    auto orig_it = original.begin();
    auto copy_it = copy.begin();
    while (orig_it != original.end() && copy_it != copy.end()) {
        EXPECT_EQ(*orig_it, *copy_it);
        ++orig_it;
        ++copy_it;
    }
}

TEST_F(SingleLinkedListTest, MoveConstructor) {
    SingleLinkedList<int> original(resource.get());
    original.push_front(2);
    original.push_front(1);
    
    SingleLinkedList<int> moved(std::move(original));
    
    EXPECT_TRUE(original.empty());
    EXPECT_EQ(moved.size(), 2);
    
    std::vector<int> result;
    for (const auto& item : moved) {
        result.push_back(item);
    }
    EXPECT_EQ(result, std::vector<int>({1, 2}));
}

TEST_F(SingleLinkedListTest, CopyAssignment) {
    SingleLinkedList<int> original(resource.get());
    original.push_front(2);
    original.push_front(1);
    
    SingleLinkedList<int> copy(resource.get());
    copy = original;
    
    EXPECT_EQ(original.size(), copy.size());
    
    auto orig_it = original.begin();
    auto copy_it = copy.begin();
    while (orig_it != original.end() && copy_it != copy.end()) {
        EXPECT_EQ(*orig_it, *copy_it);
        ++orig_it;
        ++copy_it;
    }
}

TEST_F(SingleLinkedListTest, MoveAssignment) {
    SingleLinkedList<int> original(resource.get());
    original.push_front(2);
    original.push_front(1);
    
    SingleLinkedList<int> moved(resource.get()); // Одинаковые аллокаторы
    moved = std::move(original);
    
    EXPECT_TRUE(original.empty());
    EXPECT_EQ(moved.size(), 2);
    
    std::vector<int> result;
    for (const auto& item : moved) {
        result.push_back(item);
    }
    EXPECT_EQ(result, std::vector<int>({1, 2}));
}
// Сложные типы данных
TEST_F(SingleLinkedListTest, ComplexTypes) {
    SingleLinkedList<std::string> list(resource.get());
    
    list.push_front("world");
    list.push_front("hello");
    
    EXPECT_EQ(list.front(), "hello");
    list.pop_front();
    EXPECT_EQ(list.front(), "world");
}

TEST_F(SingleLinkedListTest, CustomStruct) {
    struct Point {
        int x, y;
        Point(int x, int y) : x(x), y(y) {}
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };
    
    SingleLinkedList<Point> list(resource.get());
    list.push_front(Point(2, 3));
    list.push_front(Point(1, 2));
    
    EXPECT_EQ(list.front(), Point(1, 2));
}

// Очистка
TEST_F(SingleLinkedListTest, Clear) {
    SingleLinkedList<int> list(resource.get());
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    
    EXPECT_EQ(list.size(), 3);
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}



// Тесты для CustomMemoryResource
TEST_F(SingleLinkedListTest, AllocatorReuseMemory) {
    // Тест на повторное использование памяти
    CustomMemoryResource allocator;
    
    // Выделяем и освобождаем память
    void* ptr1 = allocator.allocate(100, 8);
    allocator.deallocate(ptr1, 100, 8);
    
    // Снова выделяем - должен переиспользовать тот же блок
    void* ptr2 = allocator.allocate(100, 8);
    EXPECT_EQ(ptr1, ptr2) << "Memory should be reused";
}

TEST_F(SingleLinkedListTest, AllocatorDifferentSizes) {
    // Тест на работу с разными размерами блоков
    CustomMemoryResource allocator;
    
    void* small_ptr = allocator.allocate(50, 8);
    void* large_ptr = allocator.allocate(200, 8);
    
    EXPECT_NE(small_ptr, large_ptr);
    
    allocator.deallocate(small_ptr, 50, 8);
    allocator.deallocate(large_ptr, 200, 8);
}

TEST_F(SingleLinkedListTest, AllocatorAlignment) {
    // Тест на корректность выравнивания
    CustomMemoryResource allocator;
    
    // Запрашиваем разное выравнивание
    void* ptr1 = allocator.allocate(64, 16);
    void* ptr2 = allocator.allocate(64, 32);
    
    // Проверяем что указатели правильно выровнены
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 32, 0);
    
    allocator.deallocate(ptr1, 64, 16);
    allocator.deallocate(ptr2, 64, 32);
}

TEST_F(SingleLinkedListTest, AllocatorDoubleFreeDetection) {
    // Тест на обнаружение двойного освобождения
    CustomMemoryResource allocator;
    
    void* ptr = allocator.allocate(100, 8);
    allocator.deallocate(ptr, 100, 8);
    
    EXPECT_THROW(allocator.deallocate(ptr, 100, 8), std::logic_error);
}

TEST_F(SingleLinkedListTest, AllocatorUnknownPointer) {
    // Тест на обработку неизвестного указателя
    CustomMemoryResource allocator;
    
    int dummy;
    void* unknown_ptr = &dummy;
    
    // Не должно бросать исключение, только предупреждение в консоль
    EXPECT_NO_THROW(allocator.deallocate(unknown_ptr, 100, 8));
}

TEST_F(SingleLinkedListTest, AllocatorWithListIntegration) {
    // Интеграционный тест аллокатора со списком
    CustomMemoryResource custom_allocator;
    
    // Создаем список с кастомным аллокатором
    SingleLinkedList<int> list(&custom_allocator);
    
    // Выполняем различные операции
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    
    EXPECT_EQ(list.size(), 3);
    
    list.pop_front();
    EXPECT_EQ(list.size(), 2);
    
    // Добавляем еще элементов - аллокатор должен переиспользовать память
    list.push_front(4);
    list.push_front(5);
    
    EXPECT_EQ(list.size(), 4);
    
    std::vector<int> result;
    for (const auto& item : list) {
        result.push_back(item);
    }
    EXPECT_EQ(result, std::vector<int>({5, 4, 2, 1}));
}

TEST_F(SingleLinkedListTest, AllocatorMemoryReuseAfterClear) {
    // Тест переиспользования памяти после очистки списка
    CustomMemoryResource custom_allocator;
    SingleLinkedList<int> list(&custom_allocator);
    
    for (int i = 0; i < 5; ++i) {
        list.push_front(i);
    }
    
    list.clear();
    EXPECT_TRUE(list.empty());
    
    // Снова заполняем - аллокатор должен переиспользовать память
    for (int i = 0; i < 5; ++i) {
        list.push_front(i * 10);
    }
    
    EXPECT_EQ(list.size(), 5);
    
    std::vector<int> result;
    for (const auto& item : list) {
        result.push_back(item);
    }
    EXPECT_EQ(result, std::vector<int>({40, 30, 20, 10, 0}));
}

