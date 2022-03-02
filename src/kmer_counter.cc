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
        delete[] kmer;
        delete this;
    }

    void KmerCounter::init_seg_buffer_queue() {
        seg_buffer_queue = new BufferQueue<Segment>[gfa_reader->param.threads_count];
        uint64_t i = 0;
        for (const auto& it: gfa_reader->segments){
            seg_buffer_queue[i % gfa_reader->param.threads_count].Push(it.second);
            i++;
        }
    }

    void KmerCounter::init_super_seg_buffer_queue() {
        super_seg_buffer_queue = new BufferQueue<SuperSeg>;
        for (const auto& it: gfa_reader->superSegments) {
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

    void KmerCounter::StartCount(bool is_save_site, std::ofstream &kmer_site_out_file, int n) const{
        if (is_save_site){
            //TODO 初始化哈希表 InitialHashTable();
            //TODO 这里需要并发

            CountKmerFromSeg(n, kmer_site_out_file);
            CountKmerFromSuperSeg(n, kmer_site_out_file);
        } else {
            // TODO
        }
    }

    void KmerCounter::CountKmerFromSeg(int n, std::ofstream &kmer_site_out_file) const {
        Segment seg;
        while(seg_buffer_queue[n].Pop(seg, false)) {
            if (seg.seq.size() >= gfa_reader->param.k) {
                for (int i = FindKmerStart(0, seg.seq); i <= seg.seq.size() - gfa_reader->param.k; i++) {
                    if (ENCODE_MER_TABLE[seg.seq[i + gfa_reader->param.k - 1]] == 4) {
                        i = FindKmerStart(i + gfa_reader->param.k + 1, seg.seq) - 1;
                        continue;
                    }
                    Kmer kmer;
                    kmer.sequence = seg.seq.substr(i, gfa_reader->param.k);
                    kmer.seg_idx = seg.segIdx;
                    kmer.seg_start_site = i;
                    ht->add_kmer(n, coder->Encode(kmer));
                    // kmer_site_out_file.write((char*)&(coder->Encode(kmer)->site), sizeof(uint64_t));
                }
            }
        }
        for (int queue_num = 0; queue_num < gfa_reader->param.threads_count; queue_num++){
            while(seg_buffer_queue[queue_num].Pop(seg, false)) {
                if (seg.seq.size() >= gfa_reader->param.k) {
                    for (int i = FindKmerStart(0, seg.seq); i <= seg.seq.size() - gfa_reader->param.k; i++) {
                        if (ENCODE_MER_TABLE[seg.seq[i + gfa_reader->param.k - 1]] == 4) {
                            i = FindKmerStart(i + gfa_reader->param.k + 1, seg.seq) - 1;
                            continue;
                        }
                        Kmer kmer;
                        kmer.sequence = seg.seq.substr(i, gfa_reader->param.k);
                        kmer.seg_idx = seg.segIdx;
                        kmer.seg_start_site = i;
                        ht->add_kmer(n, coder->Encode(kmer));
                        // kmer_site_out_file.write((char*)&(coder->Encode(kmer)->site), sizeof(uint64_t));
                    }
                }
            }
        }
    }

    int KmerCounter::FindKmerStart(int n, std::string &seq) const{
        int start = n, now = n;
        while(true){
            if (now > seq.size() || now - start + 1 >= gfa_reader->param.k){
                break;
            }
            if (ENCODE_MER_TABLE[seq[now]] != 4){
                now ++;
            } else {
                now ++;
                start = now;
            }
        }
        return start;
    }

    void KmerCounter::CountKmerFromSuperSeg(int n, std::ofstream &kmer_site_out_file) const {
        SuperSeg ss;
        while(super_seg_buffer_queue->Pop(ss, false)) {
            int len = 0;
            std::string seq;
            uint32_t start = (ss[0].seq.size() >= gfa_reader->param.k) ? (ss[0].seq.size() - gfa_reader->param.k + 1) : 0;
            for (auto & s : ss) {
                len += s.seq.size();
                seq += (s.strand == "+")? s.seq : ReverseComplement(s.seq);
            }
            uint32_t end = (ss[ss.size() - 1].seq.size() >= gfa_reader->param.k) ? len - 1 - (ss[ss.size() - 1].seq.size() - gfa_reader->param.k) : len;
            uint32_t now_seg_site = start;
            uint32_t now_seg = 0;
            for (uint32_t now = FindKmerStart(start, seq); now + gfa_reader->param.k < end; now++) {
                if (ENCODE_MER_TABLE[seq[now + gfa_reader->param.k - 1]] == 4) {
                    now = FindKmerStart(now + gfa_reader->param.k + 1, seq) - 1;
                    continue;
                }
                Kmer now_kmer;
                now_kmer.sequence = seq.substr(now, gfa_reader->param.k);
                now_kmer.seg_start_site = now_seg_site;
                now_kmer.seg_idx = ss[now_seg].segIdx;
                now_seg_site++;
                if (now_seg_site == ss[now_seg].seq.size()) {
                    now_seg_site = 0;
                    now_seg++;
                }
                ht->add_kmer(n, coder->Encode(now_kmer));
                // kmer_site_out_file.write((char*)&(coder->Encode(now_kmer)->site), sizeof(uint64_t));
            }
        }
    }

    std::vector<Kmer> KmerCounter::ProduceKmerFromSuperSeg(SuperSeg ss) const {
        int len = 0;
        std::string seq;
        uint32_t start = (ss[0].seq.size() >= gfa_reader->param.k) ? (ss[0].seq.size() - gfa_reader->param.k + 1) : 0;
        for (auto & s : ss) {
            len += s.seq.size();
            seq += (s.strand == "+")? s.seq : ReverseComplement(s.seq);
        }
        uint32_t end = (ss[ss.size() - 1].seq.size() >= gfa_reader->param.k) ? len - 1 - (ss[ss.size() - 1].seq.size() - gfa_reader->param.k) : len;
        std::vector<Kmer> kmers;
        Kmer now_kmer;
        uint32_t now_seg_site = start;
        uint32_t now_seg = 0;
        for (uint32_t now = FindKmerStart(start, seq); now + gfa_reader->param.k < end; now++) {
            if (ENCODE_MER_TABLE[seq[now + gfa_reader->param.k - 1]] == 4) {
                now = FindKmerStart(now + gfa_reader->param.k + 1, seq) - 1;
                continue;
            }
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
        // kmer_site_out_file.open("./" + kc->gfa_reader->param.kmer_site_out_file_name + "_" + std::to_string(n), std::ios::out|std::ios::binary);
        kc->StartCount(true, kmer_site_out_file, n);
    }
}
