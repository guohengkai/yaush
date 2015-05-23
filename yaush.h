/*************************************************************************
    > File Name: yaush.h
    > Author: Guo Hengkai
    > Description: Shell class definition for YAUSH
    > Created Time: Sat 16 May 2015 10:36:37 PM CST
 ************************************************************************/
#ifndef YAUSH_YAUSH_H_
#define YAUSH_YAUSH_H_

#include <string>
#include <vector>
#include "readline_handler.h"

namespace ghk
{
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

class Yaush
{
public:
    static Yaush* GetInstance();
    static void Loop();
    static std::vector<Job> fore_jobs, back_jobs;

private:
    Yaush() {}  // Never be instantiated
};
}  // namespace ghk

#endif  // YAUSH_YAUSH_H_
