//
// Created by user1 on 2021/11/23.
//

#ifndef SRC_STABLE_LOCKFREE_HASH_MAP_H
#define SRC_STABLE_LOCKFREE_HASH_MAP_H

#include "kmer_coder.h"
#include "mem_allocator.h"
#include <mutex>
#include <atomic>

namespace gjfish{
    class Kmer;
    class CompressedKmer;
    class MemAllocator;
    struct Node{
        uint64_t* kmer;
        Node* next;
        uint64_t cnt;
    };
    class LockFreeHashTable {
    public:
        uint64_t prime;
        Node** nodes;
        MemAllocator* ma;

        // 需要保证是线程安全的无锁操作；
        LockFreeHashTable(uint64_t capacity, MemAllocator *ma);
        void add_kmer(CompressedKmer* compressed_kmer);
        uint64_t get_hashcode(CompressedKmer* compressed_kmer);
        uint64_t max_prime_number(uint64_t limit);




    private:
        uint64_t capacity = 0;
        std::mutex mu;

        bool keys_locked;
        pthread_barrier_t barrier;
    };
    static
    bool is_prime_number(uint64_t n);
    static bool is_the_same_kmer(const uint64_t* kmer1, const uint64_t* kmer2);
}



#endif //SRC_STABLE_LOCKFREE_HASH_MAP_H
