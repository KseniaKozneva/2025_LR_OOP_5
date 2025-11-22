#ifndef SINGLE_LINKED_LIST_H
#define SINGLE_LINKED_LIST_H

#include <iterator>
#include <stdexcept>
#include <memory>
#include <type_traits>
#include <memory_resource>

template <typename T>
class SingleLinkedList {
private:
    struct Node {
        T value;
        Node* next;

        template <typename U>
        Node(U&& val, Node* n = nullptr)
            : value(std::forward<U>(val)), next(n) {}
    };

    using NodeAllocator = std::pmr::polymorphic_allocator<Node>;

    Node* head = nullptr;
    size_t list_size = 0;
    NodeAllocator alloc;

    Node* create_node(const T& value, Node* next = nullptr) {
        Node* node = alloc.allocate(1);
        try {
            std::allocator_traits<NodeAllocator>::construct(alloc, node, value, next);
            return node;
        } catch (...) {
            alloc.deallocate(node, 1);
            throw;
        }
    }

    Node* create_node(T&& value, Node* next = nullptr) {
        Node* node = alloc.allocate(1);
        try {
            std::allocator_traits<NodeAllocator>::construct(alloc, node, std::move(value), next);
            return node;
        } catch (...) {
            alloc.deallocate(node, 1);
            throw;
        }
    }

    void destroy_node(Node* node) {
        if (node) {
            std::allocator_traits<NodeAllocator>::destroy(alloc, node);
            alloc.deallocate(node, 1);
        }
    }

    void destroy_all() {
        while (head) {
            Node* next = head->next;
            destroy_node(head);
            head = next;
        }
        list_size = 0;
    }

    // Копирование узлов с другим аллокатором
    void copy_nodes(const SingleLinkedList& other) {
        Node** current = &head;
        const Node* other_node = other.head;
        while (other_node) {
            *current = create_node(other_node->value);
            current = &((*current)->next);
            other_node = other_node->next;
            ++list_size;
        }
    }

public:
    using value_type = T;
    using allocator_type = std::pmr::polymorphic_allocator<T>;
    using size_type = size_t;

    // Итератор
    class Iterator {
        friend class SingleLinkedList;
        Node* current;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using iterator_category = std::forward_iterator_tag;

        Iterator(Node* node = nullptr) : current(node) {}

