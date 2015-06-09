/*************************************************************************
    > File Name: command_registry.cpp
    > Author: Guo Hengkai
    > Description: Custom command factory class implementation for YAUSH
    > Created Time: Sun 17 May 2015 02:14:43 PM CST
 ************************************************************************/
#include "command_registry.h"
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <list>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include "error_util.h"
#include "job_handler.h"

using std::list;
using std::map;
using std::queue;
using std::string;
using std::stringstream;
using std::vector;

namespace ghk
{
string CommandRegistry::error_info_;

CommandRegistry::CmdRegistry& CommandRegistry::Registry()
{
    static CmdRegistry* registry_ = new CmdRegistry();
    return *registry_;
}

void CommandRegistry::AddCommand(const string &name, CmdExecute cmd)
{
    CmdRegistry &registry = Registry();
    registry[name] = cmd;
}

CmdStatus CommandRegistry::ExecuteCommand(const string &name,
            const vector<string> &argv)
{
    CmdRegistry &registry = Registry();
    if (registry.count(name) != 1)
    {
        return CmdStatus::Notfound;
    }
    else
    {
        return (registry[name](name, argv) ? CmdStatus::Ok
                                           : CmdStatus::Fail);
    }
}

string CommandRegistry::CommandList()
{
    CmdRegistry &registry = Registry();
    string result;
    int cnt = 0;
    for (auto iter = registry.begin(); iter != registry.end(); ++iter, ++cnt)
    {
        if (cnt > 0)
        {
            result += ", ";
        }
        result += iter->first;
    }
    return result;
}

bool CustomWhat(const string &name, const vector<string> &argv)
{
    string result = CommandRegistry::CommandList();
    printf("Support custom commands: %s\n", result.c_str());
    return true;
}

bool CustomCd(const string &name, const vector<string> &argv)
{
    return false;
}

bool CustomAbout(const string &name, const vector<string> &argv)
{
    printf("****************************************************\n");
    printf("  > Shell Name: YAUSH\n");
    printf("  > Version:    1.0\n");
    printf("  > Author:     Guo Hengkai\n");
    printf("  > Email:      guohengkaighk@gmail.com\n");
    printf("  > Date:       2015.06.\n");
    printf("  > Website:    https://github.com/guohengkai/yaush\n");
    printf("****************************************************\n");
    return true;
}

bool CustomExit(const string &name, const vector<string> &argv)
{
    // Do nothing in this function but in the executor.cpp
    // Bad idea to use kill(0, SIGKILL)
    return true;
}

bool CustomJobs(const string &name, const vector<string> &argv)
{
    JobHandler *job_handler = JobHandler::GetInstance();
    for (int i = 1; i <= job_handler->max_idx(); ++i)
    {
        job_handler->PrintJob(i);
    }

    return true;
}

bool CustomFg(const string &name, const vector<string> &argv)
{
    JobHandler *job_handler = JobHandler::GetInstance();
    list<Job>::iterator job_iter;
    string info;

    if (argv.size() == 1)
    {
        if (job_handler->bg_jobs.empty())
        {
            info = "current";
        }
        else
        {
            int idx = job_handler->bg_jobs.back().job_num;
            job_handler->GetJobIterator(idx, &job_iter);
        }
    }
    else
    {
        int idx = atoi(argv[1].c_str());
        if (idx <= 0 || !job_handler->GetJobIterator(idx, &job_iter))
        {
            if (argv[1] == "+" && !job_handler->bg_jobs.empty())
            {
                int idx = job_handler->bg_jobs.back().job_num;
                job_handler->GetJobIterator(idx, &job_iter);
            }
            else if (argv[1] == "-" && job_handler->bg_jobs.size() >= 2)
            {
                auto iter = job_handler->bg_jobs.rbegin();
                ++iter;
                int idx = iter->job_num;
                job_handler->GetJobIterator(idx, &job_iter);
            }
            else
            {
                info = argv[1];
            }
        }
    }

    if (!info.empty())
    {
        CommandRegistry::set_error_info(info + ": no such job");
        return false;
    }

    auto job = *job_iter;
    if (job.status == JobStatus::Done)
    {
        stringstream ss;
        ss << job.job_num;
        CommandRegistry::set_error_info("job " + ss.str() + " is done");
        return false;
    }
    else  // Running or stopped
    {
        printf("%s\n", job.cmd.c_str());
        job_handler->fg_job = job;
        job_handler->bg_jobs.erase(job_iter);

        auto &pid_list = job_handler->fg_job.pids;
        if (job.status == JobStatus::Stopped)
        {
            // Resume all the process in the job
            for (auto pid: pid_list)
            {
                auto val = kill(pid, SIGCONT);
                if (val == -1 && errno != ESRCH)
                {
                    stringstream ss;
                    ss << pid;
                    CommandRegistry::set_error_info(
                            "fail to resume the process " + ss.str());
                    return false;
                }
            }
        }

        while (!pid_list.empty())
        {
            auto pid = pid_list.front();
            int status;
            int val = waitpid(pid, &status, 0);
            if (val > 0)
            {
                LogDebug("process %d is waited successfully", pid);
            }
            else if (errno == ECHILD)  // pid not found
            {
                LogDebug("process %d not found", pid);
            }
            else
            {
                CommandRegistry::set_error_info("fail with waitpid");
                return false;
            }
            pid_list.pop_front();
        }
    }
    return true;
}

bool CustomBg(const string &name, const vector<string> &argv)
{
    JobHandler *job_handler = JobHandler::GetInstance();
    list<Job>::iterator job_iter;
    string info;

    if (argv.size() == 1)
    {
        if (job_handler->bg_jobs.empty())
        {
            info = "current";
        }
        else
        {
            int idx = job_handler->bg_jobs.back().job_num;
            job_handler->GetJobIterator(idx, &job_iter);
        }
    }
    else
    {
        int idx = atoi(argv[1].c_str());
        if (idx <= 0 || !job_handler->GetJobIterator(idx, &job_iter))
        {
            if (argv[1] == "+" && !job_handler->bg_jobs.empty())
            {
                int idx = job_handler->bg_jobs.back().job_num;
                job_handler->GetJobIterator(idx, &job_iter);
            }
            else if (argv[1] == "-" && job_handler->bg_jobs.size() >= 2)
            {
                auto iter = job_handler->bg_jobs.rbegin();
                ++iter;
                int idx = iter->job_num;
                job_handler->GetJobIterator(idx, &job_iter);
            }
            else
            {
                info = argv[1];
            }
        }
    }

    if (!info.empty())
    {
        CommandRegistry::set_error_info(info + ": no such job");
        return false;
    }

    auto &job = *job_iter;
    if (job.status == JobStatus::Done || job.status == JobStatus::Running)
    {
        stringstream ss;
        ss << job.job_num;
        string status;
        if (job.status == JobStatus::Done)
        {
            status = "done";
        }
        else
        {
            status = "running";
        }

        CommandRegistry::set_error_info("job " + ss.str() + " is " + status);
        return false;
    }
    else
    {
        auto &pid_list = job.pids;
        for (auto pid: pid_list)
        {
            auto val = kill(pid, SIGCONT);
            if (val == -1 && errno != ESRCH)
            {
                stringstream ss;
                ss << pid;
                CommandRegistry::set_error_info(
                        "fail to resume the process " + ss.str());
                return false;
            }
        }
        job.status = JobStatus::Running;
        job_handler->PrintJob(job.job_num);
    }
    return true;
}

bool CustomImage(const string &name, const vector<string> &argv)
{
    return false;
}

bool CustomLoop(const string &name, const vector<string> &argv)
{
    long long cnt = 0;
    long long limit;
    if (argv.size() == 1)
    {
        limit = 100;
    }
    else
    {
        limit = atoi(argv[1].c_str());
        if (limit <= 0)
        {
            CommandRegistry::set_error_info("invalid time parameter '"
                    + argv[1] + "'");
            return false;
        }
    }

    while (cnt < limit)
    {
        printf("Loop: %lld\n", cnt);
        ++cnt;
        sleep(1);
    }
    return true;
}
}  // namespace ghk
