//
// Created by liyuan on 2020/4/1.
//
/**
 * 编写一个函数，检查输入的链表是否是回文的。

 

示例 1：

输入： 1->2
输出： false
示例 2：

输入： 1->2->2->1
输出： true
 

进阶：
你能否用 O(n) 时间复杂度和 O(1) 空间复杂度解决此题？

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/palindrome-linked-list-lcci
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。
 */

/**
 * Definition for singly-linked list.
 *
*/

#include <iostream>
using namespace std;

struct ListNode {
      int val;
      ListNode *next;
      ListNode(int x) : val(x), next(NULL) {}
};

//class Solution {
//public:
//    bool isPalindrome(ListNode* head) {
//        auto len =  0;
//        auto first = head;
//        if (head == NULL) return true;
//        while (first != NULL) {
//            len++;
//            first = first->next;
//        }
//        if (len < 2) return true;
//        auto mid = len / 2;
//        auto pre = head;
//        auto second_head = head;
//        for (auto i = 1; i <= mid; i++){
//            pre = second_head;
//            second_head = second_head->next;
//        }
//        pre->next = NULL;
//        if (len % 2) second_head = second_head->next;
//        ListNode *new_second_head = NULL;
//        while (second_head != NULL){
//            pre = second_head;
//            second_head = second_head->next;
//            pre->next = new_second_head;
//            new_second_head = pre;
//
//        }
//        while (head != NULL) {
//            if (head->val != new_second_head->val) {
//                return false;
//            }
//            head = head->next;
//            new_second_head = new_second_head->next;
//        }
//        return true;
//    }
//};


//class Solution {
//public:
//    bool isPalindrome(ListNode* head) {
//        auto len =  0;
//        auto first = head;
//        while (first != NULL) {
//            len++;
//            first = first->next;
//        }
//        if (len < 2) return true;
//        auto mid = len / 2;
//        ListNode* pre = NULL;
//        ListNode* new_first_head = NULL;
//        first = head;
//        for (auto i = 1; i <= mid; i++){
//            pre = first;
//            first = first->next;
//            pre->next = new_first_head;
//            new_first_head = pre;
//        }
//        ListNode *second_head = first;
//        if (len % 2) second_head = second_head->next;
//        while (new_first_head != NULL) {
//            if (new_first_head->val != second_head->val) {
//                return false;
//            }
//            new_first_head = new_first_head->next;
//            second_head = second_head->next;
//        }
//        return true;
//    }
//};


//class Solution {
//public:
//    bool isPalindrome(struct ListNode* head){
//        if (!head||!head->next)
//            return 1;
//        struct ListNode *fast = head, *slow = head, *nh = (struct ListNode *)malloc(sizeof(struct ListNode));
//        nh->next = NULL;
//        while (fast&&fast->next){
//            slow = slow->next;
//            fast = fast->next->next;
//            head->next = nh->next;
//            nh->next = head;
//            head = slow;
//        }
//        if (fast)
//            slow = slow->next;
//        head = nh->next;
//        while (head){
//            if (head->val != slow->val)
//                return 0;
//            head = head->next;
//            slow = slow->next;
//        }
//        return 1;
//    }
//};



class Solution {
public:
    bool isPalindrome(struct ListNode* head){
        if (!head||!head->next)
            return 1;
        struct ListNode *fast = head, *slow = head, *nh = NULL;
        while (fast&&fast->next){
            slow = slow->next;
            fast = fast->next->next;
            head->next = nh;
            nh = head;
            head = slow;
        }
        if (fast)
            slow = slow->next;
        head = nh;
        while (head){
            if (head->val != slow->val)
                return 0;
            head = head->next;
            slow = slow->next;
        }
        return 1;
    }
};



void print(ListNode* head) {
    while (head != NULL) {
        cout << head->val << " ";
        head = head->next;
    }
    cout << endl;
}

int main() {
    ListNode node1(1), node2(2), node3(2), node4(1);
    node1.next = &node2;
    node2.next = &node3;
    node3.next = &node4;
    Solution s;
    print(&node1);
    cout << s.isPalindrome(&node1) << endl;
}


