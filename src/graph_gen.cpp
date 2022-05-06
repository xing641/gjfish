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

std::string RC(const std::string &sequence) {
    int n = sequence.size();
    std::string res(n, 'x');
    for (int i = n - 1; i >= 0; i--) {
        res[n - i - 1] = gjfish::REVERSE_TABLE[sequence[i]];
    }
    return res;
}

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
    // std::ofstream f;
    // f.open("res.txt");


    // for (auto i: reader->segments){
    //     if (i.second.seq.length() > 0){
    //         f << ">\n";
    //         f << i.second.seq << "\n";
    //     }

    // }

    // for (auto i: reader->superSegments){
    //     f << ">\n";
    //     std::string str = "";
    //     for (int x = 0; x < i.size(); x++){
            
    //         if (i[x].strand == "-") {
    //             i[x].seq = RC(i[x].seq);
    //         }
    //         if (i[x].seq.length() >= param.k) {
    //             if (x == 0){
    //                 str += i[x].seq.substr(i[x].seq.length() - param.k + 1, param.k - 1);
    //             }
    //             else if (x == i.size() - 1){
    //                 str += i[x].seq.substr(0, param.k - 1);
    //             }
    //         } else {
    //             str += i[x].seq;
    //         }
    //     }
    //     f << str << '\n';
    // }
    return 0;
}