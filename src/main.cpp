#include <iostream>
#include <string>
#include "../include/allocator.h"
#include "../include/list.h"

struct Person {
    int id;
    std::string name;
    int age;
    
    Person(int i = 0, std::string n = "", int a = 0) 
        : id(i), name(std::move(n)), age(a) {}
    
    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        os << "Person{id=" << p.id << ", name='" << p.name << "', age=" << p.age << "}";
        return os;
    }
};

void demo_simple_types() {
    std::cout << "=== DEMONSTRATION WITH SIMPLE TYPES (int) ===\n";
    
    // Каждая демо использует свой аллокатор для чистоты статистики
    auto custom_resource = std::make_unique<CustomMemoryResource>();
    
    SingleLinkedList<int> int_list(custom_resource.get());
    
    std::cout << "Pushing elements to int list...\n";
    int_list.push_front(10);
    int_list.push_front(20);
    int_list.push_front(30);
    int_list.push_front(40);
    
    std::cout << "List contents: ";
    for (const auto& val : int_list) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    
    std::cout << "List size: " << int_list.size() << "\n";
    std::cout << "Front element: " << int_list.front() << "\n";
    
    std::cout << "Popping front element...\n";
    int_list.pop_front();
    
    std::cout << "List after pop_front: ";
    for (const auto& val : int_list) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}

void demo_complex_types() {
    std::cout << "\n=== DEMONSTRATION WITH COMPLEX TYPES (Person) ===\n";
    
    auto custom_resource = std::make_unique<CustomMemoryResource>();
    SingleLinkedList<Person> person_list(custom_resource.get());
    
    std::cout << "Pushing Person objects to list...\n";
    person_list.push_front(Person(1, "Alice", 25));
    person_list.push_front(Person(2, "Bob", 30));
    person_list.push_front(Person(3, "Charlie", 35));
    
    std::cout << "List contents:\n";
    for (const auto& person : person_list) {
        std::cout << "  " << person << "\n";
    }
}

void demo_iterator_operations() {
    std::cout << "\n=== DEMONSTRATION OF ITERATOR OPERATIONS ===\n";
    
    auto custom_resource = std::make_unique<CustomMemoryResource>();
    SingleLinkedList<std::string> string_list(custom_resource.get());
    
    string_list.push_front("Third");
    string_list.push_front("Second");
    string_list.push_front("First");
    
    std::cout << "Using iterators:\n";
    auto it = string_list.begin();
    int count = 1;
    while (it != string_list.end()) {
        std::cout << "  " << count++ << ": " << *it << "\n";
        ++it;
    }
}

int main() {
    std::cout << "=== SINGLE LINKED LIST WITH CUSTOM MEMORY RESOURCE DEMO ===\n\n";
    
    try {
        demo_simple_types();
        demo_complex_types();
        demo_iterator_operations();
        
        std::cout << "\n=== ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}