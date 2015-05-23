/*************************************************************************
    > File Name: common.h
    > Author: Guo Hengkai
    > Description: Common constants
    > Created Time: Sat 23 May 2015 03:24:20 PM CST
 ************************************************************************/
#ifndef YAUSH_COMMON_H_
#define YAUSH_COMMON_H_

#include <string>

namespace ghk
{
enum FuncStatus
{
    Success = 0,
    Error,
    WaitInput
};

enum JobStatus
{
    Running = 0,
    Stopped,
    Finished
};

struct Job
{
    int pid;
    JobStatus status;
    std::string cmd;
};
}  // namespace ghk

#endif  // YAUSH_COMMON_H_

