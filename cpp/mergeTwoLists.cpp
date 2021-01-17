#include <iostream>
using namespace std;

struct ListNode
{
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}

    ListNode* add(ListNode *node) {
        next = node;
        node->next = nullptr;
        return node;
    }
};

class Solution
{
public:
    ListNode *mergeTwoLists(ListNode *l1, ListNode *l2)
    {
        ListNode *new_list = nullptr, *cur_node = nullptr;
        if (l1 == nullptr) 
            return l2;
        if (l2 == nullptr) 
            return l1;
        if (l1->val <= l2->val) {
            new_list = l1;
            l1 = l1->next;
        }   
        else {
            new_list = l2;
            l2 = l2->next;
        }
        cur_node = new_list;
        while (l1 != nullptr && l2 != nullptr) {
            if (l1->val <= l2->val) {
                cur_node->next = l1;
                l1 = l1->next;
            } else {
                cur_node->next = l2;
                l2 = l2->next;
            }
            cur_node = cur_node->next;
        }
        if (l1 != nullptr) 
            cur_node->next = l1;
        else 
            cur_node->next = l2;
        return new_list;
    }
};

void print(ListNode *l) {
    while (l != nullptr) {
        cout << l->val << " ";
        l = l->next;
    }
    cout << endl;
}

int main() {
    ListNode *l1;
    ListNode *l2;
    // 1->2->4, 1->3->4
    ListNode node1(1), node2(2), node3(4);
    ListNode node4(1), node5(3), node6(4);
    l1 = &node1;
    l1->add(&node2)->add(&node3);
    l2 = &node4;
    l2->add(&node5)->add(&node6);

    print(l1);
    print(l2);

    Solution s;
    ListNode *l3 = s.mergeTwoLists(l1, l2);
    print(l3);

}