//
// Created by zy on 2020/2/1.
//

#include <cstdio>
#include <future>
#include <iostream>
#include <windows.h>
#include <fcntl.h>
#include <psapi.h>
#include <tlhelp32.h>
#include "../../config.h"
#include "../header/judgecore.h"
#include "../../utils/header/utils.h"
#include "../../database/header/connect.h"


//-1系统错误
int JudgeCore::run(const std::string &runId, const std::string &pid, int timeLimit, int memoryLimit, int caseCount) {
    //最长运行时间，最大使用内存
    long long maxTimeUsage = 0, maxMemoryUsage = 0;
    //逐个TestCase运行
    for (int i = 1; i <= caseCount; i++) {
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&si, sizeof(STARTUPINFOA));
        //重载子进程IO
        if (!JudgeCore::setUpIORedirection(si, runId, pid, Utils::parseString(i))) {
            printf("Create Fail\n");
            return SYSTEM_ERROR;
        }
        //创建测试进程
        if (!JudgeCore::createProcess(si, pi, runId)) {
            printf("Create Process Fail!\n");
            return SYSTEM_ERROR;
        }
        long long memoryUsage = 0, timeUsage = 0;
        printf("Judge Start!\n");
        DWORD exitCode = JudgeCore::runProcess(pi, memoryUsage, timeUsage, timeLimit, memoryLimit);
        printf("Judge Finished!\n");
        if (exitCode == STILL_ACTIVE) {
            JudgeCore::killProcess(pi);
        }
        if (exitCode != 0) {
            printf("RunTime Error!\n");
            return RUNTIME_ERROR;
        }
        if (timeUsage >= timeLimit) {
            printf("Time Limit Exceeded!\n");
            return TIME_LIMIT_EXCEEDED;
        }
        if (memoryUsage >= memoryLimit) {
            printf("Memory Limit Exceeded!\n");
            return MEMORY_LIMIT_EXCEEDED;
        }
        printf("Memory:%I64dKB Time:%I64dms\n", memoryUsage, timeUsage);
        maxTimeUsage = std::max(timeUsage, maxTimeUsage);
        maxMemoryUsage = std::max(memoryUsage, maxMemoryUsage);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(si.hStdInput);
        CloseHandle(si.hStdOutput);

        //校验结果
    }

    //答案正确更新runTime, runMemory
    std::string sql = "UPDATE SUBMIT SET RUN_TIME = " + Utils::parseString(maxTimeUsage) + " , RUN_MEMORY = " + Utils::parseString(maxMemoryUsage) +" WHERE RID = " + runId;
    debug(sql);
    Connect::mysql_update(sql.c_str());
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
    //子进程stdin重定向
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
    //子进程stdout重定向
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
    //设置为无窗口显示并等待唤醒
    DWORD flags = CREATE_NO_WINDOW | CREATE_SUSPENDED;
    std::string cmd = "../data/" + runId + "/main.exe";
    std::cout << cmd << std::endl;
    //char cmdd[] = "D:\\JudgeClient\\Data\\" + runid + "main.exe";
    bool ret = CreateProcess(NULL, (LPSTR) cmd.c_str(), NULL, NULL, TRUE,
                             flags, NULL, NULL, &si, &pi);
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

DWORD JudgeCore::runProcess(PROCESS_INFORMATION &pi, long long &memoryUsage, long long &timeUsage, long long timeLimit, long long memoryLimit) {
    long long reservedTime = timeLimit * 9;
    auto feature = std::async(std::launch::async, JudgeCore::getMaxMemoryUsage, std::ref(pi), memoryLimit);
    ResumeThread(pi.hThread);

    //等待线程运行
    WaitForSingleObject(pi.hProcess, DWORD(timeLimit + reservedTime));

    FILETIME creationTimeEnd;
    FILETIME exitTimeEnd;
    FILETIME kernelTimeEnd;
    FILETIME userTimeEnd;
    GetThreadTimes(pi.hThread, &creationTimeEnd, &exitTimeEnd, &kernelTimeEnd, &userTimeEnd);

    SYSTEMTIME kernelSysTimeDiff;
    FILETIME kernelFileTimeDiff;
    kernelFileTimeDiff.dwHighDateTime = kernelTimeEnd.dwHighDateTime;
    kernelFileTimeDiff.dwLowDateTime = kernelTimeEnd.dwLowDateTime;
    FileTimeToSystemTime(&kernelFileTimeDiff, &kernelSysTimeDiff);

    printf("thread kernal exec time = %d\n",
           kernelSysTimeDiff.wSecond * 1000 + kernelSysTimeDiff.wMilliseconds); // 内核层运行时间

    timeUsage = (long long) kernelSysTimeDiff.wSecond * 1000 + kernelSysTimeDiff.wMilliseconds;

    if (getExitCode(pi.hProcess) == STILL_ACTIVE) {
        killProcess(pi);
    }
    memoryUsage = feature.get();
    return getExitCode(pi.hProcess);
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
 * 获取内存占用最大值
 * @param pi          - 子进程信息结构体
 * @param memoryLimit - 运行空间限制(KB)
 * @return 最大内存使用量
 */
long long JudgeCore::getMaxMemoryUsage(PROCESS_INFORMATION &pi, long long memoryLimit) {
    long long maxMemoryUsage = 0, currentMemoryUsage = 0;
    do {
        currentMemoryUsage = getCurrentMemoryUsage(pi.hProcess);
        if (currentMemoryUsage > maxMemoryUsage) {
            maxMemoryUsage = currentMemoryUsage;
        }
        if (currentMemoryUsage > memoryLimit) {
            killProcess(pi);
        }
        Sleep(1);
    } while (getExitCode(pi.hProcess) == STILL_ACTIVE);
    return maxMemoryUsage;
}

/**
 * 获取当前系统时间.
 * @return 当前系统时间(ms)
 */
long long JudgeCore::getMillisecondsNow() {
    static LARGE_INTEGER frequency;
    static BOOL useQpf = QueryPerformanceFrequency(&frequency);
    if (useQpf) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / frequency.QuadPart;
    } else {
        return GetTickCount();
    }
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