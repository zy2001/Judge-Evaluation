//
// Created by zy on 2020/2/1.
//

#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>
#include "../config.h"
#include "../header/manager.h"
#include "../header/utils.h"
#include "../header/judgecore.h"
#include "../header/compile.h"
#include "../header/rabbitmq.h"


bool flag = true;

void handle(int signal) {
    std::cout << "End End End !" << std::endl;
    flag = false;
    Sleep(2000);
}

void Manager::run() {
    signal(SIGINT, handle);
    while (flag) {
        JudgeItem judgeItem = getJudgeItem();
        if (judgeItem.getRid() != "-1") {
            judge(judgeItem);
        }
    }
}

void Manager::judge(JudgeItem &judgeItem) {
    // 更新状态为队列中
    JudgeStatus judgeStatus(Utils::parseInt(judgeItem.getRid()), QUEUEING);
    RabbitMQ::sendMessage(Utils::parseJSON(judgeStatus));
    // 创建测试文件夹
    std::string folderPath = "../data/" + judgeItem.getRid();
    bool create = CreateDirectory(folderPath.c_str(), nullptr);
    // 创建源程序文件
    std::ofstream file;
    // 以写入和在文件末尾添加的方式打开.cpp文件，没有的话就创建该文件。
    file.open("../data/" + judgeItem.getRid() + "/main.cpp", std::ios::out | std::ios::app);
    if (file.is_open()) {
        file << judgeItem.getCode();
    }
    file.close();
    int resultStatus = SYSTEM_ERROR;      // 评测结果代码
    // 编译源代码
    judgeStatus.setStatus(COMPILING);
    RabbitMQ::sendMessage(Utils::parseJSON(judgeStatus));
    int compilationResult = Compiler::compile(judgeItem, judgeStatus);
    if (compilationResult != 0) {
        // 编译错误
        printf("Compile Error!\n");
        resultStatus = COMPILATION_ERROR;
    } else {
        // 更新状态为正在运行
        judgeStatus.setStatus(RUNNING);
        RabbitMQ::sendMessage(Utils::parseJSON(judgeStatus));
        // 编译成功，运行源程序
        resultStatus = JudgeCore::run(judgeItem, judgeStatus);
        std::cout << "Judge Finished!" << this->ItemsQueue.size() << std::endl;
    }
    debug(resultStatus);
    // 更新评测结果
    judgeStatus.setStatus(resultStatus);
    RabbitMQ::sendMessage(Utils::parseJSON(judgeStatus));
    // 删除目录下所有文件
    Utils::DeleteAllFiles(folderPath);
    // 删除目录
    RemoveDirectory(folderPath.c_str());
}

JudgeItem Manager::getJudgeItem() {
    JudgeItem judgeItem;
    while (flag) {
        char *message = RabbitMQ::read_message();;
        if (message != nullptr) {
            std::cout << "[Receive] The result is : " << message << std::endl;
            judgeItem = Utils::parseJudgeItem(message);
            delete[] message;
            break;
        } else {
            std::cout << "Empty Queue" << std::endl;
//            RabbitMQ::close();
//            RabbitMQ::connect();
            Sleep(1000);
        }
    }
    return judgeItem;
}


