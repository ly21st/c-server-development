//
// Created by liyuan on 2020/4/2.
//
/**
 * 1001. 网格照明
难度
困难

15

收藏

分享
切换为英文
关注
反馈
在 N x N 的网格上，每个单元格 (x, y) 上都有一盏灯，其中 0 <= x < N 且 0 <= y < N 。

最初，一定数量的灯是亮着的。lamps[i] 告诉我们亮着的第 i 盏灯的位置。每盏灯都照亮其所在 x 轴、y 轴和两条对角线上的每个正方形（类似于国际象棋中的皇后）。

对于第 i 次查询 queries[i] = (x, y)，如果单元格 (x, y) 是被照亮的，则查询结果为 1，否则为 0 。

在每个查询 (x, y) 之后 [按照查询的顺序]，我们关闭位于单元格 (x, y) 上或其相邻 8 个方向上（与单元格 (x, y) 共享一个角或边）的任何灯。

返回答案数组 answer。每个值 answer[i] 应等于第 i 次查询 queries[i] 的结果。



示例：

输入：N = 5, lamps = [[0,0],[4,4]], queries = [[1,1],[1,0]]
输出：[1,0]
解释：
在执行第一次查询之前，我们位于 [0, 0] 和 [4, 4] 灯是亮着的。
表示哪些单元格亮起的网格如下所示，其中 [0, 0] 位于左上角：
1 1 1 1 1
1 1 0 0 1
1 0 1 0 1
1 0 0 1 1
1 1 1 1 1
然后，由于单元格 [1, 1] 亮着，第一次查询返回 1。在此查询后，位于 [0，0] 处的灯将关闭，网格现在如下所示：
1 0 0 0 1
0 1 0 0 1
0 0 1 0 1
0 0 0 1 1
1 1 1 1 1
在执行第二次查询之前，我们只有 [4, 4] 处的灯亮着。现在，[1, 0] 处的查询返回 0，因为该单元格不再亮着。


提示：

1 <= N <= 10^9
0 <= lamps.length <= 20000
0 <= queries.length <= 20000
lamps[i].length == queries[i].length == 2
 */

#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <utility>
using namespace std;

//class Solution {
//public:
//    int direction[8][2] = {
//                {-1, -1},
//                {-1, 0},
//                {-1, 1},
//                {0, -1},
//                {0, 1},
//                {1, -1},
//                {1, 0},
//                {1, 1}
//        };
//
//    vector<int> gridIllumination(int N, vector<vector<int>>& lamps, vector<vector<int>>& queries) {
//        set<vector<int>> lamps_set;
//        vector<int> result;
//        for (auto first = lamps.begin(); first != lamps.end(); first++) {
//            lamps_set.insert(*first);
//        }
//        auto queries_size = queries.size();
//        for (auto m = 0; m < queries_size; m++) {
//            auto i = queries[m][0];
//            auto j = queries[m][1];
//            auto b = is_ligthed(lamps_set, i, j);
//            if (b) {
//                result.push_back(1);
//            } else {
//                result.push_back(0);
//            }
//            if (m != queries_size-1) {
//                light_off(N, lamps_set, i, j);
//            }
//        }
//        return result;
//    }
//
//    bool is_ligthed(set<vector<int>>& lamps, int x, int y) {
//        for (auto first = lamps.begin(); first != lamps.end(); first++) {
//            auto i = (*first)[0];
//            auto j = (*first)[1];
//            if (x == i || y == j || (y-j)==(x-i) || (y-j)== (i-x)) {
//                return true;
//            }
//        }
//        return false;
//    }
//
//    void light_off(int N, set<vector<int>> &lamps_set, int x, int y) {
//        lamps_set.erase(vector<int>{x, y});
//        for (auto i=0; i < 8; i++) {
//            auto ii = x + direction[i][0];
//            auto jj = y + direction[i][1];
//            if (ii>=0 && ii<N && jj >= 0 && jj<N) {
//                lamps_set.erase(vector<int>{ii, jj});
//            }
//        }
//    }
//};





// struct VectorHash {
//     size_t operator()(const std::vector<int>& v) const {
//         std::hash<int> hasher;
//         size_t seed = 0;
//         for (int i : v) {
//             seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
//         }
//         return seed;
//     }
// };

// class Solution {
// public:
//     int direction[8][2] = {
//                 {-1, -1},
//                 {-1, 0},
//                 {-1, 1},
//                 {0, -1},
//                 {0, 1},
//                 {1, -1},
//                 {1, 0},
//                 {1, 1}
//         };
//     vector<int> gridIllumination(int N, vector<vector<int>>& lamps, vector<vector<int>>& queries) {
//         unordered_set<vector<int>, VectorHash> lamps_set;
//         vector<int> result;
//         for (auto first = lamps.begin(); first != lamps.end(); first++) {
//             lamps_set.insert(*first);
//         }
//         auto queries_size = queries.size();
//         for (auto m = 0; m < queries_size; m++) {
//             auto i = queries[m][0];
//             auto j = queries[m][1];
//             auto b = is_ligthed(lamps_set, i, j);
//             if (b) {
//                 result.push_back(1);
//             } else {
//                 result.push_back(0);
//             }
//             if (m != queries_size-1) {
//                 light_off(N, lamps_set, i, j);
//             }
//         }
//         return result;
//     }

