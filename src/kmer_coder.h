//
// Created by user1 on 2021/11/16.
//

#ifndef SRC_STABLE_KMER_CODER_H
#define SRC_STABLE_KMER_CODER_H

#include "kmer_counter.h"

namespace gjfish {
    struct CompressedKmer{
        uint32_t kmer[];

    };
    class Coder{
    public:
        CompressedKmer Encode(Kmer kmer) {

        }

        Kmer Decode(CompressedKmer kmer) {

        }
    };
}

#endif //SRC_STABLE_KMER_CODER_H
