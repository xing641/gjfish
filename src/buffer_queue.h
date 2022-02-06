//
// Created by Tuotuo on 1/12/2022.
//

#ifndef SRC_STABLE_BUFFER_QUEUE_H
#define SRC_STABLE_BUFFER_QUEUE_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
namespace gjfish {
    template<class Type>
/*消息队列实现*/

    class BufferQueue {
        BufferQueue &operator=(const BufferQueue &) = delete;

        BufferQueue(const BufferQueue &other) = delete;

    public:
        BufferQueue() : _queue(), _mutex(), _condition() {}

        virtual ~BufferQueue() {}

        void Push(Type record) {
            std::lock_guard<std::mutex> lock(_mutex);
            _queue.push(record);
            _condition.notify_one();
        }

        bool Pop(Type &record, bool isBlocked = true) {
            if (isBlocked) {
                std::unique_lock<std::mutex> lock(_mutex);
                while (_queue.empty()) {
                    _condition.wait(lock);
                }
            } else // If user wants to retrieve data in non-blocking mode
            {
                std::lock_guard<std::mutex> lock(_mutex);
                if (_queue.empty()) {
                    return false;
                }
            }
            std::lock_guard<std::mutex> lock(_mutex);
            record = std::move(_queue.front());
            _queue.pop();
            return true;
        }

        int32_t Size() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _queue.size();
        }

        bool Empty() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _queue.empty();
        }

    public:
        std::queue<Type> _queue;
        mutable std::mutex _mutex;
        std::condition_variable _condition;
    };
}

#endif //SRC_STABLE_BUFFER_QUEUE_H
