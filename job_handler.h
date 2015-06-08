/*************************************************************************
    > File Name: job_handler.h
    > Author: Guo Hengkai
    > Description: Job handler class definition for YAUSH
    > Created Time: Tue 02 Jun 2015 04:44:29 PM CST
 ************************************************************************/
#ifndef YAUSH_JOB_HANDLER_H_
#define YAUSH_JOB_HANDLER_H_

#include <string>
#include <list>
#include <queue>

namespace ghk
{
enum JobStatus
{
    Running = 0,
    Stopped
};

struct Job
{
    std::queue<int> pids;
    JobStatus status;
    std::string cmd;
    int job_num;
};

class JobHandler
{
public:
    static JobHandler* GetInstance();
    bool InsertBackgroundJob(const Job &job);
    void CheckBackgroundJobs();
    void KillAllJobs();
    inline int max_idx() { return max_idx_; }
    inline int IncreaseIdx() { return ++max_idx_; }

    std::list<Job> bg_jobs;
    Job fg_job;

private:
    JobHandler() {}  // Never be instantiated
    char GetJobChar(int idx);
    int max_idx_;
};
}  // namespace ghk

#endif  // YAUSH_JOB_HANDLER_H_

