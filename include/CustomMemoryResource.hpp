#include <iostream>
#include <memory_resource>
#include <list>
#include <concepts>
#include <memory>
#include <exception>
#include <algorithm>

class CustomMemoryResource : public std::pmr::memory_resource {
    struct MemoryBlock {
        size_t offset{0};
        size_t size{0}; 
    };

    static constexpr size_t BUFFER_SIZE{1024}; 
    char memory_buffer[BUFFER_SIZE];
    std::list<MemoryBlock> used_blocks;

public:
    void* do_allocate(size_t bytes, size_t alignment) override;

    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override;

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
};
