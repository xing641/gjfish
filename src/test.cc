//
// Created by user1 on 2021/12/20.
//
#include "gfa_reader.h"
#include "kmer_counter.h"
#include "kmer_coder.h"
#include "lockfree_hash_map.h"
#include "mem_allocator.h"


int main()
{
    gjfish::Param param; // 从外部输入
    param.kmer_width = 1;
    param.k = 31;
    param.threads_count = 8;
    param.mem_size = 8000000000;
    param.result_site_dir = "result";

    auto *reader = new gjfish::GFAReader("../test/GRCh38-20-0.10b.gfa", param);

    reader->Start();
    reader->GenerateSuperSeg();

    auto *counter = new gjfish::KmerCounter(reader);

    // 数据输入：两个buffer_queue 一个是segment， 一个是supersegment
    // 初始化：线程、coder、hash_table
    // 输出：hash_table
    counter->StartCount();

    return 0;
}