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
        uint64_t next;
        uint64_t cnt;
        uint64_t kmer[];
    };
    struct Block{
        uint64_t start_id;
        uint64_t end_id;

        uint64_t current_id;

        uint64_t next_id;
        bool synced;
    };

    class LockFreeHashTable {
    public:
        gjfish::Param param;

        uint64_t *table;

        uint64_t blocks_count;
        Block **blocks;

        Node* nodes;
        uint32_t kmer_size;
        uint64_t node_size;

        MemAllocator* ma;

        // 需要保证是线程安全的无锁操作；
        LockFreeHashTable(MemAllocator *ma, gjfish::Param param);
        ~LockFreeHashTable();
        uint64_t get_hashcode(uint64_t *kmer);
        uint64_t max_prime_number(uint64_t limit);
        bool is_the_same_kmer(const uint64_t* kmer1, const uint64_t* kmer2);

        Node* get_node(uint64_t node_id);
        uint64_t request_node(size_t n);
        uint64_t polling_request_node(size_t n);

        uint64_t collision_list_add_kmer(uint64_t** list, uint64_t* kmer);
        bool add_kmer(size_t n, CompressedKmer &compressed_kmer);
        void copy_kmer(uint64_t* kmer1, uint64_t* kmer2);

        /* 哈希函数 */
        // simple function
        uint64_t SimpleHash(uint64_t *kmer);

        // hash_function
        uint64_t SDBMHash(char *str);

        // RS Hash Function
        uint64_t RSHash(char *str);

        // JS Hash Function
        uint64_t JSHash(char *str);

        // P. J. Weinberger Hash Function
        uint64_t PJWHash(char *str);

        // ELF Hash Function
        uint64_t ELFHash(char *str);

        // BKDR Hash Function
        uint64_t BKDRHash(char *str);

        // DJB Hash Function
        uint64_t DJBHash(char *str);


        // AP Hash Function
        uint64_t APHash(char *str);

    private:
        uint64_t table_capacity = 0;
        std::mutex mu;

        bool keys_locked = false;
        pthread_barrier_t barrier;
    };
    static bool is_prime_number(uint64_t n);
}



#endif //SRC_STABLE_LOCKFREE_HASH_MAP_H
