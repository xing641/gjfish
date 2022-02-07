//
// Created by user1 on 2021/11/15.
//
#include "kmer_counter.h"

namespace gjfish {
    // compressedkmer
    CompressedKmer::CompressedKmer(uint64_t width) {
        //TODO 需要一个malloc来进行分配内存
        kmer = (uint64_t* )malloc(sizeof(uint64_t) * width);
        for (int i = 0; i < width; i++) {
            kmer[i] = 0;
        }
    }

    CompressedKmer::~CompressedKmer(){
        delete kmer;
    }

    void KmerCounter::init_seg_buffer_queue() {
        seg_buffer_queue = new BufferQueue<Segment>;
        for (auto it: gfa_reader->segments){
            seg_buffer_queue->Push(it.second);
        }
    }

    void KmerCounter::init_super_seg_buffer_queue() {
        super_seg_buffer_queue = new BufferQueue<SuperSeg>;
        for (auto it: gfa_reader->superSegments) {
            super_seg_buffer_queue->Push(it);
        }
    }

    // kmercounter
    KmerCounter::KmerCounter(GFAReader* gfa_reader) : gfa_reader(gfa_reader){
        init_seg_buffer_queue();
        init_super_seg_buffer_queue();
        coder = new Coder(gfa_reader->param);
        ma = gfa_reader->ma;
        ht = new gjfish::LockFreeHashTable(ma, gfa_reader->param);
    }

    void KmerCounter::StartCount(bool is_save_site, std::ofstream &kmer_site_out_file, int n){
        if (is_save_site){
            //TODO 初始化哈希表 InitialHashTable();
            //TODO 这里需要并发

            CountKmerFromSeg(n, kmer_site_out_file);
            CountKmerFromSuperSeg(n, kmer_site_out_file);
        } else {
            // TODO
        }
    }

    // TODO kmer需要考虑方向
    void KmerCounter::CountKmerFromSeg(int n, std::ofstream &kmer_site_out_file) {
        Segment seg;
        while(seg_buffer_queue->Pop(seg, false)) {
            if (seg.seq.size() >= gfa_reader->param.k) {
                for (int i = seg.seq.size() - gfa_reader->param.k; i >= 0; i--) {
                    Kmer kmer;
                    kmer.sequence = seg.seq.substr(i, gfa_reader->param.k);
                    kmer.seg_idx = seg.segIdx;
                    kmer.seg_start_site = i;
                    // ht->add_kmer(n, coder->Encode(kmer));
                    kmer_site_out_file.write((char*)&(coder->Encode(kmer)->site), sizeof(uint64_t));
                }
            }
        }
    }


    void KmerCounter::CountKmerFromSuperSeg(int n, std::ofstream &kmer_site_out_file) {
        SuperSeg ss;
        while(super_seg_buffer_queue->Pop(ss, false)) {
            std::vector<Kmer> kmers = ProduceKmerFromSuperSeg(ss);
            for (auto kmer: kmers) {
                // ht->add_kmer(n, coder->Encode(kmer));
                kmer_site_out_file.write((char*)&(coder->Encode(kmer)->site), sizeof(uint64_t));
            }
        }
    }

    std::vector<Kmer> KmerCounter::ProduceKmerFromSuperSeg(SuperSeg ss) {
        int len = 0;
        std::string seq;
        uint32_t start = (ss[0].seq.size() >= gfa_reader->param.k) ? (ss[0].seq.size() - gfa_reader->param.k) : 0;
        for (int i = 0; i < ss.size(); i++) {
            len += ss[i].seq.size();
            seq += ss[i].seq;
        }
        uint32_t end = (ss[ss.size() - 1].seq.size() >= gfa_reader->param.k) ? len - 1 - (ss[ss.size() - 1].seq.size() - gfa_reader->param.k) : len;
        std::vector<Kmer> kmers;
        Kmer now_kmer;
        uint32_t now_seg_site = start;
        uint32_t now_seg = 0;
        for (uint32_t now = start; now + gfa_reader->param.k < end; now++) {
            now_kmer.sequence = seq.substr(now, gfa_reader->param.k);
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

    KmerCounter::~KmerCounter(){
        delete gfa_reader;
        delete ht;
        delete coder;
    }

    void KmerCountWork(KmerCounter *kc, int n){
        std::ofstream kmer_site_out_file;
        kmer_site_out_file.open("./" + kc->gfa_reader->param.kmer_site_out_file_name + "_" + std::to_string(n), std::ios::out|std::ios::binary);
        kc->StartCount(true, kmer_site_out_file, n);
    }
}
