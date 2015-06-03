/*************************************************************************
    > File Name: job_handler.cpp
    > Author: Guo Hengkai
    > Description: Job handler class implementation for YAUSH
    > Created Time: Tue 02 Jun 2015 04:49:34 PM CST
 ************************************************************************/
#include "job_handler.h"
#include <map>

using std::map;

namespace ghk
{
std::map<int, Job> JobHandler::bg_jobs;
Job JobHandler::fg_job;

JobHandler* JobHandler::GetInstance()
{
    static JobHandler instance_;
    return &instance_;
}

bool JobHandler::InsertBackgroundJob(const Job &job)
{
    int idx;
    if (bg_jobs.empty())
    {
        idx = 1;
    }
    else
    {
        idx = bg_jobs.rbegin()->first + 1;
    }
    bg_jobs[idx] = job;
    return true;
}
}  // namespace ghk
