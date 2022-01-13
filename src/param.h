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
        std::string file_dir;
        uint32_t threads_count = 1;
        // ...
    };
}

#endif //SRC_STABLE_PARAM_H
