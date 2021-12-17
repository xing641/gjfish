//
// Created by user1 on 2021/11/23.
//

#include "lockfree_hash_map.h"
#include <iostream>

namespace gjfish{
    LockFreeHashTable::LockFreeHashTable(uint64_t capacity, MemAllocator* ma)  {
        this->ma = ma;
        this->capacity = capacity;
        prime = max_prime_number(capacity);
        nodes = new Node*[prime];
        for (int i = 0; i < prime; i++) {
            nodes[i] = new Node;
            nodes[i]->next = nullptr;
            nodes[i]->kmer = nullptr;
            nodes[i]->cnt = 0;
        }
    }
    uint64_t LockFreeHashTable::get_hashcode(CompressedKmer* compressed_kmer) {
        uint64_t tmp = 0;
        for (int i = 0; i < kmer_width; i++){
            tmp += compressed_kmer->kmer[i];
        }
        return tmp % prime;
    }
    void LockFreeHashTable::add_kmer(CompressedKmer* compressed_kmer) {
        uint64_t key = get_hashcode(compressed_kmer);
        mu.lock();
        Node* node_ptr = nodes[key]->next;
        while(node_ptr != nullptr){
            if (is_the_same_kmer(node_ptr->kmer, compressed_kmer->kmer)){
                node_ptr->cnt++;
                break;
            }
            node_ptr = node_ptr->next;
        }
        if(node_ptr == nullptr) {
            Node* node = new Node;
            for (int i = 0; i < kmer_width; i++){
                node->kmer = (uint64_t*)malloc(sizeof(uint64_t) * kmer_width);
                node->kmer[i] = compressed_kmer->kmer[i];
            }
            node->next = nodes[key]->next;
            node->cnt = 1;
            nodes[key]->next = node;
        }

        mu.unlock();
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

    static inline bool is_prime_number(uint64_t n) {
        if (n < 2)
            return false;
        for (uint64_t i = 2; i * i <= n; i++) {
            if (n % i == 0)
                return false;
        }
        return true;
    }
    static inline bool is_the_same_kmer(const uint64_t* kmer1, const uint64_t* kmer2) {
        for (int i = 0; i < kmer_width; i++) {
            if(kmer1[i] != kmer2[i])
                return false;
        }
        return true;
    }

}

int main()
{
    auto *reader = new gjfish::GFAReader("../test/MT.gfa");
    auto *ma = new gjfish::MemAllocator(1000);

    reader->Start();
    reader->GenerateSuperSeg();
    auto *counter = new gjfish::KmerCounter(ma, reader);

    counter->StartCount();
    for (int i = 0; i < counter->ht->prime; i++) {
        gjfish::Node* node_ptr = counter->ht->nodes[i]->next;
        while(node_ptr != nullptr){
            std::cout << counter->ht->nodes[i]->next->cnt << " ";
            node_ptr = node_ptr->next;
        }
        std::cout << std::endl;
    }
    return 0;
}