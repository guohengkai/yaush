/*************************************************************************
    > File Name: shortcut_handler.cpp
    > Author: Guo Hengkai
    > Description: Shortcut handler class implementation for YAUSH
    > Created Time: Mon 08 Jun 2015 04:01:08 PM CST
 ************************************************************************/
#include "shortcut_handler.h"
#include <setjmp.h>
#include <signal.h>
#include <sys/wait.h>
#include <queue>
#include "error_util.h"
#include "job_handler.h"

using std::queue;

namespace ghk
{
ShortcutHandler* ShortcutHandler::GetInstance()
{
    static ShortcutHandler instance_;
    return &instance_;
}

bool ShortcutHandler::BindShortcut()
{
    struct sigaction sig_act;
    sigfillset(&(sig_act.sa_mask));
    sig_act.sa_flags = SA_SIGINFO;

    sig_act.sa_sigaction = SignalCtrlZ;
    if (sigaction(SIGTSTP, &sig_act, NULL) < 0)
    {
        return false;
    }

    sig_act.sa_sigaction = SignalCtrlC;
    if (sigaction(SIGINT, &sig_act, NULL) < 0)
    {
        return false;
    }

    return true;
}

void SignalCtrlZ(int signo, siginfo_t *info, void *context)
{
    // BUGS: Vim is abnormal with "Ctrl-Z, fg"
    printf("\n");
    LogDebug("Ctrl-Z is pressed");

    JobHandler *job_handler = JobHandler::GetInstance();
    Job &job = job_handler->fg_job;
    job.status = JobStatus::Stopped;
    int job_num = job_handler->InsertBackgroundJob(job);
    job.pids.clear();
    job_handler->PrintJob(job_num);

    // All the background jobs will be stopped too
    for (auto job: job_handler->bg_jobs)
    {
        job.status = JobStatus::Stopped;
    }

    ShortcutHandler *shortcut_handler = ShortcutHandler::GetInstance();
    siglongjmp(shortcut_handler->jmp_buf_ctrlz(), 1);
}

void SignalCtrlC(int signo, siginfo_t *info, void *context)
{
    printf("\n");
    LogDebug("Ctrl-C is pressed");

    JobHandler *job_handler = JobHandler::GetInstance();
    auto &pid_list = job_handler->fg_job.pids;
    while (!pid_list.empty())
    {
        auto pid = pid_list.front();
        int status;
        waitpid(pid, &status, 0);
        pid_list.pop_front();
    }

    // All the background jobs will be terminated too
    for (auto job: job_handler->bg_jobs)
    {
        for (auto pid: job.pids)
        {
            int status;
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
        }
    }
    job_handler->bg_jobs.clear();

    ShortcutHandler *shortcut_handler = ShortcutHandler::GetInstance();
    siglongjmp(shortcut_handler->jmp_buf_ctrlc(), 1);
}
}  // namespace ghk
