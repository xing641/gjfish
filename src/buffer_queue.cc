//
// Created by Tuotuo on 1/12/2022.
//
#include "src/buffer_queue.h"
#include <thread>
#include <unistd.h>

pthread_barrier_t barrier;

void add_queue(gjfish::BufferQueue<int> &q){
    int loop = 1000000;
    pthread_barrier_wait(&barrier);
    while (loop--){
        q.Push(1);
    }
}

void pop_queue(gjfish::BufferQueue<int> &q){
    int k;
    int cnt = 0;
    pthread_barrier_wait(&barrier);
    while(cnt != 1000000){
        if(q.Pop(k, false)){
            cnt++;
        }
    }
}

//int main()
//{
//    gjfish::ConcurrentQueue<int> queue;
//    std::thread threads[10];
//    pthread_barrier_init(&barrier, NULL, 11);
//    for (int i = 0 ; i < 5; i++) {
//        threads[i] = std::thread(add_queue, std::ref(queue));
//    }
//
//    for (int i = 5; i < 10; i++) {
//        threads[i] = std::thread(pop_queue, std::ref(queue));
//    }
//
//    pthread_barrier_wait(&barrier);
//    pthread_barrier_destroy(&barrier);
//    for (auto & thread : threads) {
//        thread.join();
//    }
//
//    int m = queue.Size();
//
//    std::cout << m << std::endl;
//    return 0;
//}
