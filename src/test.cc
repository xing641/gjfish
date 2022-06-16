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
    param.mem_size = atol(argv[4]) * 1000000000;
    // std::cout << param.mem_size << std::endl;
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
        th[i] = std::thread(gjfish::KmerCountWork, counter, i);
    }

    for (int i = 0; i < param.threads_count; i++) {
        th[i].join();
    }
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "线程" << param.threads_count 
              << " 哈希函数：" << param.hash_function
              << " K值：" << param.k
              << " 总共花费时间\n" << double(duration.count()) * std::chrono::microseconds::period::num /
                                          std::chrono::microseconds::period::den << std::endl;

    uint64_t total_collsion = 0;
    uint64_t total_node = 0;
    for (int i = 0; i < param.threads_count; i++){
        total_collsion += counter->total_collsion_cnt[i];
        total_node += counter->new_node_cnt[i];
    }

    uint64_t max = 0;
    uint64_t total_kmer = 0;
    uint64_t unique_kmer = 0;
    int histogram[1000] = {0};
    for (uint64_t i = 0; i < param.threads_count; i++){
        uint64_t start = counter->ht->blocks[i]->start_id;
        // uint64_t end = counter->ht->blocks[i]->next_id;            
        // std::cout << "start: " << start << std::endl;
        // std::cout << "end: " << end << std::endl;
        uint64_t end = counter->ht->blocks[i]->end_id;

        for (uint64_t j = start; j < end; j++) {
            gjfish::Node* node = counter->ht->get_node(j);
            // if (j < start + 100){
            //     std::cout << j << ". ";
            //     std::cout << counter->coder->DecodeKmer(node->kmer) << " : ";
            //     std::cout << node->cnt << std::endl;
            // }
            if (node->cnt < 1000 && node->cnt > 0) {
                histogram[node->cnt]++;
            }
            if (node->cnt > max) {
                max = node->cnt;
            }
            total_kmer++;
            if(node->cnt == 1)unique_kmer++;
        }
    }
    for (int i = 1; i < 1000; i++) {
        std::cout << i << " : " << histogram[i] << std::endl;
    }

    std::cout << "最大k-mer个数: " << max << std::endl;
    std::cout << "总k-mer个数: " << total_kmer << std::endl;
    std::cout << "唯一k-mer个数: " << unique_kmer << std::endl;

    // std::cout << "冲突次数: " << total_collsion << " 总节点数： " << total_node << std::endl;
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