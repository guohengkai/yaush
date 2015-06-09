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
#include <string>
#include "error_util.h"

using std::list;
using std::string;

namespace ghk
{
JobHandler* JobHandler::GetInstance()
{
    static JobHandler instance_;
    return &instance_;
}

int JobHandler::InsertBackgroundJob(const Job &job)
{
    if (bg_jobs.empty())  // Reset job number
    {
        max_idx_ = 0;
    }
    bg_jobs.push_back(job);
    bg_jobs.back().job_num = IncreaseIdx();
    return bg_jobs.back().job_num;
}

void JobHandler::CheckBackgroundJobs()
{
    auto iter = bg_jobs.begin();
    while (iter != bg_jobs.end())
    {
        auto &job = *iter;
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
                job.pids.pop_front();
            }

            if (job.pids.empty())
            {
                job.status = JobStatus::Done;
                PrintJob(job.job_num);
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

bool JobHandler::GetJobIterator(int idx, list<Job>::iterator *job_iter)
{
    auto iter = bg_jobs.begin();
    while (iter != bg_jobs.end())
    {
        if (iter->job_num == idx)
        {
            break;
        }
        ++iter;
    }

    if (iter == bg_jobs.end())
    {
        return false;
    }

    *job_iter = iter;
    return true;
}

void JobHandler::PrintJob(int idx)
{
    list<Job>::iterator job_iter;
    if (!GetJobIterator(idx, &job_iter))
    {
        return;
    }

    auto job = *job_iter;
    printf("[%d]%c  %s\t\t\t%s\n",
            job.job_num, GetJobChar(job.job_num),
            GetJobStatus(job.status).c_str(), job.cmd.c_str());
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

string JobHandler::GetJobStatus(JobStatus status)
{
    switch (status)
    {
        case JobStatus::Running:
            return "Running";
        case JobStatus::Stopped:
            return "Stopped";
        case JobStatus::Done:
            return "Done";
        default:
            return "Unknown";
    }
}
}  // namespace ghk