        reference operator*() const { return current->value; }
        pointer operator->() const { return &current->value; }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return !(*this == other); }
    };

    // Константный итератор
    class ConstIterator {
        friend class SingleLinkedList;
        const Node* current;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = const T&;
        using pointer = const T*;
        using iterator_category = std::forward_iterator_tag;

        ConstIterator(const Node* node = nullptr) : current(node) {}
        ConstIterator(const Iterator& it) : current(it.current) {}

        reference operator*() const { return current->value; }
        pointer operator->() const { return &current->value; }

        ConstIterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const { return current == other.current; }
        bool operator!=(const ConstIterator& other) const { return !(*this == other); }
    };

    // Конструкторы
    SingleLinkedList() : alloc(std::pmr::get_default_resource()) {}

    explicit SingleLinkedList(std::pmr::memory_resource* resource) 
        : alloc(resource) {}

    explicit SingleLinkedList(const std::pmr::polymorphic_allocator<T>& allocator)
        : alloc(allocator) {}

    // Конструктор копирования
    SingleLinkedList(const SingleLinkedList& other) 
        : alloc(other.alloc) {
        copy_nodes(other);
    }

    // Конструктор перемещения
    SingleLinkedList(SingleLinkedList&& other) noexcept
        : head(other.head), list_size(other.list_size), alloc(other.alloc) {
        other.head = nullptr;
        other.list_size = 0;
    }

    // Оператор присваивания копированием
    SingleLinkedList& operator=(const SingleLinkedList& other) {
        if (this != &other) {
            if (alloc == other.alloc) {
                destroy_all();
                copy_nodes(other);
            } else {
                //Когда аллокаторы разные, мы не можем просто переиспользовать узлы из other, потому что они выделены в другом пуле памяти.
                SingleLinkedList temp(other); // Создает полную копию с ПРАВИЛЬНЫМ аллокатором
                swap(temp);    // Обмениваем содержимое, swap просто меняет указатели head и значения list_size
            }
        }
        return *this;
    }

    SingleLinkedList& operator=(SingleLinkedList&& other) noexcept {
        if (this != &other) {
            if (alloc == other.alloc) {
                destroy_all();
                head = other.head;
                list_size = other.list_size;
                other.head = nullptr;
                other.list_size = 0;
            } else {
                destroy_all();
                copy_nodes(other);
            }
        }
        return *this;
    }

    // Деструктор
    ~SingleLinkedList() {
        destroy_all();
    }

    // Обмен содержимым
    void swap(SingleLinkedList& other) noexcept {
        using std::swap;
        swap(head, other.head);
        swap(list_size, other.list_size);
    }

    // Доступ к элементам
    T& front() {
        if (!head) throw std::logic_error("List is empty");
        return head->value;
    }

    const T& front() const {
        if (!head) throw std::logic_error("List is empty");
        return head->value;
    }

    // Модификаторы
    void push_front(const T& value) {
        head = create_node(value, head);
        ++list_size;
    }

    void push_front(T&& value) {
        head = create_node(std::move(value), head);
        ++list_size;
    }

    void pop_front() {
        if (!head) throw std::logic_error("List is empty");
        Node* old = head;
        head = head->next;
        destroy_node(old);
        --list_size;
    }

    // Итераторы
    Iterator before_begin() { return Iterator(nullptr); }
    ConstIterator before_begin() const { return ConstIterator(nullptr); }

    Iterator begin() { return Iterator(head); }
    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator cbegin() const { return ConstIterator(head); }

    Iterator end() { return Iterator(nullptr); }
    ConstIterator end() const { return ConstIterator(nullptr); }
    ConstIterator cend() const { return ConstIterator(nullptr); }

    // Вставка после позиции
    Iterator insert_after(ConstIterator pos, const T& value) {
        if (pos.current == nullptr) {
            // before_begin(): вставка в начало
            push_front(value);
            return Iterator(head);
        }
        if (!pos.current) {
            throw std::logic_error("Invalid iterator");
        }
        Node* new_node = create_node(value, const_cast<Node*>(pos.current)->next);
        const_cast<Node*>(pos.current)->next = new_node;
        ++list_size;
        return Iterator(new_node);
    }

    Iterator insert_after(ConstIterator pos, T&& value) {
        if (pos.current == nullptr) {
            push_front(std::move(value));
            return Iterator(head);
        }
        if (!pos.current) {
            throw std::logic_error("Invalid iterator");
        }
        Node* new_node = create_node(std::move(value), const_cast<Node*>(pos.current)->next);
        const_cast<Node*>(pos.current)->next = new_node;
        ++list_size;
        return Iterator(new_node);
    }

    // Удаление после позиции
    Iterator erase_after(ConstIterator pos) {
        if (!pos.current || !pos.current->next) {
            throw std::logic_error("Invalid iterator for erase_after");
        }
        Node* to_delete = const_cast<Node*>(pos.current)->next;
        const_cast<Node*>(pos.current)->next = to_delete->next;
        destroy_node(to_delete);
        --list_size;
        return Iterator(const_cast<Node*>(pos.current)->next);
    }

    // Наблюдатели
    bool empty() const { return list_size == 0; }
    size_t size() const { return list_size; }

    void clear() {
        destroy_all();
    }

    // Аллокатор
    allocator_type get_allocator() const { return alloc; }
};

template <typename T>
void swap(SingleLinkedList<T>& lhs, SingleLinkedList<T>& rhs) noexcept {
    lhs.swap(rhs);
}

#endif 