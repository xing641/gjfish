#include "hash_counter.hpp"


uint64_t to_bit(string str){
    uint64_t a = 0;
    for(int i = str.size() - 1; i >= 0; i--) {
        a = a << 2; a |= (int)str[i];
    }
    return a;
}

void count_kmers(segment sg, HashMap *hm, int tid){
    string str = sg.str;
    string location = sg.name;
    if(str.length() < hm->k) return;
    for(int i = 0; i < str.length() - hm->k + 1; i++){
        string mer = str.substr(i, hm->k);
        int pos = mer.find_last_of('\004');
        if(pos != mer.npos){i = i + pos; continue;}
        uint64_t key = to_bit(mer);
        string loc = "L" + to_string(i) + location + "L" + to_string(i + hm->k);
        hm->add_kmer(tid, key, loc);
    }
}