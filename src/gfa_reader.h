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
        uint32_t segIdx;
        std::string seg;
        std::string SN;
        uint32_t SO;
        uint32_t SR;
    };
    struct Line{
        uint32_t leftSegIdx;
        uint32_t rightSegIdx;
    };
    struct SuperSeg{
        std::string superSeg;
        std::string leftSeg;
        std::string rightSeg;
    };

    class GFAReader{
    public:
        std::string path;
        uint64_t size;
        std::vector<Segment> segments;
        GFAReader(std::string path);
        void Start();

        void ReadSeg(std::string primitiveSeg);
        Segment HandlePrimitiveSeg(std::string primitiveSeg);

        void ReadLine(std::string primitiveLine);
        void HandlePrimitiveLine();
        void GenerateSuperSeg();
    };


}

#endif //SRC_STABLE_GFA_READER_H
