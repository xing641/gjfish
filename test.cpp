#include "test.hpp"

// read + parser + hash_create(mem_alloc) + hash_counter(men_alloc) + write
void test1_thread_work(int tid, int line_num, string file_path, HashMap *hm){
    ifstream f(file_path, ios::in);
    int pos = tid * line_num;

    f.seekg(pos, ios::beg);
    int line_cnt = 0;
    string line;
    while(getline(f, line) && line_cnt < line_num){
        segment sg = parse_segment(line);
        count_kmers(sg, hm, tid);
        line_cnt++;
    }
}

void test1(KC__MemAllocator *ma){
    string reader;
    clock_t clock_start, clock_end;
    time_t time_start, time_end;
    int thread_num = 1;

    string file_path = "/mnt/c/Users/yzx/Documents/yegpu_temp/chr2_segment.gfa";
    string res;
    ExecuteCMD(("wc -l " + file_path).c_str(), res);
    int total_line_num = stoi(res);
    
    for (int thread_num = 1; thread_num <= 12; thread_num++){
        int block_line_num = total_line_num / thread_num;

        auto s_start = chrono::system_clock::now();
        clock_start = clock();
        time_start = time(0);

        // 创建哈希表
        HashMap* hm = new HashMap(thread_num, 20, ma);

        // thread_join(file_path, block_line_num, thread_num);
        // 文件读取 + 并发
        vector<thread> ths;
        for (int i = 0; i < thread_num; i++){
            thread th(test1_thread_work, ref(i), ref(block_line_num), ref(file_path), ref(hm));
            ths.emplace_back(move(th));
        }
        for (auto it = ths.begin(); it != ths.end(); it++){
            (*it).join();
        }

        auto s_end = chrono::system_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(s_end - s_start);

        // for (int i = 0; i < 100; i++){
        //         cout << ht->t[i].cnt << " " << ht->t[i].km << endl;
        // }
        clock_end = clock();
        time_end = time(0);
        cout << thread_num << ": " << double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den << " " << (clock_end - clock_start) / CLOCKS_PER_SEC << " " << difftime(time_end, time_start) << endl;
    }
}