#ifndef CUSTOM_MEMORY_RESOURCE_H
#define CUSTOM_MEMORY_RESOURCE_H

#include <memory_resource>
#include <map>
#include <cstddef>
#include <stdexcept>
#include <iostream>

class CustomMemoryResource : public std::pmr::memory_resource {
private:
    struct BlockInfo {
        size_t size;
        size_t alignment;
        bool active;

        BlockInfo() : size(0), alignment(0), active(false) {}
        BlockInfo(size_t s, size_t a) : size(s), alignment(a), active(true) {}
    };

    // Используем стандартный аллокатор для map, чтобы избежать рекурсии
    std::map<void*, BlockInfo> allocated_blocks;
    std::pmr::memory_resource* parent_allocator;

    bool block_fits(const BlockInfo& info, size_t bytes, size_t alignment) const {
        return !info.active && info.size >= bytes && info.alignment >= alignment;
    }

public:
    explicit CustomMemoryResource(std::pmr::memory_resource* parent = nullptr)
        : parent_allocator(parent ? parent : std::pmr::new_delete_resource())
    {}

    void* do_allocate(size_t bytes, size_t alignment) override {
        // Поиск подходящего свободного блока
        for (auto& [ptr, info] : allocated_blocks) {
            if (block_fits(info, bytes, alignment)) {
                info.active = true;
                std::cout << "[CustomMemoryResource] Reused block: ptr=" << ptr 
                         << ", size=" << bytes << ", alignment=" << alignment << "\n";
                return ptr;
            }
        }

        // Выделение нового блока
        void* ptr = parent_allocator->allocate(bytes, alignment);
        allocated_blocks.emplace(ptr, BlockInfo{bytes, alignment});
        std::cout << "[CustomMemoryResource] Allocated new block: ptr=" << ptr 
                 << ", size=" << bytes << ", alignment=" << alignment << "\n";
        return ptr;
    }

    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override {
        // Подавляем предупреждения о неиспользуемых параметрах
        (void)bytes;
        (void)alignment;
        
        auto it = allocated_blocks.find(ptr);
        if (it == allocated_blocks.end()) {
            std::cout << "[CustomMemoryResource] Warning: Unknown pointer deallocated: " << ptr << "\n";
            return;
        }

        if (!it->second.active) {
            throw std::logic_error("Double deallocation detected");
        }

        it->second.active = false;
        std::cout << "[CustomMemoryResource] Deallocated block: ptr=" << ptr 
                 << ", marked as free for reuse\n";
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

    CustomMemoryResource(const CustomMemoryResource&) = delete;
    CustomMemoryResource& operator=(const CustomMemoryResource&) = delete;

    // Статистика
    size_t allocation_count() const noexcept {
        return allocated_blocks.size();
    }

    size_t used_blocks() const noexcept {
        size_t count = 0;
        for (const auto& [ptr, info] : allocated_blocks) {
            if (info.active) ++count;
        }
        return count;
    }

    size_t total_allocated() const noexcept {
        size_t total = 0;
        for (const auto& [ptr, info] : allocated_blocks) {
            total += info.size;
        }
        return total;
    }

    void print_statistics() const {
        std::cout << "\n=== CustomMemoryResource Statistics ===\n";
        std::cout << "Total blocks: " << allocation_count() << "\n";
        std::cout << "Used blocks: " << used_blocks() << "\n";
        std::cout << "Free blocks: " << (allocation_count() - used_blocks()) << "\n";
        std::cout << "Total allocated memory: " << total_allocated() << " bytes\n";
        
        std::cout << "Block details:\n";
        for (const auto& [ptr, info] : allocated_blocks) {
            std::cout << "  ptr=" << ptr << ", size=" << info.size 
                     << ", alignment=" << info.alignment 
                     << ", status=" << (info.active ? "active" : "free") << "\n";
        }
        std::cout << "=====================================\n\n";
    }

    ~CustomMemoryResource() noexcept {
        std::cout << "[CustomMemoryResource] Destructor called\n";
        print_statistics();
        
        for (const auto& [ptr, info] : allocated_blocks) {
            parent_allocator->deallocate(ptr, info.size, info.alignment);
            std::cout << "[CustomMemoryResource] Freed block: ptr=" << ptr 
                     << ", size=" << info.size << "\n";
        }
    }
};

#endif // CUSTOM_MEMORY_RESOURCE_H