//
// Created by user1 on 2021/9/28.
//

#ifndef SRC_STABLE_GFA_READER_H
#define SRC_STABLE_GFA_READER_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
namespace gjfish{

    struct Segment{
        std::string segIdx;
        std::string seq;
        std::string SN;
        uint32_t SO;
        uint32_t SR;
        bool isVisited[2] = {false, false}; // 0表示负，1表示正
    };
    // TODO 如何将leftsegIdx编码？不编码，字典树直接做~
    struct Line{
        std::string leftSeg;
        std::string rightSeg;
    };
    struct SuperSegFragment{
        std::string segIdx;
        std::string seq;
        std::string SN;
        std::string SR;
        std::string SO;
        std::string strand;
    };

    typedef std::vector<SuperSegFragment> SuperSeg;
    class GFAReader{
    public:
        std::string path;
        uint64_t size;

        std::vector<SuperSeg> superSegments;
        std::multimap<std::string, std::string> lines;
        std::unordered_map<std::string, Segment> segments;

        GFAReader(std::string path);
        void Start();

        void ReadSeg(const std::string& primitiveSeg);
        static Segment HandlePrimitiveSeg(const std::string& primitiveSeg);

        void ReadLine(const std::string& primitiveLine);
        static Line HandlePrimitiveLine(const std::string& primitiveLine);

        void GenerateSuperSeg();
        void SaveSuperSeg(std::string savePath);
        void StartDFSLines(const std::string &segIdx, const int& strand);
        void DFSLines(SuperSeg& startSseg);
        SuperSegFragment ExtractSsegFragment(std::string segSignIdx);
        bool HasNextSeg(SuperSegFragment ssf);
        bool IsNewSS(SuperSeg ss);
        void VisitedSeg(SuperSeg ss);
        ~GFAReader();
    };

    const char REVERSE_TABLE[128] = {
            0, 1, 2, 3,            4, 5, 6, 7,          8, 9, 10, 11,        12, 13, 14, 15,
            16, 17, 18, 19,        20, 21, 22, 23,      24, 25, 26, 27,      28, 29, 30, 31,
            32, 33, 34, 35,        36, 37, 38, 39,      40, 41, 42, 43,      44, 45, 46, 47,
            48, 49, 50, 51,        52, 53, 54, 55,      56, 57, 58, 59,      60, 61, 62, 63,
            64, 84, 66, 71,        68, 69, 70, 67,      72, 73, 74, 75,      76, 77, 78, 79,
            80, 81, 82, 83,        84, 85, 86, 87,      88, 89, 90, 91,      92, 93, 94, 95,
            96, 116, 98, 103,      100, 101, 102, 99,   104, 105, 106, 107,  108, 109, 110, 111,
            112, 113, 114, 115,    97, 97, 118, 119,    120, 121, 122, 123,  124, 125, 126, 127,
    };
    uint32_t k = 31;

    const char STRAND_CHAR[2] = {'-', '+'};

    std::vector<std::string> ExtractStringInfo(const std::string& tmp, char delim);
    std::string ReverseComplement(std::string sequence);
    bool judgeStrand(std::string str);
    std::string SsfToString(SuperSegFragment ssf);
}

#endif //SRC_STABLE_GFA_READER_H
