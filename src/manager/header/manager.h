//
// Created by zy on 2020/2/1.
//

#ifndef JUDGE_MANAGER_H
#define JUDGE_MANAGER_H

#include <string>
#include <mysql.h>
#include <queue>

class JudgeItem {
private:
    std::string rid;
    std::string pid;
//    std::string uid;
    std::string code;
    std::string language;
//    int status;
//    int runtime;
//    int runMemory;
//     gmtCreated;
public:
    std::string getRid() {
        return this->rid;
    }

    void setRid(std::string rid) {
        this->rid = rid;
    }

    std::string getPid() {
        return this->pid;
    }

    void setPid(std::string pid) {
        this->pid = pid;
    }

    const std::string getCode() {
        return this->code;
    }

    void setCode(std::string code) {
        this->code = code;
    }

    const std::string getLanguage() {
        return this->language;
    }

    void setLanguage(std::string language) {
        this->language = language;
    }
};

class Manager {
private:
    std::queue<JudgeItem> ItemsQueue;
public:
    /**
     * 获取未进行Judge的提交信息
     * @return void
     */
    void getItems();

    void run();

    /**
     * 获取题目时间限制、内存限制、测试样例数
     * @param runId         - 子进程STARTUPINFO结构体
     * @param pid           - 运行编号
     * @param timeLimit     - 题目编号
     * @param memoryLimit   - 题目编号
     * @param caseCount     - 题目编号
     * @return 获取信息是否成功
     */
    bool getProblemDetail(const std::string &runId, const std::string &pid,  int &timeLimit,  int &memoryLimit, int &caseCount);

};


#endif //JUDGE_MANAGER_H
