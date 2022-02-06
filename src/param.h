//
// Created by user1 on 2021/12/20.
//

#ifndef SRC_STABLE_PARAM_H
#define SRC_STABLE_PARAM_H
#include <cstdint>
#include <string>

namespace gjfish {
    class Param {
    public:
        uint32_t k = 16;
        uint32_t kmer_width = 1;
        std::string result_site_dir;
        std::string kmer_site_out_dir;
        std::string kmer_site_out_file_name = "kmer_site_out_file.tmp";
        uint32_t threads_count = 10;
        uint32_t mem_size = 100000;
        // ...
    };
}

#endif //SRC_STABLE_PARAM_H
