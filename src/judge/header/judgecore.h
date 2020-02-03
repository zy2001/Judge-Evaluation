//
// Created by zy on 2020/2/1.
//

#ifndef JUDGE_JUDGECORE_H
#define JUDGE_JUDGECORE_H

#include <string>
#include <windows.h>

class JudgeCore{
public:

    /**
     * 运行程序，检测资源使用
     * @param runId         - 运行编号
     * @param pid           - 题目编号
     * @param timeLimit     - 时间限制
     * @param memoryLimit   - 内存限制
     * @param caseCount     -测试样例数目
     * @return 运行结果编码
     */
    static int run(const std::string &runId, const std::string &pid, int timeLimit, int memoryLimit, int caseCount);

    /**
     * 重定向子进程IO
     * @param si    - 子进程STARTUPINFO结构体
     * @param runid - 运行编号
     * @param pid   - 题目编号
     * @return 重定向是否成功
     */
    static bool setUpIORedirection(STARTUPINFO &si, std::string runId, std::string pid, std::string caseNum);

    /**
     * 创建子进程
     * @param si    - 子进程STARTUPINFO结构体
     * @param pi    - 子进程PROCESS_INFOMATION结构体
     * @param runid - 运行编号
     */
    static bool createProcess(STARTUPINFO &si, PROCESS_INFORMATION &pi, std::string runId);

    /**
     * 运行子进程
     * @param pi          - 子进程信息结构体
     * @param timeUsage   - 运行时间使用(ms)
     * @param memoryUsage - 运行内存使用(KB)
     * * @param timeLimit   - 运行时间限制(ms)
     * @param memoryLimit - 运行空间限制(KB)
     * @return 进程退出代码
     */
    static DWORD runProcess(PROCESS_INFORMATION &pi, long long &memoryUsage, long long &timeUsage, long long timeLimit, long long memoryLimit);

    /**
     * 获取内存占用最大值
     * @param pi          - 子进程信息结构体
     * @param memoryLimit - 运行空间限制(KB)
     * @return 最大内存使用量
     */
    static long long getMaxMemoryUsage(PROCESS_INFORMATION &pi, long long memoryLimit);

    /**
     * 获取当前内存使用情况
     * @param hProcess - 进程句柄
     * @return 当前内存使用量(KB)
     */
    static long long getCurrentMemoryUsage(HANDLE &hProcess);


    /**
     * 获取当前系统时间.
     * @return 当前系统时间(ms)
     */
    static long long getMillisecondsNow();

    /**
     * 强制销毁进程(当触发阈值时).
     * @param  processInfo - 包含进程信息的PROCESS_INFORMATION结构体
     * @return 销毁进程操作是否成功完成
     */
    static bool killProcess(PROCESS_INFORMATION &pi);

    /**
     * 获取应用程序退出代码.
     * 0表示正常退出, 259表示仍在运行.
     * @param  hProcess - 进程的句柄
     * @return 退出代码
     */
    static DWORD getExitCode(HANDLE &hProcess);

    /**
     * 对比运行结果
     * @param runId         - 运行编号
     * @param pid           - 题目编号
     * @param caseId        - 样例编号
     * @return 运行结果编码
     */
    static int answerCompare(std::string runId, std::string pid, std::string caseId);

    /**
     * 判别是否正确
     * @param std   - 标准输出
     * @param out   - 程序输出
     * @return 是否正确
     */
    static bool isAccept(std::string &std, std::string &out);

    /**
     * 判别是否格式错误
     * @param std   - 标准输出
     * @param out   - 程序输出
     * @return 是否格式错误
     */
    static bool isPresentationError(std::string &std, std::string &out);
};



#endif //JUDGE_JUDGECORE_H
