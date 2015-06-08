/*************************************************************************
    > File Name: job_handler.cpp
    > Author: Guo Hengkai
    > Description: Job handler class implementation for YAUSH
    > Created Time: Tue 02 Jun 2015 04:49:34 PM CST
 ************************************************************************/
#include "job_handler.h"
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <list>
#include "error_util.h"

using std::list;

namespace ghk
{
JobHandler* JobHandler::GetInstance()
{
    static JobHandler instance_;
    return &instance_;
}

bool JobHandler::InsertBackgroundJob(const Job &job)
{
    bg_jobs.push_back(job);
    bg_jobs.back().job_num = IncreaseIdx();
    return true;
}

void JobHandler::CheckBackgroundJobs()
{
    auto iter = bg_jobs.begin();
    while (iter != bg_jobs.end())
    {
        auto job = *iter;
        if (job.status == JobStatus::Running)
        {
            while (!job.pids.empty())
            {
                auto pid = job.pids.front();
                int status;
                int val = waitpid(pid, &status, WNOHANG);
                if (val == 0)  // Not finished
                {
                    break;
                }
                else if (val > 0)
                {
                    LogDebug("process %d is waited successfully", pid);
                }
                else if (errno == ECHILD)
                {
                    LogDebug("process %d not found", pid);
                }
                else
                {
                    ErrorPrint(ShellError::UnknownError, "waitpid");
                }
                job.pids.pop();
            }

            if (job.pids.empty())
            {
                printf("[%d]%c  Done\t\t\t%s\n",
                        job.job_num, GetJobChar(job.job_num), job.cmd.c_str());
                iter = bg_jobs.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        else
        {
            ++iter;
        }
    }
}

char JobHandler::GetJobChar(int idx)
{
    int cnt = 0;
    auto iter = bg_jobs.rbegin();
    while (iter != bg_jobs.rend() && cnt < 2)
    {
        if (idx == iter->job_num)
        {
            if (cnt == 0)
            {
                return '+';
            }
            else if (cnt == 1)
            {
                return '-';
            }
        }
        ++cnt;
        ++iter;
    }
    return ' ';
}
}  // namespace ghk
