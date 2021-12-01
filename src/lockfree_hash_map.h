//
// Created by user1 on 2021/11/23.
//

#ifndef SRC_STABLE_LOCKFREE_HASH_MAP_H
#define SRC_STABLE_LOCKFREE_HASH_MAP_H

#include "kmer_coder.h"
#include <mutex>
#include <atomic>

namespace gjfish{
    class Kmer;
    class CompressedKmer;
    struct Site{
        Site* next;
        uint64_t val;
    };
    struct Node{
        uint64_t* kmer;
        Node* next;
        Site* sites;
    };
    class LockFreeHashTable {
    public:
        // 需要保证是线程安全的无锁操作；
        LockFreeHashTable(uint64_t capacity);
        void add_kmer(CompressedKmer* compressed_kmer);
        uint64_t get_hashcode(CompressedKmer* compressed_kmer);
        uint64_t max_prime_number(uint64_t limit);
        uint64_t prime;
        Node * *ht;



    private:
        uint64_t capacity = 0;
        std::mutex mu;

        bool keys_locked;
        pthread_barrier_t barrier;
    };
    static bool is_prime_number(uint64_t n);
    static bool is_the_same_kmer(const uint64_t* kmer1, const uint64_t* kmer2);
}



#endif //SRC_STABLE_LOCKFREE_HASH_MAP_H
