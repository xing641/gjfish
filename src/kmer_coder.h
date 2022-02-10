//
// Created by user1 on 2021/11/16.
//

#ifndef SRC_STABLE_KMER_CODER_H
#define SRC_STABLE_KMER_CODER_H

#include "kmer_counter.h"

namespace gjfish {
    class Kmer;
    // seg_idx + start_site + strand, size = 16
    struct CompressedKmer{
        uint64_t site;
        uint64_t* kmer;
        CompressedKmer(uint64_t width);
        ~CompressedKmer();
    };
    // 将kmer -> compressedkmer
    class Coder{
    public:
        gjfish::Param param;

        Coder(gjfish::Param param);

        CompressedKmer *Encode(Kmer &kmer);
        void EncodeSite(Kmer &kmer, CompressedKmer* compressed_kmer);
        void EncodeKmer(Kmer &kmer, CompressedKmer* compressed_kmer);
        Kmer Decode(CompressedKmer& compressed_kmer);
        std::string DecodeKmer(uint64_t* compressed_seq);
        uint32_t SegIdxToInt(const std::string &seg_idx);

    };
    // A: C: G: T = 0: 1: 2: 3;
    const uint64_t ENCODE_MER_TABLE[128] = {
            4, 4, 4, 4,        4, 4, 4, 4,      4, 4, 4, 4,      4, 4, 4, 4,
            4, 4, 4, 4,        4, 4, 4, 4,      4, 4, 4, 4,      4, 4, 4, 4,
            4, 4, 4, 4,        4, 4, 4, 4,      4, 4, 4, 4,      4, 4, 4, 4,
            4, 4, 4, 4,        4, 4, 4, 4,      4, 4, 4, 4,      4, 4, 4, 4,
            4, 0, 4, 1,        4, 4, 4, 2,      4, 4, 4, 4,      4, 4, 4, 4,
            4, 4, 4, 4,        3, 3, 4, 4,      4, 4, 4, 4,      4, 4, 4, 4,
            4, 0, 4, 1,        4, 4, 4, 2,      4, 4, 4, 4,      4, 4, 4, 4,
            4, 4, 4, 4,        3, 3, 4, 4,      4, 4, 4, 4,      4, 4, 4, 4,
    };

    const char DECODE_MER_TABLE[4] = {65, 67, 71, 84};
}

#endif //SRC_STABLE_KMER_CODER_H
