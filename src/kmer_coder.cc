//
// Created by user1 on 2021/11/16.
//
#include "kmer_coder.h"
#include <algorithm>
namespace gjfish {
    Coder::Coder(gjfish::Param param) : param(param){};

    CompressedKmer* Coder::Encode(Kmer& kmer) {
        CompressedKmer *compressed_kmer = new CompressedKmer(param.kmer_width);
        EncodeSite(kmer, compressed_kmer);
        EncodeKmer(kmer, compressed_kmer);
        return compressed_kmer;
    }
    // strand(1) + seg_start_site(31) + seg_idx(32, string)
    void Coder::EncodeSite(Kmer &kmer, CompressedKmer* compressed_kmer){
        uint64_t site = 0;
        site |= kmer.strand;
        site <<= 31;
        site |= kmer.seg_start_site;
        site <<=32;
        site |= (uint32_t)std::stol(kmer.seg_idx); //TODO seg_idx 需要转化成数字
        compressed_kmer->site = site;
    }

    void Coder::EncodeKmer(Kmer &kmer, CompressedKmer* compressed_kmer){
        for (int i = 0; i < param.kmer_width; i++){
            for (int j = i * 32; j < (i + 1) * 32 && j < param.k; j++){
                compressed_kmer->kmer[i] <<= 2;
                compressed_kmer->kmer[i] |= ENCODE_MER_TABLE[kmer.sequence[j]];
            }
        }
    }

    Kmer Coder::Decode(CompressedKmer& compressed_kmer) {
        Kmer kmer;
        kmer.strand = compressed_kmer.site >> 63;
        kmer.seg_start_site = (compressed_kmer.site >> 32) & (((uint64_t )1 << 31) - 1);
        kmer.seg_idx = std::to_string(compressed_kmer.site & (((uint64_t )1 << 32) - 1));
        kmer.sequence = DecodeKmer(compressed_kmer.kmer);
        return kmer;
    }

    std::string Coder::DecodeKmer(uint64_t* compressed_seq) {
        std::string str_kmer;
        for (int i = param.kmer_width - 1; i >= 0; i--) {
            for (int j = ((i == param.kmer_width - 1) ? param.k-param.kmer_width*32 + 32: 32); j > 0; j--){
                str_kmer.push_back(DECODE_MER_TABLE[compressed_seq[i] & 3]);
                compressed_seq[i] >>= 2;
            }
        }
        std::reverse(str_kmer.begin(), str_kmer.end());
        return str_kmer;
    }
}

//int main()
//{
//    gjfish::Kmer kmer;
//    gjfish::Param param;
//    param.k = 52;
//    param.kmer_width = 2;
//    kmer.strand = 1;
//    kmer.sequence = "AGCCCCCCCCGGGGGGGGTTTTTCCGAGCCCCCCCCGGGGGGGGTTTTTCCG";
//    kmer.seg_idx = "4294967295";
//    kmer.seg_start_site = 0x7FFFFFFF;
//    gjfish::Coder coder(param);
//    gjfish::CompressedKmer* compressed_kmer = coder.Encode(kmer);
//    gjfish::Kmer decoded_kmer = coder.Decode(*compressed_kmer);
//    std::cout << decoded_kmer.strand << "| " << decoded_kmer.seg_start_site << "| " << decoded_kmer.seg_idx << "| "<< decoded_kmer.sequence << std::endl;
//    std::cout << sizeof(gjfish::CompressedKmer) << std::endl;
//    return 0;
//}