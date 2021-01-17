//
// Created by liyuan on 2020/3/27.
//
#include <iostream>
#include <vector>
using namespace std;

//class Solution {
//public:
//    void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
//        if (n == 0) return;
//        if (m == 0) {
//            copy(nums2.begin(), nums2.begin()+n, nums1.begin());
//            return;
//        }
//        vector<int> result(m+n);
//        int i = 0, j = 0, cur = 0;
//        while (i < m && j < n) {
//            if (nums1[i] <= nums2[j]) {
//                result[cur++] = nums1[i++];
//            } else {
//                result[cur++] = nums2[j++];
//            }
//        }
//        copy(nums1.begin()+i, nums1.begin()+m, copy(nums2.begin()+j, nums2.begin()+n, result.begin()+cur));
//        copy(result.begin(), result.begin()+m+n, nums1.begin());
//    }
//};



class Solution {
public:
    void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
        if (n == 0) return;
        if (m == 0) {
            copy(nums2.begin(), nums2.begin()+n, nums1.begin());
            return;
        }
        copy_backward(nums1.begin(), nums1.begin()+m, nums1.begin()+n+m);
        int i = 0, j = 0, cur = 0;
        while (i < m && j < n) {
            if (nums1[n+i] <= nums2[j]) {
                nums1[cur++] = nums1[n+i];
                i++;
            } else {
                nums1[cur++] = nums2[j++];
            }
        }
        copy(nums1.begin()+n+i, nums1.begin()+n+m, copy(nums2.begin()+j, nums2.end(), nums1.begin()+cur));
    }
};


void print(vector<int>& nums) {
    for (auto it = nums.begin(); it != nums.end(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}

int main() {
    vector<int> nums1 = {1,2,3,0,0,0};
    vector<int> nums2 = {2,5,6};
    Solution s;
    s.merge(nums1, 3, nums2, 3);
    print(nums1);
}