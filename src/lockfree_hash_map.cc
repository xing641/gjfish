//
// Created by user1 on 2021/11/23.
//

#include "lockfree_hash_map.h"
#include <iostream>

namespace gjfish{
    LockFreeHashTable::LockFreeHashTable(uint64_t capacity)  {
        this->capacity = capacity;
        prime = max_prime_number(capacity);
        ht = new Node*[prime];
        for (int i = 0; i < prime; i++) {
            ht[i] = nullptr;
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
        Site* new_site = new Site;
        new_site->val = compressed_kmer->site;
        Node* tmp_node = ht[key];
        while (tmp_node != nullptr) {
            if (is_the_same_kmer(tmp_node->kmer, compressed_kmer->kmer)) {
                new_site->next = tmp_node->sites->next;
                tmp_node->sites->next = new_site;
                break;
            }
            tmp_node = tmp_node->next;
        }
        if (tmp_node == nullptr){
            Node* new_node = new Node;
            new_node->kmer = (uint64_t*)malloc(kmer_width * sizeof(uint64_t));
            for (int i = 0; i < kmer_width; i++){
                new_node->kmer[i] = compressed_kmer->kmer[i];
            }
            new_node->sites = new_site;
            new_site->next = nullptr;
            new_node->next = nullptr;
            tmp_node = new_node;
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
    reader->Start();
    reader->GenerateSuperSeg();
    auto *counter = new gjfish::KmerCounter();
    counter->StartCount(reader);
    for (int i = 0; i < counter->ht->prime; i++) {
        if(counter->ht->ht[i] != nullptr){
            std::cout << counter->ht->ht[i]->kmer << std::endl;
        }
    }
    return 0;
}