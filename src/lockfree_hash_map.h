//
// Created by user1 on 2021/11/23.
//

#ifndef SRC_STABLE_LOCKFREE_HASH_MAP_H
#define SRC_STABLE_LOCKFREE_HASH_MAP_H

#include "kmer_coder.h"

namespace gjfish{
    class lockfree_hashtable {
    public:
        // 需要保证是线程安全的无锁操作；
        void set(CompressedKmer kmer);
        uint64_t hashcode(CompressedKmer kmer);


    private:
        uint64_t capacity = 0;
        node** ht;

    };
}



#endif //SRC_STABLE_LOCKFREE_HASH_MAP_H
