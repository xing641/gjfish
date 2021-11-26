//
// Created by user1 on 2021/11/15.
//
#include "kmer_counter.h"

namespace gjfish {
    void KmerCounter::StartCount(GFAReader& reader){
        gfa_reader = &reader;

        //TODO 初始化哈希表 InitialHashTable();
        //TODO 这里需要并发
        CountKmerFromSuperSeg();
        CountKmerFromSeg();
    }

    // TODO kmer需要考虑方向
    void KmerCounter::CountKmerFromSeg() {
        for (auto it: gfa_reader->segments){
            if (it.second.seq.size() >= k) {
                for (int i = it.second.seq.size() - k; i >= 0; i--){
                    Kmer kmer;
                    kmer.sequence = it.second.seq.substr(i, k);
                    kmer.seg_idx = it.second.segIdx;
                    kmer.seg_start_site = i;
                    // HashCount(kmer);
                }
            }
        }
    }

    void KmerCounter::CountKmerFromSuperSeg() {
        for (auto it: gfa_reader->superSegments){
            std::vector<Kmer> kmers = ProduceKmerFromSuperSeg(it);
            for (auto kmer: kmers) {
                // HashCount(kmer);
            }
        }
    }

    std::vector<Kmer> KmerCounter::ProduceKmerFromSuperSeg(SuperSeg ss) {
        int len = 0;
        std::string seq;
        uint32_t start = (ss[0].seq.size() >= k) ? (ss[0].seq.size() - k) : 0;
        for (int i = 0; i < ss.size(); i++) {
            len += ss[i].seq.size();
            seq += ss[i].seq;
        }
        uint32_t end = (ss[ss.size() - 1].seq.size() >= k) ? len - 1 - (ss[ss.size() - 1].seq.size() - k) : len;
        std::vector<Kmer> kmers;
        Kmer now_kmer;
        uint32_t now_seg_site = start;
        uint32_t now_seg = 0;
        for (uint32_t now = start; now + k < end; now++) {
            now_kmer.sequence = seq.substr(now, k);
            now_kmer.seg_start_site = now_seg_site;
            now_kmer.seg_idx = ss[now_seg].segIdx;
            kmers.push_back(now_kmer);
            now_seg_site++;
            if (now_seg_site == ss[now_seg].seq.size()) {
                now_seg_site = 0;
                now_seg++;
            }
        }
        return kmers;
    }

}
