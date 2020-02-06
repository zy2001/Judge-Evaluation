//
// Created by zy on 2020/1/31.
//
#include <iostream>
#include <io.h>
#include "src/database/header/connect.h"
#include "src/manager/header/manager.h"
#include "src/judge/header/judgecore.h"
#include "src/config.h"


int main() {
    //初始化数据库连接
    Connect::init();
    //初始化管理器
    Manager manager;
    //运行管理器
    manager.run();
    //关闭数据库连接

//    debug(JudgeCore::getOutputFileUsage(runId, caseNumber));
//    debug(JudgeCore::answerCompare("1","1","1"));

    Connect::close();
    std::cout << "Server End!" << std::endl;
    return 0;
}

