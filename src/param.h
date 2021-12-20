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
        uint32_t k;
        uint32_t kmer_width;
        std::string file_dir;
        // ...
    };
}

#endif //SRC_STABLE_PARAM_H
