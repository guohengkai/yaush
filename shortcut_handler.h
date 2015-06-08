/*************************************************************************
    > File Name: shortcut_handler.h
    > Author: Guo Hengkai
    > Description: Shortcut handler class definition for YAUSH
    > Created Time: Mon 08 Jun 2015 03:50:38 PM CST
 ************************************************************************/
#ifndef YAUSH_SHORTCUT_HANDLER_H_
#define YAUSH_SHORTCUT_HANDLER_H_

#include <setjmp.h>
#include <signal.h>

#define CatchJump(handler) \
    if (sigsetjmp(handler->jmp_buf_ctrlz(), 1) != 0) {} \
    if (sigsetjmp(handler->jmp_buf_ctrlc(), 1) != 0) {}

namespace ghk
{
class ShortcutHandler
{
public:
    static ShortcutHandler* GetInstance();
    bool BindShortcut();

    inline sigjmp_buf& jmp_buf_ctrlz() { return jmp_buf_ctrlz_; }
    inline sigjmp_buf& jmp_buf_ctrlc() { return jmp_buf_ctrlc_; }

private:
    ShortcutHandler() {}
    sigjmp_buf jmp_buf_ctrlz_;
    sigjmp_buf jmp_buf_ctrlc_;
};

void SignalCtrlZ(int signo, siginfo_t *info, void *context);
void SignalCtrlC(int signo, siginfo_t *info, void *context);
}  // namespace ghk

#endif  // YAUSH_SHORTCUT_HANDLER_H_

