//
// Created by zy on 2020/2/1.
//

#include <iostream>
#include <fstream>
#include "../../config.h"
#include "../header/manager.h"
#include "../../utils/header/utils.h"

#include "../../judge/header/judgecore.h"
#include "../../compile/header/compile.h"
#include "../../database/header/connect.h"

void Manager::getItems() {
    Sleep(1000);
    if (this->ItemsQueue.empty()) {
        std::string sql = "SELECT RID, PID, CODE, LANGUAGE FROM SUBMIT WHERE STATUS = 0";
        MYSQL_RES *result = Connect::mysql_select(sql);
        if (result != nullptr) {
            MYSQL_ROW row = nullptr;
            row = mysql_fetch_row(result);
            while (nullptr != row) {
                JudgeItem judgeItem;
                judgeItem.setRid(row[0]);
                judgeItem.setPid(row[1]);
                judgeItem.setCode(row[2]);
                judgeItem.setLanguage(row[3]);
                this->ItemsQueue.push(judgeItem);
                row = mysql_fetch_row(result);
            }
        }
    }
}

bool flag = true;

void handle(int signal) {
    std::cout << "End End End !" << std::endl;
    flag = false;
    Sleep(2000);

}


void Manager::run() {
    signal(SIGINT, handle);
    while (flag) {
        getItems();
        while (this->ItemsQueue.size()) {
            //取被测数据
            JudgeItem judgeItem = this->ItemsQueue.front();
            this->ItemsQueue.pop();
            //创建测试文件夹及源程序文件
            std::string folderPath = "../data/" + judgeItem.getRid();
            bool create = CreateDirectory(folderPath.c_str(), 0);
            std::ofstream file;
            //以写入和在文件末尾添加的方式打开.cpp文件，没有的话就创建该文件。
            file.open("../data/" + judgeItem.getRid() + "/main.cpp", std::ios::out | std::ios::app);
            if (file.is_open()) {
                file << judgeItem.getCode();
            }
            file.close();
            int resultStatus = SYSTEM_ERROR;      //评测结果代码
            int timeLimit = 0, memoryLimit = 0, caseCount = 0;
            if (getProblemDetail(judgeItem.getRid(), judgeItem.getPid(), timeLimit, memoryLimit, caseCount)) {
                //获取题目信息成功
                //编译源代码
                int compilationResult = Compiler::compile(judgeItem.getRid(), judgeItem.getPid(),
                                                          judgeItem.getLanguage());
                if (compilationResult != 0) {
                    //编译错误
                    printf("Compile Error!\n");
                    resultStatus = COMPILATION_ERROR;
                } else {
                    //编译成功
                    //运行源程序
                    resultStatus = JudgeCore::run(judgeItem.getRid(), judgeItem.getPid(), timeLimit, memoryLimit,
                                                  caseCount);
                    std::cout << "Judge Finished!" << this->ItemsQueue.size() << std::endl;
                }
            }
            debug(resultStatus);
            //更新评测结果
            std::string sql = "UPDATE SUBMIT SET STATUS = " + Utils::parseString(resultStatus) + " WHERE RID = " +
                              judgeItem.getRid();
            debug(sql);
            Connect::mysql_update(sql.c_str());
            //删除目录下所有文件
            Utils::DeleteAllFiles(folderPath);
            //删除目录
            RemoveDirectory(folderPath.c_str());
        }

    }


}

bool Manager::getProblemDetail(const std::string &runId, const std::string &pid, int &timeLimit, int &memoryLimit,
                               int &caseCount) {
    std::string sql = "SELECT TIME_LIMIT, MEMORY_LIMIT, CASE_COUNT FROM PROBLEM WHERE PID = " + pid;
    MYSQL_RES *result = Connect::mysql_select(sql);
    if (nullptr == result) {
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(result);
    if (nullptr != row) {
        try {
            if (nullptr != row[0]) timeLimit = Utils::parseInt(row[0]);
            else return false;
            if (nullptr != row[1]) memoryLimit = Utils::parseInt(row[1]);
            else return false;
            if (nullptr != row[2]) caseCount = Utils::parseInt(row[2]);
            else return false;
        } catch (int val) {
            return false;
        }
    } else {
        return false;
    }
    std::cout << "timeLimit:" << timeLimit << "\nmemoryLimit:" << memoryLimit << "\ncaseCount:" << caseCount
              << std::endl;
    return true;
}
