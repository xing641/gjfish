//
// Created by user1 on 2021/9/28.
//

#ifndef SRC_STABLE_GFA_READER_H
#define SRC_STABLE_GFA_READER_H

#include <string>
#include <iostream>
#include <vector>
namespace gjfish{
    struct Segment{
        uint32_t seg_idx;
        uint64_t seg;
        std::string SN;
        uint32_t SO;
        uint32_t SR;
    };
    struct Line{
        uint32_t seg1_idx;
        uint32_t seg2_idx;
    };
    class GFAReader{
    public:
        std::string path;
        uint64_t size;
        std::vector<Segment> seg;
        GFAReader(std::string path);
        void ReadGraph();

        void ReadSeg();
        void HandlePrimitiveSeg();

        void ReadLine();
        void HandlePrimitiveLine();
        void GenerateSuperSeg();
    };



}

#endif //SRC_STABLE_GFA_READER_H
