//
// Created by user1 on 2021/11/16.
//
#include "kmer_coder.h"
namespace gjfish {
    CompressedKmer Coder::Encode(Kmer kmer) {
        CompressedKmer compressed_kmer(gjfish::kmer_width);
        compressed_kmer.site = EncodeSite(kmer);
        return compressed_kmer;
    }
    // strand(1) + seg_start_site(31) + seg_idx(32, string)
    uint64_t Coder::EncodeSite(Kmer kmer){
        uint64_t site = 0;
        site |= kmer.strand;
        site <<= 31;
        site |= kmer.seg_start_site;
        site <<=32;
        site |= (uint32_t)std::stol(kmer.seg_idx);
        return site;
    }

    uint64_t* Coder::EncodeKmer(Kmer kmer){

    }

    Kmer Coder::Decode(CompressedKmer compressed_kmer) {
        Kmer kmer;
        kmer.strand = compressed_kmer.site >> 63;
        kmer.seg_start_site = (compressed_kmer.site >> 32) & (((uint64_t )1 << 31) - 1);
        kmer.seg_idx = std::to_string(compressed_kmer.site & (((uint64_t )1 << 32) - 1));
        kmer.sequence = DecodeSeq(compressed_kmer.kmer);
    }

    std::string Coder::DecodeSeq(uint64_t* compressed_seq) {

    }

}

int main()
{
    gjfish::Kmer kmer;
    kmer.strand = 1;
    kmer.sequence = "AGCCCCCCCCGGGGGGGGTTTTTCCGAGCCCCCCCCGGGGGGGGTTTTTCCG";
    kmer.seg_idx = "4294967295";
    kmer.seg_start_site = 0x7FFFFFFF;
    gjfish::Coder coder;
    uint64_t res = coder.EncodeSite(kmer);
    uint32_t strand = res >> 63;
    uint32_t seg_start_site = (res >> 32) & (((uint64_t )1 << 31) - 1);
    uint32_t seg_idx = res & (((uint64_t )1 << 32) - 1);
    std::cout << strand << "| " << seg_start_site << "| " << seg_idx << std::endl;
    return 0;
}