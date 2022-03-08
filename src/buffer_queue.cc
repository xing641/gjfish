//
// Created by Tuotuo on 1/12/2022.
//
#include "src/buffer_queue.h"
#include <thread>
#include <unistd.h>

pthread_barrier_t barrier;

void add_queue(gjfish::BufferQueue<int> &q){
    int loop = 1000000;
    // pthread_barrier_wait(&barrier);
    while (loop--){
        q.Push(1);
    }
}

void pop_queue(gjfish::BufferQueue<int> &q){
    int k;
    int cnt = 0;
    // pthread_barrier_wait(&barrier);
    while(!q.Empty()){
        if(q.Pop(k)){
            cnt++;
        }
    }
}

// int main()
// {
//    gjfish::BufferQueue<int> queue;
//    int thread_count = 48;
//    std::thread threads[thread_count];
//    for (int i = 0 ; i < thread_count; i++) {
//        threads[i] = std::thread(add_queue, std::ref(queue));
//    }

//     for (auto & thread : threads) {
//        thread.join();
//     }

//    for (int i = 0; i < thread_count; i++) {
//        threads[i] = std::thread(pop_queue, std::ref(queue));
//    }

//    for (auto & thread : threads) {
//        thread.join();
//    }

//    int m = queue.Size();

//    std::cout << m << std::endl;
//    return 0;
// }
