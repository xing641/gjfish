//
// Created by user1 on 2021/12/20.
//
#include "gfa_reader.h"
#include "kmer_counter.h"
#include "kmer_coder.h"
#include "lockfree_hash_map.h"
#include "mem_allocator.h"
#include <thread>
#include <chrono>


int main(int argc, char **argv)
{
    gjfish::Param param; // 从外部输入
    param.k = atoi(argv[5]);
    param.kmer_width = param.k / 32 + ((param.k % 32 == 0)? 0 : 1);
    param.kmer_char = param.k / 8 + ((param.k % 8 == 0)? 0 : 1);
    param.threads_count = atoi(argv[2]);
    param.hash_function = atoi(argv[3]);
    param.mem_size = atoi(argv[4]);
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
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < param.threads_count; i++) {
        th[i] = std::thread(gjfish::KmerCountWork, std::ref(counter), i);
    }

    for (int i = 0; i < param.threads_count; i++) {
        th[i].join();
    }
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "线程" << param.threads_count 
              << " 哈希函数：" << param.hash_function
              << " K值：" << param.k
              << " 总共花费时间" << double(duration.count()) * std::chrono::microseconds::period::num /
                                          std::chrono::microseconds::period::den << "s" << std::endl;
    // counter->ExportHashTable();
    // counter->ImportHashTable("/");
    // delete counter;
    // delete reader;

    return 0;
}

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