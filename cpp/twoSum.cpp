//
// Created by liyuan on 2020/3/30.
//
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
using namespace std;

//class Solution {
//public:
//    vector<int> twoSum(vector<int>& nums, int target) {
//        vector<int> result;
//        auto size = nums.size();
//        for (auto i = 0; i < size-1; i++) {
//            for (auto j = i+1; j < size; j++) {
//                if (nums[i]+nums[j] == target) {
//                    result.push_back(i);
//                    result.push_back(j);
//                    return result;
//                }
//            }
//        }
//        return result;
//    }
//};




//class Solution {
//public:
//    vector<int> twoSum(vector<int>& nums, int target) {
//        vector<int> result;
//        auto size = nums.size();
//        map<int, int> m;
//        for (auto i = 0; i < size; i++) {
//            m[nums[i]] = i;
//        }
//        for (auto i = 0; i < size; i++) {
//            auto v = target - nums[i];
//            auto it = m.find(v);
//            if (it != m.end() && it->second != i) {
//                result.push_back(i);
//                result.push_back(it->second);
//                return result;
//            }
//        }
//        return result;
//    }
//};



class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        vector<int> result;
        auto size = nums.size();
        unordered_map<int, int> m;
        for (auto i = 0; i < size; i++) {
            auto v = target - nums[i];
            auto it = m.find(v);
            if (it != m.end()) {
                result.push_back(it->second);
                result.push_back(i);
                return result;
            }
            m[nums[i]] = i;
        }
        return result;
    }
};


void print(vector<int> &nums) {
    for (auto it=nums.begin(); it != nums.end(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}


int main() {
    Solution s;
//    vector<int> nums{2, 7, 11, 15};
    vector<int> nums{3,2,4};
    vector<int> result;
//    result = s.twoSum(nums, 9);
    result = s.twoSum(nums, 6);
    print(result);
}