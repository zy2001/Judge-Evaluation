//
// Created by zy on 2020/2/1.
//

#ifndef JUDGE_UTILS_H
#define JUDGE_UTILS_H

#include <string>

class Utils
{
public:

    /**
     * c++字符串类型转int型
     * @param str - c++字符串
     * @return c++字符串的int形式
     */
    static int parseInt(std::string str);

    /**
     * c字符串类型转int型
     * @param str - c字符串
     * @return c字符串的int形式
     */
    static int parseInt(char* str);

    /**
     * int类型转字符串型
     * @param val - 源数字
     * @return 数字的字符串形式
     */
    static std::string parseString(int val);

    /**
     * long long类型转字符串型
     * @param val - 源数字
     * @return 数字的字符串形式
     */
    static std::string parseString(long long val);

    /**
     * 读文件内容
     * @param path  - 文件路径
     * @return 文件内容
     */
    static std::string readFile(std::string path);

    /**
     * 删除目录下所有文件
     * @param path  - 目录路径
     */
    static void DeleteAllFiles(std::string path);

    /**
     * 忽略结尾换行符
     * @param str   - 源字符串
     * @return 忽略换行符后的字符串
     */
    static std::string& ignoreLineEnd(std::string &str);
};

#endif //JUDGE_UTILS_H
