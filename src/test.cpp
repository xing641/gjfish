//
// Created by user1 on 2021/12/20.
//
#include "gfa_reader.h"
#include "kmer_counter.h"
#include "kmer_coder.h"
#include "lockfree_hash_map.h"
#include "mem_allocator.h"


int main()
{
    uint64_t total = 1;
    gjfish::MemAllocator *ma = new gjfish::MemAllocator(total);
    int* array = (int*)ma->mem_allocate(sizeof(int) * 60);
    std::cout << "???" << std::endl;
    for (int i = 0 ; i < 60; i++){
        array[i] = i;
    }
    delete ma;
    return 0;
}