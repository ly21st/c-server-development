//
// Created by liyuan on 2020/3/29.
//
#include <iostream>
#include <deque>
#include <list>
using namespace std;

class MyCircularDeque {
private:
    list<int> contain;
    size_t max_size;
public:
    /** Initialize your data structure here. Set the size of the deque to be k. */
    MyCircularDeque(int k): max_size(k) {

    }

    /** Adds an item at the front of Deque. Return true if the operation is successful. */
    bool insertFront(int value) {
        if (contain.size() >= max_size) {
            return false;
        }
        contain.push_front(value);
        return true;
    }

    /** Adds an item at the rear of Deque. Return true if the operation is successful. */
    bool insertLast(int value) {
        if (contain.size() >= max_size) {
            return false;
        }
        contain.push_back(value);
        return true;
    }

    /** Deletes an item from the front of Deque. Return true if the operation is successful. */
    bool deleteFront() {
        if (contain.empty()) {
            return false;
        }
        contain.pop_front();
        return true;
    }

    /** Deletes an item from the rear of Deque. Return true if the operation is successful. */
    bool deleteLast() {
        if (contain.empty()) {
            return false;
        }
        contain.pop_back();
        return true;
    }

    /** Get the front item from the deque. */
    int getFront() {
        if (contain.empty()) {
            return -1;
        }
        return contain.front();
    }

    /** Get the last item from the deque. */
    int getRear() {
        if (contain.empty()) {
            return -1;
        }
        return contain.back();
    }

    /** Checks whether the circular deque is empty or not. */
    bool isEmpty() {
        return contain.empty();
    }

    /** Checks whether the circular deque is full or not. */
    bool isFull() {
        return contain.size() == max_size;
    }
};

int main() {
    /**
 * Your MyCircularDeque object will be instantiated and called as such:
     * */

    MyCircularDeque* obj = new MyCircularDeque(3);
    bool param_1 = obj->insertFront(1);
    bool param_2 = obj->insertLast(2);
    bool param_3 = obj->deleteFront();
    bool param_4 = obj->deleteLast();
    int param_5 = obj->getFront();
    int param_6 = obj->getRear();
    bool param_7 = obj->isEmpty();
    bool param_8 = obj->isFull();

}
