//
// Created by user1 on 2021/9/28.
//

#include "src/gfa_reader.h"
#include <fstream>
#include <sstream>
#include <utility>


namespace gjfish{
    GFAReader::GFAReader(std::string path) : path(std::move(path)){}
    GFAReader::~GFAReader() = default;

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
                std::string startSeg = it.second.segIdx;
                StartDFSLines(startSeg, 0);
                it.second.isVisited[0] = true;
            }
            if (!it.second.isVisited[1]) {
                std::string startSeg = it.second.segIdx;
                StartDFSLines(startSeg, 1);
                it.second.isVisited[1] = true;
            }
        }
    }

    void GFAReader::StartDFSLines(const std::string& segIdx, const int& strand) {
        if (!segments[segIdx].isVisited[strand]) {
            SuperSeg sseg;
            SuperSegFragment ssegFrag = ExtractSsegFragment(segIdx + STRAND_CHAR[strand]);
            if (HasNextSeg(ssegFrag)){
                sseg.push_back(ssegFrag);
                DFSLines(sseg);
            }
        }
    }

    void GFAReader::DFSLines(SuperSeg &startSseg) {

        auto range = lines.equal_range(startSseg[startSseg.size() - 1].segIdx + startSseg[startSseg.size() - 1].strand);
        if (range.first == range.second){
            if (IsNewSS(startSseg)){
                superSegments.push_back(startSseg);
                VisitedSeg(startSseg);
            }
            return;
        }
        for (auto it = range.first; it != range.second; it++) {
            SuperSegFragment ssegFrag = ExtractSsegFragment(it->second);
            startSseg.push_back(ssegFrag);
            if (segments[ssegFrag.segIdx].isVisited[(ssegFrag.strand == "+")? 1 : 0] || ssegFrag.seq.size() >= k) {
                if (IsNewSS(startSseg)){
                    superSegments.push_back(startSseg);
                    VisitedSeg(startSseg);
                }
            } else {
                DFSLines(startSseg);
            }
            startSseg.pop_back();
        }
    }

    SuperSegFragment GFAReader::ExtractSsegFragment(std::string segSignIdx) {
        SuperSegFragment ssegFrag;
        ssegFrag.strand = segSignIdx[segSignIdx.length() - 1];
        segSignIdx.pop_back();
        ssegFrag.seq = segments[segSignIdx].seq;
        ssegFrag.segIdx = segments[segSignIdx].segIdx;
        ssegFrag.SN = segments[segSignIdx].SN;
        ssegFrag.SO = std::to_string(segments[segSignIdx].SO);
        ssegFrag.SR = std::to_string(segments[segSignIdx].SR);
        return ssegFrag;
    }

    bool GFAReader::HasNextSeg(SuperSegFragment ssf) {
        auto range = lines.equal_range(ssf.segIdx + ssf.strand);
        if (range.first == range.second) return false;
        else return true;
    }

    bool GFAReader::IsNewSS(SuperSeg ss){
        int len = 0;
        for (auto ssf: ss) {
            len += ssf.seq.length();
        }
        if (len >= k) return true;
        else return false;
    }

    void GFAReader::VisitedSeg(SuperSeg ss) {
        for (auto ssf : ss) {
            segments[ssf.seq].isVisited[(ssf.strand=="+")?1:0] = true;
        }
    }

    void GFAReader::SaveSuperSeg(std::string savePath) {
        std::fstream file;
        file.open(savePath, std::ios::out);
        if (!file.is_open()){
            std::cout << "Open fail!" << std::endl;
        }
        int count_3 = 0, count_big = 0, count_all = 0;
        for (auto ss: superSegments) {
//            for (auto ssf: ss) {
//                file.write((const char*)&ssf, sizeof(SuperSegFragment));
//            }
            count_all++;
            if ( ss.size() == 3 ) count_3++;
            if ( ss.size() > 3 ) count_big++;
            for (auto ssf: ss) {
                file << SsfToString(ssf) << "\n";
            }
            file << "\n----------------\n";
        }
        file << "总共生成的ss数量：" << count_all << ".\n";
        file << "生成等于2的ss数量：" << count_all - count_3 - count_big << ".\n";
        file << "生成等于3的ss数量：" << count_3 << ".\n";
        file << "生成大于3的ss数量：" << count_big << ".\n";
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

    std::string SsfToString(SuperSegFragment ssf) {
        return ssf.segIdx + " " + ssf.strand + " " + ssf.seq + " SR:i:" + ssf.SR + " SO:i:" + ssf.SO + " SN:Z:" + ssf.SN;
    }

}
//int main()
//{
//    gjfish::GFAReader *reader = new gjfish::GFAReader("../data/GRCh38-20-0.10b.gfa");
//    reader->Start();
//    reader->GenerateSuperSeg();
//    int max_seq_size = 0; // max_size = 30092626 << 2^32
//    int total_num = 0; // total_num = 192458 << 2^32
//    for(auto it : reader->segments) {
//        if (it.second.seq.size() > max_seq_size) max_seq_size = it.second.seq.size();
//        total_num++;
//    }
//    std::cout << "Max_seq_size: " << max_seq_size << std::endl;
//    std::cout << "Total_num: " << total_num << std::endl;
//    return 0;
//}

