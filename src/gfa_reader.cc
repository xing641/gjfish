//
// Created by user1 on 2021/9/28.
//

#include "src/gfa_reader.h"
#include <fstream>
#include <sstream>


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
                ReadSeg(gfaFileLine);
            } else if (gfaFileLine[0] == 'L'){
                // ReadLine(gfaFileLine);
            }
        }
    }
    // S	s105809	AAGGTGCCCGGCACG	LN:i:15(长度)	SN:Z:CHM13#LBHZ01000344.1(名字）	SO:i:2026584	SR:i:3（	cf:f:0.2


    void GFAReader::ReadSeg(std::string primitiveSeg) {
        Segment seg = HandlePrimitiveSeg(primitiveSeg);
        segments.push_back(seg);
    }

    Segment GFAReader::HandlePrimitiveSeg(std::string primitiveSeg) {
        Segment seg;

        char delim = '\t';
        std::istringstream iss(primitiveSeg);
        std::string tmp;
        int count = 0;
        while (std::getline(iss, tmp, delim)){
            count++;
            if (count == 3) {
                seg.seq = tmp;
            } else if (count > 3) {
                std::vector<std::string> field = ExtractStringInfo(tmp, ':');
                if (field[0] == "SN") {
                    seg.SN = field[2];
                } else if (field[0] == "SO") {
                    seg.SO = std::stoi(field[2]);
                } else if (field[0] == "SR") {
                    seg.SR = std::stoi(field[2]);
                }
            }
        }
        return seg;
    }

    // L       MTh4001 +       MTh4502 +       0M      SR:i:0
    // L	s60647	-	s60646	+	0M(overlap)	SR:i:16	L1:i:1399(长度）	L2:i:350(长度）	cf:f:0.05
    void GFAReader::ReadLine(std::string primitiveLine) {

        Line line = HandlePrimitiveLine(primitiveLine);
        lines.push_back(line);
        // SuperSeg superSeg = GenerateSuperSeg(line);
        // superSegments.push_back(superSeg);
    }
    Line GFAReader::HandlePrimitiveLine(std::string primitiveLine) {

        char delim = '\t';
        std::vector<std::string> splitLine = ExtractStringInfo(primitiveLine, delim);

        Line line;
        line.leftSegIdx = splitLine[1];
        line.leftStrand = judgeStrand(splitLine[2]);
        line.rightSegIdx = splitLine[3];
        line.rightStrand = judgeStrand(splitLine[4]);

        return line;
    }

    void GFAReader::GenerateSuperSeg() {
        for (Line line : lines) {
            Segment startSeg = segments[line.rightSegIdx];
            if (!isVisited(startSeg)) {
                SuperSeg sseg = DFSLines(startSeg);
                if (sseg != NULL)
                    superSegments.push_back(sseg);
            }
        }
    }

    SuperSeg GFAReader::DFSLines(Segment seg) {



    }

    std::vector<std::string> ExtractStringInfo(std::string str, char delim) {
        std::istringstream iss(str);
        std::string tmp;
        std::vector<std::string> splitStr;
        while (std::getline(iss, tmp, delim)) {
            splitStr.push_back(tmp);
        }
        return splitStr;
    }

    std::string ReverseComplement(std::string sequence) {
        int n = sequence.size();
        std::string res(n, 'x');
        for (int i = n - 1; i >= 0; i--) {
            res[n - i - 1] = REVERSE_TABLE[sequence[i]];
        }
        return res;
    }

    bool judgeStrand(std::string str){
        if (str[0] == '+')
            return 1;
        else if (str[0] == '-')
            return 0;
        return 0;
    }

}
int main()
{
    gjfish::GFAReader *reader = new gjfish::GFAReader("../test/MT.gfa");
    reader->Start();
    return 0;
}

