/*************************************************************************
    > File Name: shortcut_handler.cpp
    > Author: Guo Hengkai
    > Description: Shortcut handler class implementation for YAUSH
    > Created Time: Mon 08 Jun 2015 04:01:08 PM CST
 ************************************************************************/
#include "shortcut_handler.h"
#include <setjmp.h>
#include <signal.h>
#include "error_util.h"

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

    ShortcutHandler *shortcut_handler = ShortcutHandler::GetInstance();
    siglongjmp(shortcut_handler->jmp_buf_ctrlc(), 1);
}
}  // namespace ghk
