//
// Created by liyuan on 2020/3/31.
//

// 最长公共前缀
/**
    编写一个函数来查找字符串数组中的最长公共前缀。

    如果不存在公共前缀，返回空字符串 ""。

    示例 1:

    输入: ["flower","flow","flight"]
    输出: "fl"
    示例 2:

    输入: ["dog","racecar","car"]
    输出: ""
    解释: 输入不存在公共前缀。
    说明:

    所有输入只包含小写字母 a-z 。

    来源：力扣（LeetCode）
    链接：https://leetcode-cn.com/problems/longest-common-prefix
    著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。
 */

#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        size_t j = 0;
        auto size = strs.size();
        if (size < 1) return "";
        vector<char> result;
        size_t first_len = strs[0].size();
        for (j = 0; j < first_len; j++) {
            auto c = strs[0][j];
            for (auto i = 1; i < strs.size(); i++) {
                if (j >= strs[i].size() || c != strs[i][j]) {
                    return string(result.begin(), result.end());
                }
            }
            result.push_back(c);
        }
        return string(result.begin(), result.end());
    }
};

int main() {
    vector<string> strs{"flower","flow","flight"};
//    vector<string> strs{"dog","racecar","car"};
    Solution s;
    string r = s.longestCommonPrefix(strs);
    cout << r << endl;
}