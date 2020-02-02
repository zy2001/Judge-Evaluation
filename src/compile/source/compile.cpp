//
// Created by zy on 2020/1/31.
//

#include <string>
#include <iostream>

#include "../header/compile.h"
#include "../../utils/header/utils.h"
#include "../../database/header/connect.h"

int Compiler::compile(std::string runId, std::string pid, std::string language) {
    int result = 0;
    if ("C++" == language) {
        result = compile_cpp(runId, pid);
    }
    saveCompilationMessage(runId);
    return result;
}

int Compiler::compile_cpp(std::string runId, std::string pid) {
    std::string ord = "cd ../data/" + runId + " & g++ -o main.exe main.cpp 2>compile.log";
    std::cout << ord << std::endl;
    return system(ord.c_str());
}

bool Compiler::saveCompilationMessage(std::string runId) {
    std::string str = Utils::readFile("../data/" + runId + "/compile.log");
    std::string compilationMessage = "";
    for (auto i : str) {
        compilationMessage += i;
        if (i == '\'') compilationMessage += i;
    }
    std::string sql = "UPDATE SUBMIT SET COMPILATION_MESSAGE = '" + compilationMessage + "' WHERE RID = " + runId;
    std::cout << sql << std::endl;
    bool res = Connect::mysql_update(sql.c_str());
    return res;
}

