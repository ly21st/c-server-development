//
// Created by liyuan on 2020/4/3.
//

#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include<unordered_map>
#include <utility>
using namespace std;


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
   typedef unordered_map<int, int> int_map_t;

   vector<int> gridIllumination(int N, vector<vector<int>>& lamps, vector<vector<int>>& queries) {
       int_map_t lines;
       int_map_t cols;
       int_map_t slash;
       int_map_t backslash;
       unordered_set<long long> lamps_set;
       vector<int> result;

       // 把亮着的灯放进一个集合
       for (auto first = lamps.begin(); first != lamps.end(); first++) {
           auto x = (*first)[0];
           auto y = (*first)[1];
           lamps_set.insert(long(N)*x+y);
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

   bool is_ligthed(int N, int_map_t &lines, int_map_t &cols, int_map_t &slash,
                    int_map_t &backslash, int x, int y) {
        if (lines.find(x) != lines.end()) return true;  
        if (cols.find(y) != cols.end()) return true;
        if (slash.find(x+y) != slash.end()) return true;
        if (backslash.find(N-1-(x-y)) != backslash.end()) return true;
        return false;
   }

   void light_one_up_or_off(int N, int_map_t &lines, int_map_t &cols, int_map_t &slash,
                            int_map_t &backslash, int x, int y) {
        lines[x] -= 1;
        if (lines[x] <= 0) lines.erase(x);
        cols[y] -= 1;
        if (cols[y] <= 0) cols.erase(y);
        slash[x+y] -= 1;
        if (slash[x+y] <= 0) slash.erase(x+y);
        backslash[N-1-(x-y)] -= 1;
        if (backslash[N-1-(x-y)] <= 0) backslash.erase(N-1-(x-y));
    }

   void light_off(int N, int_map_t &lines, 
                int_map_t &cols, int_map_t &slash,
                int_map_t &backslash,
                int x, int y,
                unordered_set<long long> &lamps_set) {
       auto size = lamps_set.erase(long(N)*x+y);
       if (size > 0) {
           light_one_up_or_off(N, lines, cols, slash, backslash, x, y);
       }
       for (auto i=0; i < 8; i++) {
           auto ii = x + direction[i][0];
           auto jj = y + direction[i][1];
           if (ii>=0 && ii<N && jj >= 0 && jj<N) {
               auto size = lamps_set.erase(long(N)*ii+jj);
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
    // vector<vector<int>> lamps{{0,0},{4,4}};
    // vector<vector<int>> queries {{1,1},{1,0}};

    vector<vector<int>> lamps{{0,0},{1,0}};
    vector<vector<int>> queries {{1,1},{1,1}};

//     [[0,0],[1,0]]
// [[1,1],[1,1]]
    vector<int> r = s.gridIllumination(5, lamps, queries);
    print(r);
}