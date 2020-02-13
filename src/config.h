//
// Created by zy on 2020/2/1.
//

#ifndef JUDGE_CONFIG_H
#define JUDGE_CONFIG_H

#define MYSQL_USERNAME "root"
#define MYSQL_PASSWORD "zy_151814"

#define SYSTEM_ERROR -1                 //系统错误
#define WAITING 0                       //等待
#define QUEUEING 1                      //队列中
#define COMPILING 2                     //编译中
#define COMPILATION_ERROR  3            //编译错误
#define RUNNING 4                       //运行中
#define RUNTIME_ERROR 5                 //运行错误
#define TIME_LIMIT_EXCEEDED 6           //时间超限
#define MEMORY_LIMIT_EXCEEDED 7         //内存超限
#define OUTPUT_LIMIT_EXCEEDED 8         //输出超限
#define WRONG_ANSWER  9                 //答案错误
#define PRESENTATION_ERROR 10           //格式错误
#define ACCEPT  11                      //答案正确


#define OUTPUT_LIMIT  268435456         //输出大小限制

#define debug(x) std::cout << x << std::endl;

#endif //JUDGE_CONFIG_H
