/*************************************************************************
    > File Name: executor.cpp
    > Author: Guo Hengkai
    > Description: Command executor class implementation for YAUSH
    > Created Time: Sun 17 May 2015 03:27:19 PM CST
 ************************************************************************/
#include "executor.h"
#include <unistd.h>
#include <string>
#include <queue>
#include <vector>
#include "common.h"
#include "error_util.h"
#include "job_handler.h"

using std::string;
using std::queue;
using std::vector;

namespace ghk
{
Executor::Executor()
{

}

FuncStatus Executor::Execute(const vector<CommandGroup> &command_list)
{
    for (auto group: command_list)
    {
        auto flag = Execute(group.cmd, group.str,
                group.logic == CommandLogic::Background);
        if ((flag == FuncStatus::Success && group.logic == CommandLogic::Or)
                || (flag == FuncStatus::Error && group.logic == CommandLogic::And))
        {
#ifdef DEBUG
            string info = (group.logic == CommandLogic::Or ? "OR" : "AND");
            printf("Debug: %s triggered.\n", info.c_str());
#endif
            return FuncStatus::Success;
        }
    }
    return FuncStatus::Success;
}

FuncStatus Executor::Execute(const vector<Command> &cmds,
        const string &str, bool is_bg)
{
    return FuncStatus::Error;
    if (cmds.empty())
    {
        return FuncStatus::Success;
    }

    JobHandler *handler = JobHandler::GetInstance();

    /*
    int pipes[cmds.size() - 1][2];
    for (size_t i = 0; i < cmds.size() - 1; ++i)
    {
        if (pipe(pipes[i]) < 0)
        {
            ErrorPrint(ShellError::PipeCreation, cmds[i].name);
            return FuncStatus::Error;
        }
    }
    */

    Job current_job;
    for (auto i = cmds.size() - 1; i >= 0; --i)  // Reversely connect pipes
    {
    }

    if (is_bg)  // background
    {
        handler->InsertBackgroundJob(current_job);
    }
    else  // foreground
    {
        handler->fg_job = current_job;
        queue<int> &pid_list = handler->fg_job.pids;
        while (!pid_list.empty())
        {
            pid_list.pop();
        }
    }
    return FuncStatus::Success;
}
}  // namespace ghk
