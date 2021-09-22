#include"kmer_processor.hpp"


segment parse_segment(string &line)
{
    string str;
    segment mer;
    int name_start = 0, start = 0,end = 0;
    string seg;
    int count = 0;
    for(int i = 0; i < line.size(); i++){
        if(line[i] == '\t'){
            count++;
            if (count == 1) name_start = i + 1;
            else if(count == 2) start = i + 1;
            else if(count == 3) end = i - 1;
        }
        if(count == 3) {break;}
    }
    string name  = line.substr(name_start, start - name_start - 1);
    str = line.substr(start, end - start + 1);
    for(int i = 0; i < end - start + 1; i++){
        str[i] = seq_nt4_table[str[i]];
    }
    mer.str = str;
    // mer.name = atoi(name.c_str()+1);
    mer.name = name;
    return mer;
}

void read_line(int k, string &line, multimap<string, string> &line_hm, unordered_map<string, string> &segment_hm)
{
    string str1, str2;
    segment mer;
    int s1_start = 0, s1_end=0, s2_start = 0, s2_end = 0;
    char s1_strand = 0, s2_strand = 0;
    string seg;
    int count = 0;
    for(int i=0; i<line.size(); i++){
        if(line[i]=='\t'){
            count++;
            if (count == 1) s1_start = i+1;
            else if(count == 2) {s1_end = i-1; s1_strand = line[i+1];}
            else if(count == 3) s2_start = i+1;
            else if(count == 4) {s2_end = i-1; s2_strand = line[i+1];}
        }
        if(count == 4) {break;}
    }
    string s1_name = line.substr(s1_start, s1_end-s1_start+1);
    string s2_name = line.substr(s2_start, s2_end-s2_start+1);

    line_hm.insert(make_pair(s1_name + s1_strand, s2_name + s2_strand));
}

void segment_dfs(string seg, int k, unordered_map<string, string> &segment_hm, multimap<string, string> &line_hm, segment to_merge_seg, vector<segment> &merge_result){
    segment tmp_merge_seg;
    tmp_merge_seg.name = seg;
    tmp_merge_seg.str = segment_hm[tmp_merge_seg.name.substr(0, tmp_merge_seg.name.size() - 1)];

    if (seg.back() == '-') {
        reverse(tmp_merge_seg.str.begin(), tmp_merge_seg.str.end());
    }

    if (to_merge_seg.str == "" ) {
        if (tmp_merge_seg.str.size() >= k) {
            to_merge_seg.name += "L";
            to_merge_seg.name += to_string(tmp_merge_seg.str.size() - k + 1);
            tmp_merge_seg.str = tmp_merge_seg.str.substr(tmp_merge_seg.str.size() - k, k - 1);
        } else {
            return;
        }
    }

    if (to_merge_seg.str.size() + tmp_merge_seg.str.size() >= 2 * k - 2){
        to_merge_seg.name += tmp_merge_seg.name;
        to_merge_seg.str += tmp_merge_seg.str.substr(0, k - 1);
        if (tmp_merge_seg.str.size() >= k) {
            to_merge_seg.name += "L";
            to_merge_seg.name += to_string(k - 1);
        } else {
            to_merge_seg.name += "L";
            to_merge_seg.name += to_string(tmp_merge_seg.str.size() - 1);
        }
        merge_result.push_back(to_merge_seg);
        return;
    } else {
        to_merge_seg.name += tmp_merge_seg.name;
        to_merge_seg.str += tmp_merge_seg.str;
    }
    
    auto range = line_hm.equal_range(seg);
    if (range.first == range.second) return;
    for (multimap<string, string>::iterator it = range.first; it != range.second; it++) {
        segment_dfs(it->second, k, segment_hm,line_hm, to_merge_seg, merge_result);
    }
}

void from_line_to_segment_and_wrtie(int k, unordered_map<string, string> &segment_hm, multimap<string, string> &line_hm){
    string file_path = "merged_result.tmp";
    for (auto it: segment_hm){
        string pos_start_seg = it.first + "+";
        string neg_start_seg = it.first + "-";

        vector<segment> pos_dfs, neg_dfs;
        segment to_be_merged_seg;
        to_be_merged_seg.str = "";
        to_be_merged_seg.name = "";

        segment_dfs(pos_start_seg, k, segment_hm, line_hm, to_be_merged_seg, pos_dfs);
        segment_dfs(neg_start_seg, k, segment_hm, line_hm, to_be_merged_seg, neg_dfs);

        file_write(pos_dfs, file_path);
        file_write(neg_dfs, file_path);
    }
}

void file_write(vector<segment> &merged_seg, string file_path){
    ofstream file(file_path, ios::app);
    for (auto it: merged_seg){
        file << it.name << " " << it.str << "\n";
    }
    file.close();
}

// int main()
// {
//     multimap<string, string> line_hm;
//     unordered_map<string, string> segment_hm;

//     string file_path = "/data/home/zxyang/data/seg.gfa";
//     ifstream seg_file(file_path, ios::in);
//     string line;
//     while(getline(seg_file, line)){
//         segment tmp_seg = parse_segment(line);
//         segment_hm[tmp_seg.name] = tmp_seg.str;
//     }

//     string line_file_path = "/data/home/zxyang/data/line.gfa";
//     ifstream line_file(line_file_path, ios::in);
//     while(getline(line_file, line)){
//         read_line(20, line, line_hm, segment_hm);
//     }

//     from_line_to_segment_and_wrtie(20, segment_hm, line_hm);

//     return 0;
// }
