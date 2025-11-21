#include "CustomMemoryResource.hpp"

void* CustomMemoryResource::do_allocate(size_t bytes, size_t alignment) {
    size_t allocation_offset = 0;
    
    for (const MemoryBlock& block : used_blocks) {
        if ((allocation_offset + bytes <= block.offset) || 
            (allocation_offset >= block.offset + block.size)) {
        } else {
            allocation_offset = block.offset + block.size + 1;
        }
    }

    if (allocation_offset + bytes >= BUFFER_SIZE) {
        throw std::bad_alloc();
    }

    used_blocks.emplace_back(allocation_offset, bytes);
    
    used_blocks.sort([](const MemoryBlock& left, const MemoryBlock& right) {
        return left.offset < right.offset;
    });
    
    return memory_buffer + allocation_offset;
}

void CustomMemoryResource::do_deallocate(void* ptr, size_t bytes, size_t alignment) {        
    for (auto it = used_blocks.begin(); it != used_blocks.end(); ++it) {
        if (ptr == memory_buffer + it->offset) {
            used_blocks.erase(it);
            return;
        }
    }
    throw std::logic_error("An attempt to release an unallocated block");
}

bool CustomMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}
