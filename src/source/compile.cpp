//
// Created by zy on 2020/1/31.
//

#include <string>
#include <iostream>
#include "../header/compile.h"
#include "../header/utils.h"


int Compiler::compile_cpp(std::string runId, std::string pid) {
    std::string ord = "cd ../data/" + runId + " & g++ -Wall -g -O2 main.cpp -o main.exe 2>compile.log";
    std::cout << ord << std::endl;
    return system(ord.c_str());
}

int Compiler::compile(JudgeItem &judgeItem, JudgeStatus &judgeStatus) {
    int result = -1;
    if ("C++" == judgeItem.getLanguage()) {
        result = compile_cpp(judgeItem.getRid(), judgeItem.getPid());
    }
    std::string str = Utils::readFile("../data/" + judgeItem.getRid() + "/compile.log");
    judgeStatus.setCompilationMessage(str);
    return result;
}

