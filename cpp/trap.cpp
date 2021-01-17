//
// Created by liyuan on 2020/3/29.
//
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int trap(vector<int>& height) {
        auto first = height.begin();
        auto last = height.end();
        if (last - first < 3)
            return 0;
        auto total = 0;
        while (first != last) {
            while (first + 1 != last && *first <= *(first + 1)) first++;
            if (first+1 == last) break;
            auto cur = first + 2;
            if (cur == last)
                break;
            auto p_max = cur;
            while (cur != last && *cur < *first) {
                if (*p_max < *cur) {
                    p_max = cur;
                }
                cur++;
            }
            if (cur == last) {
                for (auto it = first+1; it != p_max; it++) {
                    if (*p_max <= *it) continue;
                    total = *p_max - *it + total;
                }
                first = p_max;
            } else {
                for (auto it = first + 1; it != cur; it++) {
                    total = *first - *it + total;
                }
                first = cur;
            }
        }
        return total;
    }
};

int main() {
//    vector<int> nums{0,1,0,2,1,0,1,3,2,1,2,1};
//    vector<int> nums{4, 2, 3};
//    vector<int> nums{0, 2, 0};
//    vector<int> nums{5,4,1,2};
    vector<int> nums{9,6,8,8,5,6,3};
    Solution s;
    auto count = s.trap(nums);
    cout << count << endl;
}