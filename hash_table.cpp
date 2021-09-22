#include "hash_table.hpp"


Table* HashMap::table;
uint64_t HashMap::table_capacity;

Block1** HashMap::blocks1;
Block2** HashMap::blocks2;
uint64_t HashMap::blocks_cnt;

Node1* HashMap::nodes1;
uint64_t HashMap::node1_size;
uint64_t HashMap::nodes1_cnt;

Node2* HashMap::nodes2;
uint64_t HashMap::node2_size;
uint64_t HashMap::nodes2_cnt;

bool HashMap::keys_locked;
pthread_barrier_t HashMap::barrier;


// 哈希没有问题
uint32_t HashMap::hashfunction(uint64_t key){
    return key % table_capacity;
}

HashMap::HashMap(uint64_t threads_count, size_t K, KC__MemAllocator* ma){
    // KC__HashMap* hm = (KC__HashMap*)KC__mem_alloc(ma, sizeof(struct KC__HashMap), "hash map");

    // block initialize...
    blocks_cnt = threads_count;
    blocks1 = (Block1**)KC__mem_alloc(ma, sizeof(Block1*) * blocks_cnt, "hash map blocks array");
    for (size_t i = 0; i < blocks_cnt; i++) {
        blocks1[i] = (Block1*)KC__mem_aligned_alloc(ma, sizeof(Block1), "hash map block");
    }
    blocks2 = (Block2**)KC__mem_alloc(ma, sizeof(Block2*) * blocks_cnt, "hash map blocks array");
    for (size_t i = 0; i < blocks_cnt; i++) {
        blocks2[i] = (Block2*)KC__mem_aligned_alloc(ma, sizeof(Block2), "hash map block");
    }

    // hm->kmer_width = KC__calculate_kmer_width(K);
    // hm->kmer_size = KC__calculate_kmer_size(K);
    node1_size = sizeof(Node1);
    node2_size = sizeof(Node2);

    const size_t mem_limit = KC__mem_available(ma);

    // node initialize...
    size_t nodes1_cnt_limit = mem_limit / (sizeof(Node1) * 1 + sizeof(Node2) * 10 + sizeof(uint64_t) * 2) * 1;
    if (nodes1_cnt_limit > MAX_NODE_SIZE) {
        // LOGGING_WARNING("The count of nodes to be allocated is too large: %zu.", nodes_count_limit);
        nodes1_cnt_limit = MAX_NODE_SIZE;
        // LOGGING_WARNING("Reduce the count of nodes to %zu.", nodes_count_limit);
    }
    nodes1_cnt = (uint64_t)nodes1_cnt_limit;
    const size_t nodes1_mem = node1_size * nodes1_cnt;

    size_t nodes2_cnt_limit = mem_limit / (sizeof(Node1) * 1 + sizeof(Node2) * 10 + sizeof(uint64_t) * 2) * 10;
    if (nodes2_cnt_limit > MAX_NODE_SIZE) {
        // LOGGING_WARNING("The count of nodes to be allocated is too large: %zu.", nodes_count_limit);
        nodes2_cnt_limit = MAX_NODE_SIZE;
        // LOGGING_WARNING("Reduce the count of nodes to %zu.", nodes_count_limit);
    }
    nodes2_cnt = (uint64_t)nodes2_cnt_limit;
    const size_t nodes2_mem = node2_size * nodes2_cnt;

    // table initialize...
    const size_t table_mem_limit = mem_limit - nodes1_mem - nodes2_mem;
    const size_t table_capacity_limit = table_mem_limit / sizeof(uint64_t);
    table_capacity = KC__max_prime_number(table_capacity_limit);
    const size_t table_mem = sizeof(uint64_t) * table_capacity;
    table = (uint64_t*)KC__mem_aligned_alloc(ma, table_mem, "hash map table");

    // node initialize...
    nodes1 = (Node1*)KC__mem_aligned_alloc(ma, nodes1_mem, "hash map nodes");
    const uint64_t step1 = nodes1_cnt / (uint64_t)(blocks_cnt);
    for (size_t i = 0; i < blocks_cnt; i++) {
        Block1* block = blocks1[i];
        block->start_id = 1 + step1 * (uint64_t)i;
        block->end_id = (i == blocks_cnt - 1) ? (nodes1_cnt) : (1 + step1 * (uint64_t)(i + 1));
    }

    nodes2 = (Node2*)KC__mem_aligned_alloc(ma, nodes2_mem, "hash map nodes");
    const uint64_t step2 = nodes2_cnt / (uint64_t)(blocks_cnt);
    for (size_t i = 0; i < blocks_cnt; i++) {
        Block2* block = blocks2[i];
        block->start_id = 1 + step2 * (uint64_t)i;
        block->end_id = (i == blocks_cnt - 1) ? (nodes2_cnt) : (1 + step2 * (uint64_t)(i + 1));
    }

    // LOGGING_DEBUG("        Hash table capacity: %zu (limit: %zu)", hm->table_capacity, table_capacity_limit);
    // LOGGING_DEBUG("          Hash table memory: %zu", table_mem);
    // LOGGING_DEBUG("                Nodes count: %zu", nodes_count);
    // LOGGING_DEBUG("               Nodes memory: %zu", nodes_mem);
    // LOGGING_DEBUG("Hash table and nodes memory: %zu (limit: %zu)", table_mem + nodes_mem, mem_limit);
    // for (size_t i = 0; i < blocks_cnt; i++) {
    //     Block1* block = blocks1[i];
    //     LOGGING_DEBUG("Nodes block #%zu (start: %zu, end: %zu, length: %zu)", i, block->start_id, block->end_id, block->end_id - block->start_id);
    // }

    // pthread_barrier_init(&(hm->barrier), NULL, (unsigned int)threads_count);

    hash_map_clear();

}

