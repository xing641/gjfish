//
// Created by user1 on 2021/11/23.
//

#ifndef SRC_STABLE_LOCKFREE_STACK_H
#define SRC_STABLE_LOCKFREE_STACK_H
#include <atomic>
#include <iostream>

namespace gjfish{
    template <typename Data>
    struct Node{
        Node* next;
        Data val;
        explicit Node(Data const& val_):val(val_), next(nullptr){}
        Node():val(0), next(nullptr){};
    };

    template <typename Data>
    class LockFreeStack {
    public:
        void Push(Data data){
            //TODO 换成内存池来分配
            Node<Data>* node = new Node<Data>(data);
            node->next = head.load();
            while(!head.compare_exchange_weak(node->next, node));
        }

        void Display(){
            int size = 0;
            for (auto it = head.load(); it->next != nullptr; it = it->next){
                std::cout << it->val << " | ";
                size++;
            }
        }

        bool Empty() {
            return (head->next == nullptr);
        }

        LockFreeStack(){
            head = new Node<Data>(0);
        }

    private:
        std::atomic<Node<Data>*> head;
    };
}



#endif //SRC_STABLE_LOCKFREE_STACK_H
