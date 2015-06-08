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
#include "error_util.h"
#include "job_handler.h"

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
    printf("\n");
    LogDebug("Ctrl-Z is pressed");

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
        int res = kill(pid, SIGINT);
        printf("kill: %d\n", res);
        int status;
        int val = waitpid(pid, &status, 0);
        printf("val: %d\n", val);
        pid_list.pop();
    }

    ShortcutHandler *shortcut_handler = ShortcutHandler::GetInstance();
    siglongjmp(shortcut_handler->jmp_buf_ctrlc(), 1);
}
}  // namespace ghk
