//
// Created by user1 on 2021/11/23.
//

#ifndef SRC_STABLE_MEM_ALLOCATOR_H
#define SRC_STABLE_MEM_ALLOCATOR_H
#include <cstdint>

namespace gjfish{
    // 内存分配还可以优化
    class MemAllocator {
    public:
        uint64_t total;
        uint64_t available;

        // 为了判断是否内存泄漏，如果用智能指针或许就不需要了。
        uint64_t allocated_count = 0;
        uint64_t freed_count = 0;

        MemAllocator(uint64_t total);

        void* mem_allocate(uint64_t size);
        void* mem_aligned_allocate(uint64_t size);
        void mem_free(void* ptr);

        ~MemAllocator();


    };
}



#endif //SRC_STABLE_MEM_ALLOCATOR_H