void HashMap::lock_keys() {
    keys_locked = true;
}

Node1* HashMap::get_node1(uint64_t node_id) {
    char* node1 = (char*)(nodes1);
    node1 += node1_size * (size_t)node_id;
    return (Node1*)node1;
}

Node2* HashMap::get_node2(uint64_t node_id) {
    char* node2 = (char*)(nodes2);
    node2 += node2_size * (size_t)node_id;
    return (Node2*)node2;
}

uint64_t HashMap::request_node1(size_t n) {
    Block1* block = blocks1[n];
    uint64_t node_id;

    do {
        node_id = block->next_id;
        if (node_id == block->end_id) {
            return NODE_ID_NULL;
        }
    } while (!__sync_bool_compare_and_swap(&(block->next_id), node_id, node_id + 1));

    Node1* node = get_node1(node_id);
    node->cnt = 0;
    node->loc_end_node_id = 0;
    node->loc_start_node_id = 0;

    return node_id;
}

uint64_t HashMap::request_node2(size_t n) {
    Block2* block = blocks2[n];
    uint64_t node_id;

    do {
        node_id = block->next_id;
        if (node_id == block->end_id) {
            return NODE_ID_NULL;
        }
    } while (!__sync_bool_compare_and_swap(&(block->next_id), node_id, node_id + 1));

    Node2* node = get_node2(node_id);

    return node_id;
}

uint64_t HashMap::polling_request_node1(size_t n) {
    uint64_t node_id = request_node1(n);
    if (node_id != NODE_ID_NULL) {
        return node_id;
    }

    size_t m = blocks_cnt;
    for (size_t i = 0; i < m - 1; i++) {
        n++;
        if (n == m)
            n = 0;

        node_id = request_node1(n);
        if (node_id != NODE_ID_NULL) {
            return node_id;
        }
    }
    return NODE_ID_NULL;
}

uint64_t HashMap::polling_request_node2(size_t n) {
    uint64_t node_id = request_node2(n);
    if (node_id != NODE_ID_NULL) {
        return node_id;
    }

    size_t m = blocks_cnt;
    for (size_t i = 0; i < m - 1; i++) {
        n++;
        if (n == m)
            n = 0;

        node_id = request_node2(n);
        if (node_id != NODE_ID_NULL) {
            return node_id;
        }
    }
    return NODE_ID_NULL;
}

void* HashMap::hash_map_clear_table(void* ptr) {
    ClearTableParam* param = (ClearTableParam*)ptr;
    for (size_t i = param->start; i < param->end; i++) {
        table[i] = 0;
    }
    pthread_exit(NULL);
}

