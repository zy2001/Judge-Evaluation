//
// Created by zy on 2020/2/1.
//

#include <cstdio>
#include <iostream>
#include <windows.h>
#include <fcntl.h>
#include <psapi.h>
#include <tlhelp32.h>
#include "../config.h"
#include "../header/utils.h"
#include "../header/judgecore.h"


//-1系统错误
/**
 * 运行程序，检测资源使用
 * @param runId         - 运行编号
 * @param pid           - 题目编号
 * @param timeLimit     - 时间限制
 * @param memoryLimit   - 内存限制
 * @param caseCount     -测试样例数目
 * @return 运行结果编码
 */
int JudgeCore::run(JudgeItem &judgeItem, JudgeStatus &judgeStatus) {
    // 最长运行时间，最大使用内存
    long long maxTimeUsage = 0, maxMemoryUsage = 0;
    // 逐个TestCase运行
    for (int caseNumber = 1; caseNumber <= judgeItem.getCaseCount(); caseNumber++) {
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(STARTUPINFOA));
        // 重载子进程IO
        if (!JudgeCore::setUpIORedirection(si, judgeItem.getRid(), judgeItem.getPid(),
                                           Utils::parseString(caseNumber))) {
            printf("Create Fail\n");
            return SYSTEM_ERROR;
        }
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        // 创建测试进程
        if (!JudgeCore::createProcess(si, pi, judgeItem.getRid())) {
            printf("Create Process Fail!\n");
            return SYSTEM_ERROR;
        }
        HANDLE hJob = CreateJobObject(nullptr, nullptr);
        JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimit;
        ZeroMemory(&basicLimit, sizeof(basicLimit));
        basicLimit.ActiveProcessLimit = 1;
        basicLimit.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
        SetInformationJobObject(hJob, JobObjectBasicLimitInformation, &basicLimit, sizeof(basicLimit));

        JOBOBJECT_BASIC_UI_RESTRICTIONS jobUILimit;
        jobUILimit.UIRestrictionsClass =
                JOB_OBJECT_UILIMIT_DESKTOP |            // 阻止进程创建或切换桌面。
                JOB_OBJECT_UILIMIT_DISPLAYSETTINGS |    // 进程更改显示设置。
                JOB_OBJECT_UILIMIT_EXITWINDOWS |        // 阻止进程注销，关机，重启或断开系统电源
                JOB_OBJECT_UILIMIT_GLOBALATOMS |        // 为作业指定其专有的全局原子表，并限定作业中的进程只能访问此作业的表。
                JOB_OBJECT_UILIMIT_HANDLES |            // 阻止作业中的进程使用同一个作业外部的进程所创建的用户对象( 如HWND) 。
                JOB_OBJECT_UILIMIT_READCLIPBOARD |      // 阻止进程读取剪贴板中的内容。
                JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS |   // 阻止进程更改系统参数。
                JOB_OBJECT_UILIMIT_WRITECLIPBOARD;      // 阻止进程清除剪贴板中的内容。
        SetInformationJobObject(hJob, JobObjectBasicUIRestrictions, &jobUILimit, sizeof(jobUILimit));
        AssignProcessToJobObject(hJob, pi.hProcess);

        long long memoryUsage = 0, timeUsage = 0, outputUsage = 0;
        printf("Judge Start!\n");
        DWORD exitCode = JudgeCore::runProcess(pi, judgeItem.getRid(), Utils::parseString(caseNumber), memoryUsage,
                                               timeUsage,
                                               outputUsage, judgeItem.getTimeLimit(), judgeItem.getMemoryLimit());
        // 关闭进程相关句柄
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(si.hStdInput);
        CloseHandle(si.hStdOutput);
        TerminateJobObject(hJob, 0);
        CloseHandle(hJob);
        printf("Judge Finished!\n");
        if (outputUsage > OUTPUT_LIMIT) {
            printf("Output Limit Exceeded!\n");
            return OUTPUT_LIMIT_EXCEEDED;
        }
        if (exitCode == ERROR_NOT_ENOUGH_QUOTA || timeUsage > judgeItem.getTimeLimit()) {
            printf("Time Limit Exceeded!\n");
            return TIME_LIMIT_EXCEEDED;
        }
        if (memoryUsage > judgeItem.getMemoryLimit()) {
            printf("Memory Limit Exceeded!\n");
            return MEMORY_LIMIT_EXCEEDED;
        }
        if (exitCode != 0) {
            printf("RunTime Error!\n");
            return RUNTIME_ERROR;
        }
        printf("Memory:%lldKB Time:%lldms\n", memoryUsage, timeUsage);
        maxTimeUsage = std::max(timeUsage, maxTimeUsage);
        maxMemoryUsage = std::max(memoryUsage, maxMemoryUsage);
        // 校验结果
        int result = answerCompare(judgeItem.getRid(), judgeItem.getPid(), Utils::parseString(caseNumber));
        if (ACCEPT != result) {
            // 答案不正确
            return result;
        }
    }
    // 答案正确更新runTime, runMemory
    judgeStatus.setRunTime(maxTimeUsage);
    judgeStatus.setRunMemory(maxMemoryUsage);
    return ACCEPT;
}


