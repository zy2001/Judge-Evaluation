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
    std::string code;
    std::string language;
    int status;
    int timeLimit;
    int memoryLimit;
    int caseCount;
public:
    JudgeItem() {
        this->rid = "-1";
    }
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

    std::string getCode() {
        return this->code;
    }

    void setCode(std::string code) {
        this->code = code;
    }

    std::string getLanguage() {
        return this->language;
    }

    void setLanguage(std::string language) {
        this->language = language;
    }

    int getStatus() {
        return this->status;
    }

    void setStatus(int status) {
        this->status = status;
    }

    int getTimeLimit() {
        return this->timeLimit;
    }

    void setTimeLimit(int timeLimit) {
        this->timeLimit = timeLimit;
    }

    int getMemoryLimit() {
        return this->memoryLimit;
    }

    void setMemoryLimit(int memoryLimit) {
        this->memoryLimit = memoryLimit;
    }

    int getCaseCount() {
        return this->caseCount;
    }

    void setCaseCount(int caseCount) {
        this->caseCount = caseCount;
    }
};

class JudgeStatus {
private:
    int rid;
    int status;
    int runTime;
    int runMemory;
    std::string compilationMessage;
public:
    JudgeStatus() {
        this->rid = -1;
        this->status = -1;
        this->runTime = -1;
        this->runMemory = -1;
        this->compilationMessage = "";
    }

    JudgeStatus(int rid, int status) : rid(rid), status(status) {
        this->runTime = -1;
        this->runMemory = -1;
        this->compilationMessage = "";
    }
    int getRid() {
        return this->rid;
    }

    void setRid(int rid) {
        this->rid = rid;
    }

    int getStatus() {
        return this->status;
    }

    void setStatus(int status) {
        this->status = status;
    }

    int getRunTime() {
        return this->runTime;
    }

    void setRunTime(int runTime) {
        this->runTime = runTime;
    }

    int getRunMemory() {
        return this->runMemory;
    }

    void setRunMemory(int runMemory) {
        this->runMemory = runMemory;
    }

    std::string getCompilationMessage() {
        return this->compilationMessage;
    }

    void setCompilationMessage(std::string compilationMessage) {
        this->compilationMessage = compilationMessage;
    }
};

class Manager {
private:
    std::queue<JudgeItem> ItemsQueue;
public:

    void run();


    void judge(JudgeItem &judgeItem);


    JudgeItem getJudgeItem();

};


#endif //JUDGE_MANAGER_H
