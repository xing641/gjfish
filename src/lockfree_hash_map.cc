//
// Created by user1 on 2021/11/23.
//

#include "lockfree_hash_map.h"
#include <iostream>
#include <cstring>

namespace gjfish{
    LockFreeHashTable::LockFreeHashTable(MemAllocator* ma, gjfish::Param param)  : param(param){
        this->ma = ma;

        // allocate blocks
        blocks_count = param.threads_count;
        blocks = (Block**)ma->mem_allocate(sizeof(Block*) * blocks_count);
        for (int i = 0; i < blocks_count; i++){
            blocks[i] = (Block*)ma->mem_allocate(sizeof(Block));
        }

        // count nodes num
        kmer_size = sizeof(uint64_t) * param.kmer_width;
        node_size = kmer_size + sizeof(Node);

        uint64_t nodes_count = ma->available / (node_size * 3 + sizeof(uint64_t) * 4) * 3;
        uint64_t nodes_mem = node_size * nodes_count;

        // allocate table
        uint64_t table_mem_max = ma->available - nodes_mem;
        uint64_t table_capacity_max = table_mem_max / sizeof(uint64_t);
        table_capacity = max_prime_number(table_capacity_max);
        uint64_t table_mem = table_capacity * sizeof(uint64_t);
        table = (uint64_t*)ma->mem_allocate(table_mem);
        for (uint64_t i = 0; i < table_capacity; i++) {
            table[i] = 0;
        }

        // allocate nodes
        nodes = (Node*)ma->mem_allocate(nodes_mem);
        uint64_t block_node_size = nodes_count / blocks_count;
        for (int i = 0; i < blocks_count; i++) {
            blocks[i]->start_id = 1 + block_node_size * (uint64_t)i;
            blocks[i]->end_id = (i == blocks_count - 1) ? (nodes_count) : (1 + block_node_size * (uint64_t)(i + 1));
            blocks[i]->next_id = blocks[i]->start_id;
            blocks[i]->current_id = 0;
            blocks[i]->synced = false;
        }

        pthread_barrier_init(&(barrier), NULL, (unsigned int)param.threads_count);
    }

    LockFreeHashTable::~LockFreeHashTable() {
        ma->mem_free(nodes);
        ma->mem_free(table);
        for (int i = 0; i < blocks_count; i++){
            ma->mem_free(blocks[i]);
        }
        ma->mem_free(blocks);
    }

    uint64_t LockFreeHashTable::get_hashcode(uint64_t * kmer) {
        uint64_t tmp = 0;
        for (int i = 0; i < param.kmer_width; i++){
            tmp += kmer[i];
        }
        return tmp % table_capacity;
    }

    void LockFreeHashTable::copy_kmer(uint64_t *kmer1, uint64_t *kmer2){
        memcpy(kmer1, kmer2, kmer_size);
    }

    Node* LockFreeHashTable::get_node(uint64_t node_id) {
        char* node = (char*)nodes;
        node += node_size * node_id;
        return (Node*)node;
    }

    uint64_t LockFreeHashTable::request_node(size_t n) {
        Block* block = blocks[n];
        uint64_t node_id;

        do {
            node_id = block->next_id;
            if (node_id == block->end_id) {
                return 0;
            }
        } while (!__sync_bool_compare_and_swap(&(block->next_id), node_id, node_id + 1));

        Node* node = get_node(node_id);
        node->cnt = 0;

        return node_id;
    }

    uint64_t LockFreeHashTable::polling_request_node(size_t n) {
        uint64_t node_id = request_node(n);
        if (node_id != 0) {
            return node_id;
        }

        size_t m = blocks_count;
        for (size_t i = 0; i < m - 1; i++) {
            n++;
            if (n == m)
                n = 0;

            node_id = request_node(n);
            if (node_id != 0) {
                return node_id;
            }
        }
        return 0;
    }

    uint64_t LockFreeHashTable::collision_list_add_kmer(uint64_t **list, uint64_t* kmer) {
        uint64_t node_id;
        uint64_t* p = *list;
        while (true) {
            node_id = *p;

            if (node_id == 0) {
                break;
            }

            Node* node = get_node(node_id);
            if (is_the_same_kmer(node->kmer, kmer)) {
                uint32_t count;
                do {
                    count = node->cnt;
                    if (count == UINT32_MAX)
                        break;
                } while (!__sync_bool_compare_and_swap(&(node->cnt), count, count + 1));

                break;
            }
            p = &(node->next);
        }

        *list = p;
        return node_id;
    }

    bool LockFreeHashTable::add_kmer(size_t n, CompressedKmer* compressed_kmer) {
        Block* block = blocks[n];

        if (!(block->synced) && (block->current_id == 0)) {
            block->current_id = polling_request_node(n);
            if (block->current_id == 0) {
                keys_locked = true;
            }
        }

        // For multi-threaded situation, when the keys are locked by one of the working threads, the other threads may
        // not yet knew the change, so they need to sync once. As the failure of adding K-mer may occur a long time later
        // (if one thread adds many existed K-mers continuously), checking if the keys of the hash map are locked is a
        // good signal.

        if (!(block->synced) && keys_locked) {
            pthread_barrier_wait(&(barrier));
            block->synced = true;
        }

        size_t table_idx = get_hashcode(compressed_kmer->kmer);

        uint64_t * collision_list = &(table[table_idx]);
        uint64_t node_id = collision_list_add_kmer(&collision_list, compressed_kmer->kmer);

        if (node_id != 0) {
            return true;
        }

        // If some thread has set keys_locked, assume this thread noticed the change here and return false, while the other
        // thread has not seen the change and is adding a new node to hash table, then it may cause inconsistency.
        // Checking if this thread has been synced is important.
        if (block->synced && keys_locked) {
            return false;
        }

        Node *node = get_node(block->current_id);
        copy_kmer(node->kmer, compressed_kmer->kmer);
        node->cnt = 1;
        node->next = 0;

        do {
            node_id = collision_list_add_kmer(&collision_list, compressed_kmer->kmer);
            if (node_id != 0) {
                // Mark the node invalid.
                node->cnt = 0;
                return true;
            }
        } while (!__sync_bool_compare_and_swap(collision_list, node_id, block->current_id));

        block->current_id = 0;

        return true;
    }

    uint64_t LockFreeHashTable::max_prime_number(uint64_t limit){
        uint64_t n = limit;
        while (!is_prime_number(n) && (n > 0))
            n--;
        if (n == 0) {
            exit(EXIT_FAILURE);
        }
        return n;
    }

    bool LockFreeHashTable::is_the_same_kmer(const uint64_t* kmer1, const uint64_t* kmer2) {
        for (int i = 0; i < param.kmer_width; i++) {
            if(kmer1[i] != kmer2[i])
                return false;
        }
        return true;
    }

    static inline bool is_prime_number(uint64_t n) {
        if (n < 2)
            return false;
        for (uint64_t i = 2; i * i <= n; i++) {
            if (n % i == 0)
                return false;
        }
        return true;
    }

}

