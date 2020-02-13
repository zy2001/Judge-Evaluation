//
// Created by zy on 2020/1/31.
//

#ifndef JUDGE_COMPILE_H
#define JUDGE_COMPILE_H

#include <string>
#include "domain.h"
#include "manager.h"


class Compiler {
public:

    static int compile(JudgeItem &judgeItem, JudgeStatus &judgeStatus);

    static int compile_cpp(std::string runId, std::string pid);

};


#endif //JUDGE_COMPILE_H
