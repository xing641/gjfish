#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include<bits/stdc++.h>
#include"mem_allocator.hpp"
#include"utils.hpp"
#define MAXSIZE 100
#define MAX_KMER_SIZE 100
#define MAX_NODE_SIZE UINT64_MAX
#define COUNT_MAX UINT32_MAX
#define NODE_ID_NULL 0
using namespace std;

const unsigned char nt4_seq_table[4] = {'A','C','G','T'};

// segment & kemr
struct segment{
    string str;
    string name;
};



/**********************************************/ 

/***************hashtable******************/

typedef void (*ExportCallback) (const uint64_t kmer, uint64_t count, void* data);


// 可优化对齐长度,Entry可以加内存池
typedef uint64_t Table;

struct Kmer{
    uint64_t km;
    uint64_t cnt;
    uint64_t start_node_id;
    uint64_t end_node_id;
};

struct Block1{
    uint64_t start_id;
    uint64_t end_id;

    uint64_t current_id;


    uint64_t next_id;

    bool synced;
};

struct Block2{
    uint64_t start_id;
    uint64_t end_id;

    uint64_t current_id;


    uint64_t next_id;

    bool synced;
};

struct Node1{
    uint64_t next;
    uint64_t cnt;

    uint64_t km;

    uint64_t loc_start_node_id;
    uint64_t loc_end_node_id;
};

struct Node2{
    char loc[24];
    uint64_t next;
};

typedef struct {
    size_t n;
    size_t start;
    size_t end;
} ClearTableParam;

class HashMap{
public:
    static Table *table;
    static uint64_t table_capacity;

    static Block1 **blocks1;
    static Block2 **blocks2;
    static uint64_t blocks_cnt;

    static Node1 *nodes1;
    static uint64_t node1_size;
    static uint64_t nodes1_cnt;

    static Node2 *nodes2;
    static uint64_t node2_size;
    static uint64_t nodes2_cnt;

    int k;
    static bool keys_locked;
    static pthread_barrier_t barrier;

    // function
    HashMap(uint64_t threads_count, size_t K, KC__MemAllocator* ma);

    // uint64_t max_key_count();
    // void set_table_capacity();
    void lock_keys();
    void hash_map_clear();
    static void* hash_map_clear_table(void* ptr);

    static Node1* get_node1(uint64_t node_id);
    static Node2* get_node2(uint64_t node_id);

    static uint64_t request_node1(size_t n);
    static uint64_t request_node2(size_t n);
    static uint64_t polling_request_node1(size_t n);
    static uint64_t polling_request_node2(size_t n);

    static uint64_t collision_list_add_kmer1(uint64_t** list, const uint64_t kmer);
    static uint64_t collision_list_add_kmer2(uint64_t** list);
    
    static bool add_kmer(size_t n, const uint64_t kmer, string loc);

    void finish_adding_kmers(size_t n);
    void hash_map_export(size_t n, ExportCallback callback, void* data, size_t* exported_count);

    static uint32_t hashfunction(uint64_t key);

    mutex mtx;
    ~HashMap();
};


#endif
