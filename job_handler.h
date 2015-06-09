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

namespace ghk
{
enum JobStatus
{
    Running = 0,
    Stopped,
    Done
};

struct Job
{
    std::list<int> pids;
    JobStatus status;
    std::string cmd;
    int job_num;
};

class JobHandler
{
public:
    static JobHandler* GetInstance();
    int InsertBackgroundJob(const Job &job);
    void CheckBackgroundJobs();
    void PrintJob(int idx);
    bool GetJobIterator(int idx, std::list<Job>::iterator *job_iter);

    inline int max_idx() { return max_idx_; }
    inline int IncreaseIdx() { return ++max_idx_; }
    inline bool is_exit() { return is_exit_; }
    inline void set_is_exit(bool is_exit) { is_exit_ = is_exit; }

    std::list<Job> bg_jobs;
    Job fg_job;

private:
    JobHandler() {}  // Never be instantiated
    char GetJobChar(int idx);
    std::string GetJobStatus(JobStatus status);

    int max_idx_;
    bool is_exit_;
};
}  // namespace ghk

#endif  // YAUSH_JOB_HANDLER_H_

