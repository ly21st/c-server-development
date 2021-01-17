#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// class Solution {
// public:
//     void rotate(vector<int>& nums, int k) {
//         auto first = nums.begin();
//         auto last = nums.end();
//         if (first == last)
//             return;
//         if (k <= 0)
//             return;
//         k = k % (last - first);
//         for (auto i = 0; i < k; i++) {
//             auto value = *(nums.end()-1);
//             std::copy(nums.begin(), nums.end()-1, nums.begin()+1);
//             *nums.begin() = value;
//         }
//     }
// };

// class Solution {
// public:
//     void rotate(vector<int>& nums, int k) {
//         auto first = nums.begin();
//         auto last = nums.end();
//         if (first == last)
//             return;
//         if (k <= 0)
//             return;
//         k = k % (last - first);
//         reverse(first, last-k);
//         reverse(last-k, last);
//         reverse(first, last);

//     }

//     void reverse(vector<int>::iterator first, vector<int>::iterator last) {
//         for (; first < last;) {
//             swap(*first++, *--last);
//         }
//     }
// };

// class Solution {
// public:
//     void rotate(vector<int>& nums, int k) {
//         auto first = nums.begin();
//         auto last = nums.end();
//         if (first == last)
//             return;
//         if (k <= 0)
//             return;
//         k = k % (last - first);
//         reverse(first, last-k);
//         reverse(last-k, last);
//         reverse(first, last);

//     }

//     inline void reverse(vector<int>::iterator first, vector<int>::iterator last) {
//         while (first < last) {
//             swap(*first++, *--last);
//         }
//     }

//     inline void swap(int &n1, int &n2) {
//         auto tmp = n1;
//         n1 = n2;
//         n2 = tmp;
//     }
// };

// class Solution {
// public:
//     void rotate(vector<int>& nums, int k) {
//         auto first = nums.begin();
//         auto last = nums.end();
//         if (first == last)
//             return;
//         if (k <= 0)
//             return;
//         k = k % (last - first);
//         reverse(first, last-k);
//         reverse(last-k, last);
//         reverse(first, last);

//     }

//     void reverse(vector<int>::iterator first, vector<int>::iterator last) {
//         while (first < last) {
//             swap(*first++, *--last);
//         }
//     }

//     void swap(int &n1, int &n2) {
//         auto tmp = n1;
//         n1 = n2;
//         n2 = tmp;
//     }
// };

// class Solution {
// public:
//     void rotate(vector<int>& nums, int k) {
//         auto first = nums.begin();
//         auto last = nums.end();
//         if (first == last)
//             return;
//         if (k <= 0)
//             return;
//         k = k % (last - first);
//         if (k != 0) {
//             reverse(first, last-k);
//             reverse(last-k, last);
//             reverse(first, last);
//         }

//     }

//     void reverse(vector<int>::iterator first, vector<int>::iterator last) {
//         while (first < last) {
//             iter_swap(first++, --last);
//         }
//     }

// };

// class Solution
// {
// public:
//     void rotate(vector<int> &nums, int k)
//     {
//         auto first = nums.begin();
//         auto last = nums.end();
//         if (first == last)
//             return;
//         if (k <=0)
//             return;
//         k = k % (last - first);
//         if (k)
//             __rotate(first, last-k, last);
//     }

//     void __rotate(vector<int>::iterator first, vector<int>::iterator middle,
//                   vector<int>::iterator last)
//     {
//         for (vector<int>::iterator i = middle;;)
//         {
//             iter_swap(first, i);
//             ++first;
//             ++i;
//             if (first == middle)
//             {
//                 if (i == last)
//                     return;
//                 middle = i;
//             }
//             else if (i == last)
//                 i = middle;
//         }
//     }
// };

class Solution
{
public:
    void rotate(vector<int> &nums, int k)
    {
        auto first = nums.begin();
        auto last = nums.end();
        if (first == last)
            return;
        if (k <= 0)
            return;
        k = k % (last - first);
        if (k)
            __rotate(first, last - k, last);
    }

    ptrdiff_t __gcd(ptrdiff_t m, ptrdiff_t n)
    {
        while (n != 0)
        {
            int t = m % n;
            m = n;
            n = t;
        }
        return m;
    }

    void __rotate_cycle(vector<int>::iterator first, vector<int>::iterator last,
                        vector<int>::iterator initial, int shift)
    {
        int value = *initial;
        vector<int>::iterator ptr1 = initial;
        vector<int>::iterator ptr2 = ptr1 + shift;
        while (ptr2 != initial)
        {
            *ptr1 = *ptr2;
            ptr1 = ptr2;
            if (last - ptr2 > shift)
                ptr2 += shift;
            else
                ptr2 = first + (shift - (last - ptr2));
        }
        *ptr1 = value;
    }

    void __rotate(vector<int>::iterator first, vector<int>::iterator middle,
                  vector<int>::iterator last)
    {
        int n = __gcd(last - first, middle - first);
        while (n--)
            __rotate_cycle(first, last, first + n, middle - first);
    }
};

void print(vector<int> &nums)
{
    for (auto it = nums.begin(); it != nums.end(); it++)
    {
        cout << *it << " ";
    }
    cout << endl;
}
int main()
{
    vector<int> nums{1, 2, 3, 4, 5, 6, 7};
    // vector<int> nums{-1,-100,3,99};
    print(nums);

    Solution s;
    // s.rotate(nums, 3);
    s.rotate(nums, 3);
    print(nums);
}
