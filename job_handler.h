/*************************************************************************
    > File Name: job_handler.h
    > Author: Guo Hengkai
    > Description: Job handler class definition for YAUSH
    > Created Time: Tue 02 Jun 2015 04:44:29 PM CST
 ************************************************************************/
#ifndef YAUSH_JOB_HANDLER_H_
#define YAUSH_JOB_HANDLER_H_

#include <string>
#include <map>
#include <queue>

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
    std::queue<int> pids;
    JobStatus status;
    std::string cmd;
};

class JobHandler
{
public:
    static JobHandler* GetInstance();
    static bool InsertBackgroundJob(const Job &job);

    static std::map<int, Job> bg_jobs;
    static Job fg_job;

private:
    JobHandler() {}  // Never be instantiated
};
}  // namespace ghk

#endif  // YAUSH_JOB_HANDLER_H_

