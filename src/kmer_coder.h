//
// Created by user1 on 2021/11/16.
//

#ifndef SRC_STABLE_KMER_CODER_H
#define SRC_STABLE_KMER_CODER_H

#include "kmer_counter.h"

namespace gjfish {
    // seg_idx + start_site + strand
    struct CompressedKmer{
        uint64_t site;
        uint64_t* kmer;
        CompressedKmer(uint64_t width) {
            kmer = (uint64_t* )malloc(sizeof(uint64_t) * width);
        }
        ~CompressedKmer(){
            delete kmer;
        }
    };
    // 将kmer -> compressedkmer
    class Coder{
    public:
        CompressedKmer Encode(Kmer kmer);
        uint64_t EncodeSite(Kmer kmer);
        uint64_t* EncodeKmer(Kmer kmer);
        Kmer Decode(CompressedKmer compressed_kmer);
        std::string DecodeSeq(uint64_t* compressed_seq);
    };
}

#endif //SRC_STABLE_KMER_CODER_H
