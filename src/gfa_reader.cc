//
// Created by user1 on 2021/9/28.
//

#include "src/gfa_reader.h"
#include <fstream>


namespace gjfish{
    GFAReader::GFAReader(std::string path) : path(path){};

    void GFAReader::Start() {
        std::fstream file;

        file.open(path);
        std::string gfaFileLine;
        // TODO log.out()
        if (!file.is_open()){
            std::cout << "file open fail!" << std::endl;
            return ;
        }

        while (getline(file, gfaFileLine)){
            if (gfaFileLine[0] == 'S'){
                std::cout << gfaFileLine << std::endl;
                // ReadSeg(gfaFileLine);
            } else if (gfaFileLine[0] == 'L'){
                std::cout << gfaFileLine << std::endl;
                // ReadLine(gfaFileLine);
            }
        }
    }
    // S	s105809	AAGGTGCCCGGCACG	LN:i:15	SN:Z:CHM13#LBHZ01000344.1	SO:i:2026584	SR:i:3	cf:f:0.2
    // L       MTh4001 +       MTh4502 +       0M      SR:i:0

    void GFAReader::ReadSeg(std::string primitiveSeg) {
        Segment seg = HandlePrimitiveSeg(primitiveSeg);
        // segments.push_back(seg);
    }

    Segment GFAReader::HandlePrimitiveSeg(std::string primitiveSeg) {
    }

    void GFAReader::ReadLine(std::string primitiveLine) {

    }
    void GFAReader::HandlePrimitiveLine() {}
    void GFAReader::GenerateSuperSeg() {}
}
int main()
{
    gjfish::GFAReader *reader = new gjfish::GFAReader("../data/MT.gfa");
    reader->Start();
    return 0;
}