void HashMap::hash_map_clear(){
    keys_locked = false;

    for (size_t i = 0; i < blocks_cnt; i++) {
        Block1* block1 = blocks1[i];
        Block2* block2 = blocks2[i];

        block1->next_id = block1->start_id;
        block1->current_id = 0;
        block1->synced = false;

        block2->next_id = block2->start_id;
        block2->current_id = 0;
        block2->synced = false;
    }

    // The count of threads used to clear hash table equals to blocks count.
    size_t clear_table_threads_count = blocks_cnt;
    pthread_t threads[clear_table_threads_count];
    ClearTableParam params[clear_table_threads_count];
    size_t step = table_capacity / clear_table_threads_count;
    for (size_t i = 0; i < clear_table_threads_count; i++) {
        size_t start = i * step;
        size_t end = (i == clear_table_threads_count - 1) ? (table_capacity) : ((i + 1) * step);
        params[i].n = i;
        params[i].start = start;
        params[i].end = end;
        pthread_create(&(threads[i]), NULL, hash_map_clear_table, &(params[i]));
    }

    for (size_t i = 0; i < clear_table_threads_count; i++) {
        pthread_join(threads[i], NULL);
    }
}



HashMap::~HashMap(){
    for (size_t i = 0; i < blocks_cnt; i++) {
        free(blocks1[i]);
        free(blocks2[i]);
    }
    free(blocks1);
    free(blocks2);

    free(nodes1);
    free(nodes2);
    free(table);

    pthread_barrier_destroy(&(barrier));
}


// Add K-mer to collision list
uint64_t HashMap::collision_list_add_kmer1(uint64_t** list, const uint64_t kmer) {
    uint64_t node_id;
    uint64_t* p = *list;
    while (true) {
        node_id = *p;

        if (node_id == NODE_ID_NULL) {
            break;
        }

        Node1* node = get_node1(node_id);
        if (node->km == kmer) {
            uint32_t count;
            do {
                count = node->cnt;
                if (count == COUNT_MAX)
                    break;
            } while (!__sync_bool_compare_and_swap(&(node->cnt), count, count + 1));

            break;
        }
        p = &(node->next);
    }

    *list = p;
    return node_id;
}

uint64_t HashMap::collision_list_add_kmer2(uint64_t** list) {
    uint64_t node_id;
    uint64_t* p = *list;
    while (true) {
        node_id = *p;

        if (node_id == NODE_ID_NULL) {
            break;
        }

        Node2* node = get_node2(node_id);
        p = &(node->next);
    }

    *list = p;
    return node_id;
}

// This is very important.
bool HashMap::add_kmer(size_t n, const uint64_t kmer, string loc) {
    Block1* block = blocks1[n];

    if (!(block->synced) && (block->current_id == NODE_ID_NULL)) {
        block->current_id = polling_request_node1(n);
        if (block->current_id == NODE_ID_NULL) {
            keys_locked = true;
        }
    }

    // For multi-threaded situation, when the keys are locked by one of the working threads, the other threads may
    // not yet knew the change, so they need to sync once. As the failure of adding K-mer may occur a long time later
    // (if one thread adds many existed K-mers continuously), checking if the keys of the hash map are locked is a
    // good signal.

    if (!(block->synced) && keys_locked) {
        pthread_barrier_wait(&(barrier));
        block->synced = true;
    }

    size_t table_idx = hashfunction(kmer);

    uint64_t* collision_list = &(table[table_idx]);
    uint64_t node_id = collision_list_add_kmer1(&collision_list, kmer);

    if (node_id != NODE_ID_NULL) {
        return true;
    }

    // If some thread has set keys_locked, assume this thread noticed the change here and return false, while the other
    // thread has not seen the change and is adding a new node to hash table, then it may cause inconsistency.
    // Checking if this thread has been synced is important.
    if (block->synced && keys_locked) {
        return false;
    }

    Node1 *node = get_node1(block->current_id);
    node->km = kmer;
    node->cnt = 1;
    node->next = NODE_ID_NULL;

    do {
        node_id = collision_list_add_kmer1(&collision_list, kmer);
        if (node_id != NODE_ID_NULL) {
            // Mark the node invalid.
            node->cnt = 0;
            return true;
        }
    } while (!__sync_bool_compare_and_swap(collision_list, node_id, block->current_id));

    block->current_id = NODE_ID_NULL;

    return true;
}

void HashMap::finish_adding_kmers(size_t n) {
    Block1* block = blocks1[n];

    if (!(block->synced)) {
        pthread_barrier_wait(&(barrier));
        block->synced = true;
    }
}

void HashMap::hash_map_export(size_t n, ExportCallback callback, void* data, size_t* exported_count) {
    size_t ec = 0;

    Block1* block = blocks1[n];
    for (uint64_t i = block->start_id; i < block->next_id; i++) {
        Node1* node = get_node1(i);
        if (node->cnt != 0) {
            callback(node->km, node->cnt, data);
            ec++;
        } else {
        }
    }

    if (exported_count != NULL) {
        *exported_count = ec;
    }
}
