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
                ReadLine(gfaFileLine);
            }
        }
    }
    // S	s105809	AAGGTGCCCGGCACG	LN:i:15(长度)	SN:Z:CHM13#LBHZ01000344.1(名字）	SO:i:2026584	SR:i:3（	cf:f:0.2 (覆盖度）


    void GFAReader::ReadSeg(const std::string& primitiveSeg) {
        Segment seg = HandlePrimitiveSeg(primitiveSeg);
        segments[seg.segIdx] = seg;
    }

    Segment GFAReader::HandlePrimitiveSeg(const std::string& primitiveSeg) {
        Segment seg;

        char delim = '\t';
        std::istringstream iss(primitiveSeg);
        std::string tmp;
        int count = 0;
        while (std::getline(iss, tmp, delim)){
            count++;
            if (count == 2) {
                seg.segIdx = tmp;
            }
            else if (count == 3) {
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
    void GFAReader::ReadLine(const std::string& primitiveLine) {

        Line line = HandlePrimitiveLine(primitiveLine);
        lines.insert(std::make_pair(line.leftSeg, line.rightSeg));
    }
    Line GFAReader::HandlePrimitiveLine(const std::string& primitiveLine) {

        char delim = '\t';
        std::vector<std::string> splitLine = ExtractStringInfo(primitiveLine, delim);

        Line line;
        line.leftSeg = splitLine[1] + splitLine[2];
        line.rightSeg = splitLine[3] + splitLine[4];

        return line;
    }

    void GFAReader::GenerateSuperSeg() {
        for (auto& it : segments) {
            if (!it.second.isVisited[0]) {
                std::string startSeg = it.second.segIdx + "-";
                SuperSeg sseg = DFSLines(startSeg);
                if (!sseg.empty())
                    superSegments.push_back(sseg);
                it.second.isVisited[0] = true;
            }
            if (!it.second.isVisited[1]) {
                std::string startSeg = it.second.segIdx + "+";
                SuperSeg sseg = DFSLines(startSeg);
                if (!sseg.empty())
                    superSegments.push_back(sseg);
                it.second.isVisited[1] = true;
            }
        }
    }

    SuperSeg GFAReader::DFSLines(const std::string& seg) {




    }
    // L	s22457	+	s93424	+	0M	SR:i:2	L1:i:43348	L2:i:313	cf:f:0.65 (1, 0.65)
    // L	s148609	+	s130193	+	0M	SR:i:21	L1:i:665	L2:i:283	cf:f:0.15 (0.15, 0.2)
    // L	s148592	+	s85632	-	0M	SR:i:21	L1:i:1702	L2:i:1965	cf:f:0.05 (0.05  0.95)
    std::vector<std::string> ExtractStringInfo(const std::string& str, char delim) {
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

}
int main()
{
    gjfish::GFAReader *reader = new gjfish::GFAReader("../test/MT.gfa");
    reader->Start();
    return 0;
}

