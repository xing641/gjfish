//
// Created by user1 on 2021/11/15.
//

#ifndef SRC_STABLE_KMER_COUNTER_H
#define SRC_STABLE_KMER_COUNTER_H

#include "gfa_reader.h"
#include "lockfree_hash_map.h"
#include "mem_allocator.h"
#include "buffer_queue.h"
#include <fstream>
#include <ctime>

namespace gjfish {

    class Coder;
    class GFAReader;
    class LockFreeHashTable;
    class MemAllocator;
    class CompressedKmer;
    // strand 0表示负，1表示正
    struct Kmer {
        std::string sequence;
        std::string seg_idx;
        uint32_t seg_start_site;
        uint32_t strand;
    };
    class KmerCounter {
    public:
        GFAReader* gfa_reader;
        LockFreeHashTable* ht;
        MemAllocator* ma;
        Coder* coder;

        BufferQueue<Segment> *seg_buffer_queue;
        BufferQueue<SuperSeg> *super_seg_buffer_queue;
        void init_seg_buffer_queue();
        void init_super_seg_buffer_queue();


        explicit KmerCounter(GFAReader* gfa_reader);
        // 计算kmer
        void StartCount(bool is_save_site, std::ofstream &kmer_site_out_file, int n) const;

        // 1. 初始化哈希表
        void InitialHashTable();

        /*
         * Q1: 2和3能不能同时进行？
         *
         * A1： 肯定可以！
         * Q2： 当2或3其中一个停止对hashtable的统计有没有影响？
         * A2： 肯定不能有！
         *
         * */
        // 2. 从segment中生成kmer并计算
        void CountKmerFromSeg(int n, std::ofstream &kmer_site_out_file) const;
        // 3. 从superseg中生成kmer并计算
        void CountKmerFromSuperSeg(int n, std::ofstream &kmer_site_out_file) const;
        [[nodiscard]] std::vector<Kmer> ProduceKmerFromSuperSeg(SuperSeg ss) const;
        int FindKmerStart(int n, std::string &seq) const;

        // TODO
        void ExportHashTable();
        void ImportHashTable(std::string file_path) const;

        ~KmerCounter();

        // utils
        std::string ReverseComplement(std::string &sequence);

    };

    void KmerCountWork(KmerCounter *kc, int n);
}
#endif //SRC_STABLE_KMER_COUNTER_H
