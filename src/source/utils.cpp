//
// Created by zy on 2020/2/1.
//

#include <io.h>
#include <fstream>
#include <sstream>
#include <writer.h>
#include <iostream>
#include <algorithm>
#include <fileapi.h>
#include <document.h>
#include <stringbuffer.h>
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
    if (val == 0) { return "0"; }
    bool flag = false;
    if (val < 0) {
        flag = true;
        val *= -1;
    }
    std::string res;
    while (val > 0) {
        res += (val % 10 + '0');
        val /= 10;
    }
    if (flag) res += "-";
    std::reverse(res.begin(), res.end());
    return res;
}

std::string Utils::parseString(long long val) {
    if (val == 0) { return "0"; }
    bool flag = false;
    if (val < 0) {
        flag = true;
        val *= -1;
    }
    std::string res;
    while (val > 0) {
        res += (val % 10 + '0');
        val /= 10;
    }
    if (flag) res += "-";
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
    file.close();
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


JudgeItem Utils::parseJudgeItem(char *str) {
    JudgeItem judgeItem;
    rapidjson::Document document;
    if (!document.Parse(str).HasParseError()) {
        if (document.HasMember("rid") && document["rid"].IsInt()) {
            judgeItem.setRid(parseString(document["rid"].GetInt()));
        }
        if (document.HasMember("pid") && document["pid"].IsInt()) {
            judgeItem.setPid(parseString(document["pid"].GetInt()));
        }
        if (document.HasMember("language") && document["language"].IsString()) {
            judgeItem.setLanguage(document["language"].GetString());
        }
        if (document.HasMember("code") && document["code"].IsString()) {
            judgeItem.setCode(document["code"].GetString());
        }
        if (document.HasMember("timeLimit") && document["rid"].IsInt()) {
            judgeItem.setTimeLimit(document["timeLimit"].GetInt());
        }
        if (document.HasMember("memoryLimit") && document["rid"].IsInt()) {
            judgeItem.setMemoryLimit(document["memoryLimit"].GetInt());
        }
        if (document.HasMember("caseCount") && document["rid"].IsInt()) {
            judgeItem.setCaseCount(document["caseCount"].GetInt());
        }
        judgeItem.setStatus(1);
    }
    return judgeItem;
}

std::string Utils::parseJSON(JudgeStatus &judgeStatus) {
    using namespace rapidjson;

    Document document;
    document.SetObject();
    Document::AllocatorType &allocator = document.GetAllocator();
    //null值
    Value null(kNullType);
    null.SetNull();
    //根Object
    Value root(kObjectType);
    //rid
    root.AddMember("rid", judgeStatus.getRid(), allocator);
    //status
    root.AddMember("status", judgeStatus.getStatus(), allocator);
    //runTime
    if (judgeStatus.getRunTime() != -1) {
        root.AddMember("runTime", judgeStatus.getRunTime(), allocator);
    } else {
        root.AddMember("runTime", null, allocator);
    }
    //runMemory
    if (judgeStatus.getRunMemory() != -1) {
        root.AddMember("runMemory", judgeStatus.getRunMemory(), allocator);
    } else {
        root.AddMember("runMemory", null, allocator);
    }
    //compilationMessage
    Value compilationMessage(kStringType);
    compilationMessage.SetString(judgeStatus.getCompilationMessage().c_str(),
                                 judgeStatus.getCompilationMessage().size(), allocator);
    root.AddMember("compilationMessage", compilationMessage, allocator);
    StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);
    root.Accept(writer);
    return strBuf.GetString();
}


