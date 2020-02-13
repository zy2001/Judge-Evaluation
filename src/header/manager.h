//
// Created by zy on 2020/2/1.
//

#ifndef JUDGE_MANAGER_H
#define JUDGE_MANAGER_H

#include <queue>
#include <string>
#include "domain.h"

class Manager {
private:
    std::queue<JudgeItem> ItemsQueue;
public:

    void run();


    void judge(JudgeItem &judgeItem);


    JudgeItem getJudgeItem();

};


#endif //JUDGE_MANAGER_H
