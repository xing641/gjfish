//
// Created by user1 on 2021/12/20.
//
#include "gfa_reader.h"
#include "kmer_counter.h"
#include "kmer_coder.h"
#include "lockfree_hash_map.h"
#include "mem_allocator.h"
#include <thread>


int main(int argc, char **argv)
{
    gjfish::Param param; // 从外部输入
    param.kmer_width = 1;
    param.k = 31;
    param.threads_count = 16;
    param.mem_size = 16000000000;
    param.result_site_dir = "kmer_site_out_file_name.bin";
    param.input_file = argv[1];
    // param.input_file = "../test/KmerCounter_test.gfa";

    auto *reader = new gjfish::GFAReader(param.input_file, param);

    reader->Start();
    reader->GenerateSuperSeg();

    auto counter = new gjfish::KmerCounter(reader);

    // 数据输入：两个buffer_queue 一个是segment， 一个是supersegment
    // 初始化：线程、coder、hash_table
    // 输出：hash_table
    std::thread th[param.threads_count];
    for (int i = 0; i < param.threads_count; i++) {
        th[i] = std::thread(gjfish::KmerCountWork, std::ref(counter), i);
    }

    for (int i = 0; i < param.threads_count; i++) {
        th[i].join();
    }

    // counter->ExportHashTable();
    // counter->ImportHashTable("/");

    return 0;
}
//
//int main(int argc, char **argv)
//{
//    gjfish::Param param; // 从外部输入
//    param.kmer_width = 1;
//    param.k = 4;
//    param.threads_count = 1;
//    param.mem_size = 2000;
//    param.result_site_dir = "kmer_site_out_file_name.bin";
//    // param.input_file = argv[1];
//    param.input_file = "../test/KmerCounter_test.gfa";
//
//    auto *reader = new gjfish::GFAReader(param.input_file, param);
//
//    reader->Start();
//    reader->GenerateSuperSeg();
//
//    auto counter = new gjfish::KmerCounter(reader);
//
//    // 数据输入：两个buffer_queue 一个是segment， 一个是supersegment
//    // 初始化：线程、coder、hash_table
//    // 输出：hash_table
//    std::thread th[param.threads_count];
//    for (int i = 0; i < param.threads_count; i++) {
//        th[i] = std::thread(gjfish::KmerCountWork, std::ref(counter), i);
//    }
//
//    for (int i = 0; i < param.threads_count; i++) {
//        th[i].join();
//    }
//
//    // counter->ExportHashTable();
//    // counter->ImportHashTable("/");
//
//    return 0;
//}