//     bool is_ligthed(unordered_set<vector<int>, VectorHash>& lamps, int x, int y) {
//         for (auto first = lamps.begin(); first != lamps.end(); first++) {
//             auto i = (*first)[0];
//             auto j = (*first)[1];
//             if (x == i || y == j || (y-j)==(x-i) || (y-j)== (i-x)) {
//                 return true;
//             }
//         }
//         return false;
//     }

//     void light_off(int N, unordered_set<vector<int>, VectorHash> &lamps_set, int x, int y) {
//         lamps_set.erase(vector<int>{x, y});

//         for (auto i=0; i < 8; i++) {
//             auto ii = x + direction[i][0];
//             auto jj = y + direction[i][1];
//             if (ii>=0 && ii<N && jj >= 0 && jj<N) {
//                 lamps_set.erase(vector<int>{ii, jj});
//             }
//         }
//     }
// };



struct VectorHash {
   size_t operator()(const std::vector<int>& v) const {
       std::hash<int> hasher;
       size_t seed = 0;
       for (int i : v) {
           seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
       }
       return seed;
   }
};

class Solution {
public:
   int direction[8][2] = {
           {-1, -1},
           {-1, 0},
           {-1, 1},
           {0, -1},
           {0, 1},
           {1, -1},
           {1, 0},
           {1, 1}
   };

   vector<int> gridIllumination(int N, vector<vector<int>>& lamps, vector<vector<int>>& queries) {
       vector<vector<int>> vec(N, vector<int>(N, 0));
       unordered_set<vector<int>, VectorHash> lamps_set;
       vector<int> result;

       // 把亮着的灯放进一个集合
       for (auto first = lamps.begin(); first != lamps.end(); first++) {
           lamps_set.insert(*first);
       }
       // 点亮所有灯
       light_up(N, vec, lamps, true);
       print(N, vec);

       auto queries_size = queries.size();
       for (auto m = 0; m < queries_size; m++) {
           auto i = queries[m][0];
           auto j = queries[m][1];
           auto b = is_ligthed(N, vec, i, j);
           if (b) {
               result.push_back(1);
           } else {
               result.push_back(0);
           }
           if (m != queries_size-1) {
               light_off(N, vec, lamps_set, i, j);
           }
       }
       return result;
   }

   void light_up(int N, vector<vector<int>> &vec, vector<vector<int>>& lamps, bool up_flag) {
       for (auto first = lamps.begin(); first != lamps.end(); first++) {
           auto i = (*first)[0];
           auto j = (*first)[1];
           light_one_up_or_off(N, vec, i, j, true);
       }
   }

   void light_one_up_or_off(int N, vector<vector<int>> &vec, int i, int j, bool up_flag) {
       int n;
       if (up_flag) {
           n = 1;
       } else {
           n = -1;
       }
       vec[i][j] += n;
       // 点亮同一行的所有灯
       for (auto jj = 0; jj < N; jj++) {
           if (jj != j)
               vec[i][jj] += n;
       }
       // 点亮同一列的所有灯
       for (auto ii=0; ii < N; ii++) {
           if (ii != i) {
               vec[ii][j] += n;
           }
       }
       //
       for (auto ii=i-1, jj=j-1; ii>=0 && jj>=0; ii--, jj--) {
           vec[ii][jj] += n;
       }
       for (auto ii=i+1, jj=j+1; ii<N && jj<N; ii++, jj++) {
           vec[ii][jj] += n;
       }
       for (auto ii=i-1, jj=j+1; ii>=0 && jj<N; ii--, jj++) {
           vec[ii][jj] += n;
       }
       for (auto ii=i+1, jj=j-1; ii<N && jj>=0; ii++, jj--) {
           vec[ii][jj] += n;
       }
   }

   bool is_ligthed(int N, vector<vector<int>> &vec, int x, int y) {
       if (vec[x][y] > 0) return true;
       else return false;
   }

   void light_off(int N, vector<vector<int>> &vec, unordered_set<vector<int>, VectorHash> &lamps_set, int x, int y) {
       auto size = lamps_set.erase(vector<int>{x, y});
       if (size > 0) {
           light_one_up_or_off(N, vec, x, y, false);
       }
       for (auto i=0; i < 8; i++) {
           auto ii = x + direction[i][0];
           auto jj = y + direction[i][1];
           if (ii>=0 && ii<N && jj >= 0 && jj<N) {
               auto size = lamps_set.erase(vector<int>{ii, jj});
               if (size > 0) {
                   light_one_up_or_off(N, vec, ii, jj, false);
               }
           }
       }
   }

   void print(int N, vector<vector<int>> &vec) {
       for(auto i = 0; i < N; i++) {
           for (auto j = 0; j < N; j++) {
               cout<<vec[i][j]<< " ";
           }
           cout<<endl;
       }
   }
};





void print(vector<int>& vec) {
    for (auto first = vec.begin(); first != vec.end(); first++) {
        cout << *first << " ";
    }
    cout << endl;
}

int main() {
    Solution s;
    vector<vector<int>> lamps{{0,0},{4,4}};
    vector<vector<int>> queries {{1,1},{1,0}};
    vector<int> r = s.gridIllumination(5, lamps, queries);
    print(r);
}