/**
 * 重定向子进程IO
 * @param si    - 子进程STARTUPINFO结构体
 * @param runid - 运行编号
 * @param pid   - 题目编号
 * @return 重定向是否成功
 */
bool JudgeCore::setUpIORedirection(STARTUPINFO &si, std::string runId, std::string pid, std::string caseNum) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    // 子进程stdin重定向
    std::string path = "../testcase/" + pid + "/" + caseNum + ".in";
    std::cout << "inpath" << path << std::endl;
    HANDLE hFileRead = CreateFile(path.c_str(),
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  &sa,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
    if (hFileRead == INVALID_HANDLE_VALUE) {
        printf("Open InPutFile Fail!\n");
        return false;
    }
    // 子进程stdout重定向
    path = "../data/" + runId + "/" + caseNum + ".out";
    std::cout << "outpath" << path << std::endl;
    HANDLE hFileWrite = CreateFile(path.c_str(),
                                   GENERIC_WRITE,
                                   0,
                                   &sa,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if (hFileWrite == INVALID_HANDLE_VALUE) {
        printf("Open OutPutFile Fail!\n");
        return false;
    }
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = hFileRead;
    si.hStdError = hFileWrite;
    si.hStdOutput = hFileWrite;
    return true;
}

/**
 * 创建子进程
 * @param si    - 子进程STARTUPINFO结构体
 * @param pi    - 子进程PROCESS_INFOMATION结构体
 * @param runid - 运行编号
 */
bool JudgeCore::createProcess(STARTUPINFO &si, PROCESS_INFORMATION &pi, std::string runId) {
    // 设置进程无窗口并等待唤醒
    std::string cmd = "../data/" + runId + "/main.exe";
    std::cout << cmd << std::endl;
    bool ret = CreateProcess(nullptr, (LPSTR) cmd.c_str(), nullptr, nullptr, TRUE,
                             CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
    return ret;
}

/**
 * 运行子进程
 * @param pi          - 子进程信息结构体
 * @param timeLimit   - 运行时间限制(ms)
 * @param memoryLimit - 运行空间限制(KB)
 * @param timeUsage   - 运行时间使用(ms)
 * @param memoryUsage - 运行内存使用(KB)
 * @return 进程退出代码
 */
DWORD
JudgeCore::runProcess(PROCESS_INFORMATION &pi, std::string runId, std::string caseNumber, long long &memoryUsage,
                      long long &timeUsage, long long &outputUsage, long long timeLimit,
                      long long memoryLimit) {
    // 唤醒子进程
    ResumeThread(pi.hThread);
    // 检测资源使用是否超限
    while (getExitCode(pi.hProcess) == STILL_ACTIVE) {
        timeUsage = getTimeUsage(pi.hThread);
        memoryUsage = std::max(memoryUsage, getCurrentMemoryUsage(pi.hProcess));
        outputUsage = getOutputUsage(pi, runId, caseNumber);
        // 超时杀死进程
        // 超内存杀死进程
        // 输出超限杀死进程
        if (timeUsage > timeLimit || memoryUsage > memoryLimit || outputUsage > OUTPUT_LIMIT) {
            killProcess(pi);
            break;
        }
//        printf("thread kernel exec time = %lld\n", timeUsage); // 内核层运行时间
//        printf("run memory usage = %lld\n", memoryUsage);
//        printf("output file size = %lld\n", outputUsage);
        Sleep(10);
    }
    return getExitCode(pi.hProcess);
}

/**
 * 获取当前时间使用情况
 * @param hThread - 进程句柄
 * @return 当前时间使用量(ms)
 */
long long JudgeCore::getTimeUsage(HANDLE &hThread) {
    //保存进程运行时间信息
    FILETIME creationTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;
    GetThreadTimes(hThread, &creationTime, &exitTime, &kernelTime, &userTime);

    // 计算用户态耗时
    FILETIME userFileTimeDiff;
    userFileTimeDiff.dwHighDateTime = userTime.dwHighDateTime;
    userFileTimeDiff.dwLowDateTime = userTime.dwLowDateTime;
    SYSTEMTIME userSysTimeDiff;
    FileTimeToSystemTime(&userFileTimeDiff, &userSysTimeDiff);
    return userSysTimeDiff.wMinute * 60000 + userSysTimeDiff.wSecond * 1000 + userSysTimeDiff.wMilliseconds;
}


/**
 * 获取当前内存使用情况
 * @param hProcess - 进程句柄
 * @return 当前内存使用量(KB)
 */
long long JudgeCore::getCurrentMemoryUsage(HANDLE &hProcess) {
    PROCESS_MEMORY_COUNTERS pmc;
    long long currentMemoryUsage = 0;
    if (!GetProcessMemoryInfo(hProcess, &pmc, sizeof pmc)) {
        return 0;
    }
    currentMemoryUsage = pmc.PeakWorkingSetSize >> 10;
    if (currentMemoryUsage < 0) {
        currentMemoryUsage = INT_MAX >> 10;
    }
    return currentMemoryUsage;
}

/**
 *  获取输出文件大小
 * @param pi         - 子进程信息结构体
 * @param runId      - 运行编号
 * @param caseNumber - 测试样例编号
 * @return 输出文件大小
 */
long long JudgeCore::getOutputUsage(PROCESS_INFORMATION &pi, std::string &runId, std::string &caseNumber) {
    long long currentOutputFileUsage = -1;
    std::string path = "../data/" + runId + "/" + caseNumber + ".out";
    HANDLE hFile = CreateFile(reinterpret_cast<LPCSTR>(path.c_str()), 0, FILE_SHARE_WRITE, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (INVALID_HANDLE_VALUE != hFile) {
        LARGE_INTEGER size;
        ::GetFileSizeEx(hFile, &size);
        currentOutputFileUsage = size.QuadPart;
    }
    CloseHandle(hFile);
    return currentOutputFileUsage;
}

/**
 * 获取应用程序退出代码.
 * 0表示正常退出, 259表示仍在运行.
 * @param  hProcess - 进程的句柄
 * @return 退出代码
 */
DWORD JudgeCore::getExitCode(HANDLE &hProcess) {
    DWORD exitCode = 0;
    GetExitCodeProcess(hProcess, &exitCode);
    return exitCode;
}

/**
 * 强制销毁进程(当触发阈值时).
 * @param  processInfo - 包含进程信息的PROCESS_INFORMATION结构体
 * @return 销毁进程操作是否成功完成
 */
bool JudgeCore::killProcess(PROCESS_INFORMATION &pi) {
    DWORD processId = pi.dwProcessId;
    PROCESSENTRY32 processEntry = {0};
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE handleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(handleSnap, &processEntry)) {
        BOOL isContinue = TRUE;

        do {
            if (processEntry.th32ParentProcessID == processId) {
                HANDLE hChildProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
                if (hChildProcess) {
                    TerminateProcess(hChildProcess, 1);
                    CloseHandle(hChildProcess);
                }
            }
            isContinue = Process32Next(handleSnap, &processEntry);
        } while (isContinue);

        HANDLE hBaseProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (hBaseProcess) {
            TerminateProcess(hBaseProcess, 1);
            CloseHandle(hBaseProcess);
        }
    }

    if (getExitCode(pi.hProcess) == STILL_ACTIVE) {
        return false;
    }
    return true;
}

/**
 * 对比运行结果
 * @param runId         - 运行编号
 * @param pid           - 题目编号
 * @param caseId        - 样例编号
 * @return 运行结果编码
 */
int JudgeCore::answerCompare(std::string runId, std::string pid, std::string caseId) {
    //读标准输出
    std::string std = Utils::readFile("../testcase/" + pid + "/" + caseId + ".out");
    //忽略结尾换行符
    std = Utils::ignoreLineEnd(std);
    //读程序输出
    Sleep(100);
    std::string out = Utils::readFile("../data/" + runId + "/" + caseId + ".out");
    //忽略结尾换行符
    out = Utils::ignoreLineEnd(out);
//    debug(std);
//    debug(out);
    //判别ACCEPT
    if (JudgeCore::isAccept(std, out)) {
        return ACCEPT;
    }
    //判别格式错误
    if (JudgeCore::isPresentationError(std, out)) {
        return PRESENTATION_ERROR;
    }
    //答案错误
    return WRONG_ANSWER;
}

/**
 * 判别是否正确
 * @param std   - 标准输出
 * @param out   - 程序输出
 * @return 是否正确
 */
bool JudgeCore::isAccept(std::string &std, std::string &out) {
    //长度不相等不匹配，返回false
    if (std.length() != out.length()) return false;
    for (int i = 0; i < std.length(); i++) {
        if (std[i] != out[i]) {
            //字符不相等，返回false
            return false;
        }
    }
    return true;
}

/**
 * 判别是否格式错误
 * @param std   - 标准输出
 * @param out   - 程序输出
 * @return 是否格式错误
 */
bool JudgeCore::isPresentationError(std::string &std, std::string &out) {
    int i = 0, j = 0;
    while (i < std.length() && j < out.length()) {
        if (std[i] == ' ' || std[i] == '\n') {
            i++;
        } else if (out[j] == ' ' || out[j] == '\n') {
            j++;
        } else if (std[i] != out[j]) { break; }
        else {
            i++;
            j++;
        }
    }
    if (i == std.length() && j == out.length()) {
        return true;
    }
    return false;
}

