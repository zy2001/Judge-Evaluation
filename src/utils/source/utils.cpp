//
// Created by zy on 2020/2/1.
//

#include <io.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <fileapi.h>
#include "../header/utils.h"

int Utils::parseInt(std::string str) {
    if (str.length() > 9) throw -1;
    int res = 0;
    for (auto i : str) {
        if (i < '0' || i > '9') throw -1;
        res = res * 10 + (i - '0');
    }
    return res;
}

int Utils::parseInt(char *str) {
    int len = strlen(str);
    if (len > 9) throw -1;
    int res = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') throw -1;
        res = res * 10 + (str[i] - '0');
    }
    return res;
}

std::string Utils::parseString(int val) {
    std::string res;
    while (val > 0) {
        res += (val % 10 + '0');
        val /= 10;
    }
    std::reverse(res.begin(), res.end());
    return res;
}

std::string Utils::parseString(long long val) {
    std::string res;
    while (val > 0) {
        res += (val % 10 + '0');
        val /= 10;
    }
    std::reverse(res.begin(), res.end());
    return res;
}


void Utils::DeleteAllFiles(std::string path) {
    long hFile = 0;                 // 文件句柄
    struct _finddata_t fileInfo;    // 文件信息
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileInfo)) != -1) {
        do {
            // 修改路径名并删除文件
            p.assign(path).append("/").append(fileInfo.name);
            DeleteFile(p.c_str());
        } while (_findnext(hFile, &fileInfo) == 0);  //寻找下一个，成功返回0，否则-1
        _findclose(hFile);          //关闭句柄
    }
}

std::string Utils::readFile(std::string path) {
    std::ifstream file(path);
    std::ostringstream tmp;
    tmp << file.rdbuf();
    std::string str = tmp.str();
    return str;
}

std::string &Utils::ignoreLineEnd(std::string &str) {
    int cut = 0;
    for (cut = str.length() - 1; cut >= 0; --cut) {
        if (str[cut] != '\n') break;
    }
//    debug(str);
    return str.erase(cut + 1, str.length());
}


