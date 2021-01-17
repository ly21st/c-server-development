//
// Created by liyuan on 2020/4/3.
//

#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <utility>
using namespace std;

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
       vector<int> lines(N, 0);
       vector<int> cols(N, 0);
       vector<int> slash(2*N-1, 0);
       vector<int> backslash(2*N-1, 0);
       unordered_set<vector<int>, VectorHash> lamps_set;
       vector<int> result;

       // 把亮着的灯放进一个集合
       for (auto first = lamps.begin(); first != lamps.end(); first++) {
           lamps_set.insert(*first);
           auto x = (*first)[0];
           auto y = (*first)[1];
           lines[x] += 1;
           cols[y] += 1;
           slash[x+y] += 1;
           backslash[N-1-(x-y)] += 1;
       }

       auto queries_size = queries.size();
       for (auto m = 0; m < queries_size; m++) {
           auto i = queries[m][0];
           auto j = queries[m][1];
           auto b = is_ligthed(N, lines, cols, slash, backslash, i, j);
           if (b) {
               result.push_back(1);
           } else {
               result.push_back(0);
           }
           if (m != queries_size-1) {
               light_off(N, lines, cols, slash, backslash, i, j, lamps_set);
           }
       }
       return result;
   }

   bool is_ligthed(int N, vector<int> &lines, vector<int> &cols, vector<int> &slash,
                    vector<int> &backslash, int x, int y) {
        if (lines[x] > 0) return true;
        if (cols[y] > 0) return true;
        if (slash[x+y] > 0) return true;
        if (backslash[N-1-(x-y)] > 0) return true;
        return false;
   }

   void light_one_up_or_off(int N, vector<int> &lines, vector<int> &cols, vector<int> &slash,
                            vector<int> &backslash, int x, int y) {
        lines[x] -= 1;
        cols[y] -= 1;
        slash[x+y] -= 1;
        backslash[N-1-(x-y)] -= 1;
    }

   void light_off(int N, vector<int> &lines, 
                vector<int> &cols, vector<int> &slash,
                vector<int> &backslash,
                int x, int y,
                unordered_set<vector<int>, VectorHash> &lamps_set) {
       auto size = lamps_set.erase(vector<int>{x, y});
       if (size > 0) {
           light_one_up_or_off(N, lines, cols, slash, backslash, x, y);
       }
       for (auto i=0; i < 8; i++) {
           auto ii = x + direction[i][0];
           auto jj = y + direction[i][1];
           if (ii>=0 && ii<N && jj >= 0 && jj<N) {
               auto size = lamps_set.erase(vector<int>{ii, jj});
               if (size > 0) {
                   light_one_up_or_off(N, lines, cols, slash, backslash, ii, jj);
               }
           }
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