#include <iostream>
#include <vector>
using namespace std;

void print(vector<int>& nums, int n) {
    cout << "n:" << n << endl;

    for (auto p = &nums[0]; p != &nums[n]; p++) {
        cout <<*p<< " ";
    }
    cout << endl << endl;
}

// class Solution {
// public:
//     int removeDuplicates(vector<int>& nums) {
//         auto pi = nums.begin();
//         auto pn = nums.end();
//         if (pi == pn) {
//             return 0;
//         }
//         for (; pi != pn  - 1; pi++) {
//             auto pj = pi + 1;
//             auto pm = pn;
//             while ((pj != pn) && (*pj == *pi)) {
//                 pj++;
//                 pm--;
//             }
//             if ((pi+1) != pj) 
//                 std::copy(pj, pn, pi+1);
//             pn = pm;
//         }
        
//         return pn - nums.begin() ;
//     }
// };


// class Solution {
// public:
//     int removeDuplicates(vector<int>& nums) {
//         auto n = nums.size();
//         if (n==0) {
//             return 0;
//         }
//         for (auto i = 0; i != n-1; i++) {
//             auto j = i + 1;
//             auto m = n;
//             while ((j != n) && (nums[j] == nums[i])) {
//                 j++;
//                 m--;
//             }
//             if ((i+1) != j) 
//                 std::copy(&nums[j], &nums[n], &nums[i+1]);
//             n = m;
//             if (i+1 == n) {
//                 break;
//             }
//         }
        
//         return n;
//     }
// };

// class Solution {
// public:
//     int removeDuplicates(vector<int>& nums) {
//         auto pb = nums.begin();
//         auto pe = nums.end();
//         while (pb != nums.end()) {
//             auto &tmp = *pb;
//             auto pn = pb + 1;
//             while (pn != nums.end() && (*pb == *pn)) {
//                 pn = nums.erase(pn);
//             }
//             pb = pn;
//         }
        
//         return nums.end() - nums.begin();
//     }
// };


// class Solution {
// public:
//     int removeDuplicates(vector<int>& nums) {
//         auto begin = nums.begin();
//         auto end = nums.end();
//         if (begin == end) {
//             return 0;
//         }
//         auto pre_ptr = begin;
//         auto next_ptr = pre_ptr + 1;
//         while (next_ptr != end) {
//             while(next_ptr != end && *next_ptr == *pre_ptr) {
//                 next_ptr++;
//             }
//             if (next_ptr != end) {
//                 *++pre_ptr = *next_ptr++;
//             }
//         } 
//         pre_ptr++;
//         return pre_ptr - begin;
//     }
// };

// class Solution {
// public:
//     int removeDuplicates(vector<int>& nums) {
//         auto first = nums.begin();
//         auto last = nums.end();
//         auto raw_first = first;

//         if (first == last) {
//             return 0;
//         }
//         auto result = first;
//         while (++first != last) {
//             if (*result != *first)
//                 *++result = *first;
//         }
//         return ++result - raw_first;
//     }
// };

class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        auto first = nums.begin();
        auto last = nums.end();
        auto raw_first = first;

        if (first == last) {
            return 0;
        }
        auto result = first;
        auto value = *result;
        while (++first != last) {
            if (value != *first) {
                value = *first;
                *++result = value;
            }
        }
        return ++result - raw_first;
    }
};

void test1() {
    vector<int> nums{0,0,1,1,1,2,2,3,3,4};
    // vector<int> nums{1, 1};
    Solution s;
    auto n = s.removeDuplicates(nums);
    print(nums, n);
}


int main() {
    test1();

}


