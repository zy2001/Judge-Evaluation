//
// Created by zy on 2020/2/1.
//

#ifndef JUDGE_UTILS_H
#define JUDGE_UTILS_H

#include <string>

class Utils
{
public:
    static int parseInt(std::string str);

    static int parseInt(char* str);

    static std::string parseString(int val);

    static std::string parseString(long long val);

    static std::string readFile(std::string path);

    /**
     * 删除目录下所有文件
     * @param path  - 目录路径
     */
    static void DeleteAllFiles(std::string path);
};

#endif //JUDGE_UTILS_H
