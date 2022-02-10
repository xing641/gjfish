//
// Created by user1 on 2021/11/23.
//

#include <stdlib.h>
#include <iostream>
#include "mem_allocator.h"
namespace gjfish{
    MemAllocator::MemAllocator(uint64_t total) : total(total), available(total){};

    void* MemAllocator::mem_allocate(uint64_t size, std::string name) {
        if (size < available){
            void* mem_ptr = malloc(size);
            if (mem_ptr != nullptr) {
                available -= size;
                allocated_count++;
                return mem_ptr;
            }
            std::cout << name << " malloc " << size << " B failed!" << std::endl;
        }
        std::cout << "no memory to allocate" << name << std::endl;
        exit(EXIT_FAILURE);
    }

    void* MemAllocator::mem_aligned_allocate(uint64_t size, std::string name) {
        if (size < available){
            void* mem_ptr;
            posix_memalign(&mem_ptr, 64, size);
            if (mem_ptr != nullptr) {
                available -= size;
                allocated_count++;
                return mem_ptr;
            }
            std::cout << name << " memalign_malloc" << size << " B failed!" << std::endl;
        }
        std::cout << "no memory to memalign_allocate " << name << std::endl;
        exit(EXIT_FAILURE);
    }

    void MemAllocator::mem_free(void *ptr) {
        free(ptr);
        freed_count++;
    }

    MemAllocator::~MemAllocator() {
        if (freed_count == allocated_count){
            //TODO logging
        } else {
            //TODO logging
            std::cout << "memory leak" << std::endl;
        }
    }
}

//int main()
//{
//    uint64_t total = 1024000;
//    gjfish::MemAllocator *ma = new gjfish::MemAllocator(total);
//    int* array = (int*)ma->mem_allocate(sizeof(int) * 60);
//    for (int i = 0 ; i < 60; i++){
//        array[i] = i;
//    }
//    delete ma;
//    return 0;
//}