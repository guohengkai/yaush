/*************************************************************************
    > File Name: command_registry.cpp
    > Author: Guo Hengkai
    > Description: Custom command factory class implementation for YAUSH
    > Created Time: Sun 17 May 2015 02:14:43 PM CST
 ************************************************************************/
#include "command_registry.h"
#include <cstring>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include "error_util.h"
#include "job_handler.h"
#include "readline_handler.h"

using std::list;
using std::map;
using std::queue;
using std::set;
using std::string;
using std::stringstream;
using std::vector;

namespace ghk
{
CommandRegistry* CommandRegistry::GetInstance()
{
    static CommandRegistry instance_;
    return &instance_;
}

void CommandRegistry::AddCommand(const string &name,
                                 CmdExecute cmd, bool is_main)
{
    registry_[name] = cmd;
    if (is_main)
    {
        main_set_.insert(name);
    }
}

CmdStatus CommandRegistry::ExecuteCommand(const string &name,
            const vector<string> &argv)
{
    if (registry_.count(name) != 1)
    {
        return CmdStatus::Notfound;
    }
    else
    {
        return (registry_[name](name, argv) ? CmdStatus::Ok
                                            : CmdStatus::Fail);
    }
}

string CommandRegistry::CommandList()
{
    string result;
    int cnt = 0;
    for (auto iter = registry_.begin(); iter != registry_.end(); ++iter, ++cnt)
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
    CommandRegistry *registry = CommandRegistry::GetInstance();
    string result = registry->CommandList();
    printf("Support custom commands: %s\n", result.c_str());
    return true;
}

bool CustomCd(const string &name, const vector<string> &argv)
{
    string path;
    if (argv.size() == 1 || argv[1] == "~")
    {
        path = string("/home/") + getenv("USER") + "/";
    }
    else
    {
        path = argv[1];
    }

    int val = chdir(path.c_str());
    if (val == -1)
    {
        CommandRegistry *registry = CommandRegistry::GetInstance();
        registry->set_error_info(string(strerror(errno)));
        return false;
    }
    return true;
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
    // Bad idea to use kill(0, SIGKILL)
    JobHandler *job_handler = JobHandler::GetInstance();
    job_handler->set_is_exit(true);
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

    CommandRegistry *registry = CommandRegistry::GetInstance();
    if (!info.empty())
    {
        registry->set_error_info(info + ": no such job");
        return false;
    }

    auto job = *job_iter;
    if (job.status == JobStatus::Done)
    {
        stringstream ss;
        ss << job.job_num;
        registry->set_error_info("job " + ss.str() + " is done");
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
                    registry->set_error_info(
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
                registry->set_error_info("fail with waitpid");
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

    CommandRegistry *registry = CommandRegistry::GetInstance();
    if (!info.empty())
    {
        registry->set_error_info(info + ": no such job");
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

        registry->set_error_info("job " + ss.str() + " is " + status);
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
                registry->set_error_info(
                        "fail to resume the process " + ss.str());
                return false;
            }
        }
        job.status = JobStatus::Running;
        job_handler->PrintJob(job.job_num);
    }
    return true;
}

bool CustomHistory(const string &name, const vector<string> &argv)
{
    ReadlineHandler handler_;
    if (argv.size() == 1)  // List all the history
    {
        vector<string> his_list;
        handler_.GetHistoryList(&his_list);
        for (size_t i = 0; i < his_list.size(); ++i)
        {
            printf("%4zu  %s\n", i + 1, his_list[i].c_str());
        }
    }
    return true;
}
}  // namespace ghk
