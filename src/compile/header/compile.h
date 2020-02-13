//
// Created by zy on 2020/1/31.
//

#ifndef JUDGE_COMPILE_H
#define JUDGE_COMPILE_H
#include <string>
#include "../../manager/header/manager.h"

class Compiler
{
public:
    static int compile(std::string runId, std::string pid, std::string language);

    static int compile(JudgeItem &judgeItem, JudgeStatus &judgeStatus);

    static int compile_cpp(std::string runId, std::string pid);

//    static bool saveCompilationMessage(std::string runId);
};


#endif //JUDGE_COMPILE_H
