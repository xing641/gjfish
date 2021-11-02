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
        std::string seq;
        std::string SN;
        uint32_t SO;
        uint32_t SR;
    };
    struct Line{
        uint32_t leftSegIdx;
        bool leftStrand;
        uint32_t rightSegIdx;
        bool rightStrand;
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

        ~GFAReader();
    };

    const unsigned char REVERSE_TABLE[128] = {
            0, 1, 2, 3,            4, 5, 6, 7,          8, 9, 10, 11,        12, 13, 14, 15,
            16, 17, 18, 19,        20, 21, 22, 23,      24, 25, 26, 27,      28, 29, 30, 31,
            32, 33, 34, 35,        36, 37, 38, 39,      40, 41, 42, 43,      44, 45, 46, 47,
            48, 49, 50, 51,        52, 53, 54, 55,      56, 57, 58, 59,      60, 61, 62, 63,
            64, 84, 66, 71,        68, 69, 70, 67,      72, 73, 74, 75,      76, 77, 78, 79,
            80, 81, 82, 83,        84, 85, 86, 87,      88, 89, 90, 91,      92, 93, 94, 95,
            96, 116, 98, 103,      100, 101, 102, 99,   104, 105, 106, 107,  108, 109, 110, 111,
            112, 113, 114, 115,    97, 97, 118, 119,    120, 121, 122, 123,  124, 125, 126, 127,
    };

    std::vector<std::string> ExtractStringInfo(std::string tmp, char delim);
    std::string ReverseComplement(std::string sequence);
}

#endif //SRC_STABLE_GFA_READER_H
