//
// Created by zy on 2020/2/1.
//

#ifndef JUDGE_CONFIG_H
#define JUDGE_CONFIG_H

#define PATH "www"
#define MYSQL_USERNAME "root"
#define MYSQL_PASSWORD "zy_151814"

#define SYSTEM_ERROR -1             //系统错误
#define QUEUE 0	                    //队列中
#define COMPILING 1	                //编译中
#define COMPILATION_ERROR  2	    //编译错误
#define COMPILATION_SUCCESS  3	    //编译成功
#define RUNNING 4	                //运行中
#define RUNTIME_ERROR 5
#define TIME_LIMIT_EXCEEDED 6	    //时间超限
#define MEMORY_LIMIT_EXCEEDED 7	    //内存超限
#define WRONG_ANSWER  8	            //答案错误
#define PRESENTATION_ERROR 9	    //格式错误
#define ACCEPT  10	                //答案正确

#define debug(x) std::cout << x << std::endl;

#endif //JUDGE_CONFIG_H
