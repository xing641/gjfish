//
// Created by user1 on 2021/11/15.
//

#ifndef SRC_STABLE_KMER_PRODUCER_H
#define SRC_STABLE_KMER_PRODUCER_H

#include "gfa_reader.h"

namespace gjfish {
    struct Kmer {
        std::string sequence;
        std::string seg_idx;
        uint32_t seg_start_site;
    };
    class KmerCounter {
    public:
        GFAReader *gfa_reader;
        // 计算kmer
        void StartCount(GFAReader& reader);

        // 1. 初始化哈希表
        void InitialHashTable();

        /*
         * Q1: 2和3能不能同时进行？
         * A1： 肯定可以！
         * Q2： 当2或3其中一个停止对hashtable的统计有没有影响？
         * A2： 肯定不能有！
         *
         * */
        // 2. 从segment中生成kmer并计算
        void CountKmerFromSeg();
        // 3. 从superseg中生成kmer并计算
        void CountKmerFromSuperSeg();
        std::vector<Kmer> ProduceKmerFromSuperSeg(SuperSeg ss);

    };
}
#endif //SRC_STABLE_KMER_PRODUCER_H
