/*************************************************************************
    > File Name: executor.cpp
    > Author: Guo Hengkai
    > Description: Command executor class implementation for YAUSH
    > Created Time: Sun 17 May 2015 03:27:19 PM CST
 ************************************************************************/
#include "executor.h"
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <queue>
#include <vector>
#include "command_registry.h"
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
    bool is_execute = true;
    for (auto group: command_list)
    {
        FuncStatus flag = FuncStatus::Error;
        if (is_execute)
        {
            flag = Execute(group.cmd, group.str,
                group.logic == CommandLogic::Background);
        }

        if ((flag == FuncStatus::Success && group.logic == CommandLogic::Or)
                || (flag == FuncStatus::Error && group.logic == CommandLogic::And))
        {
            is_execute = false;
            string info = (group.logic == CommandLogic::Or ? "OR" : "AND");
            LogDebug("%s triggered.", info.c_str());
        }
        else
        {
            is_execute = true;
        }
    }
    return FuncStatus::Success;
}

FuncStatus Executor::Execute(const vector<Command> &cmds,
        const string &str, bool is_bg)
{
    if (cmds.empty())
    {
        return FuncStatus::Success;
    }

    JobHandler *handler = JobHandler::GetInstance();
    CommandRegistry *registry = CommandRegistry::GetInstance();

    int pipes[cmds.size() - 1][2];
    for (size_t i = 0; i < cmds.size() - 1; ++i)
    {
        if (pipe(pipes[i]) < 0)
        {
            ErrorPrint(ShellError::PipeCreation, cmds[i].name);
            return FuncStatus::Error;
        }
    }

    Job current_job;
    current_job.status = JobStatus::Running;
    current_job.cmd = str;
    for (int i = static_cast<int>(cmds.size()) - 1; i >= 0; --i)  // Reversely connect pipes
    {
        int pid = fork();
        if (pid > 0)  // parent
        {
            current_job.pids.push_back(pid);
        }
        else if (pid == 0)  // child
        {
            // Input
            if (cmds[i].io_type[0] == CommandIOType::Pipe)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                LogDebug("command %s duplicate pipe to stdin",
                        cmds[i].name.c_str());
            }
            else if (cmds[i].io_type[0] == CommandIOType::File)
            {
                for (auto name: cmds[i].io_file_name[0])
                {
                    int fid = open(name.c_str(), O_RDONLY);
                    if (fid == -1)
                    {
                        ErrorPrint(ShellError::FileError, name);
                        exit(-1);
                    }
                    dup2(fid, STDIN_FILENO);
                    close(fid);
                    LogDebug("command %s duplicate %s to stdin",
                            cmds[i].name.c_str(), name.c_str());
                }
            }

            // Output
            if (cmds[i].io_type[1] == CommandIOType::Pipe)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
                LogDebug("command %s duplicate pipe to stdout",
                        cmds[i].name.c_str());  // Must use stderr
            }
            else if (cmds[i].io_type[1] == CommandIOType::File)
            {
                for (auto name: cmds[i].io_file_name[1])
                {
                    int fid = open(name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);
                    if (fid == -1)
                    {
                        ErrorPrint(ShellError::FileError, name);
                        exit(-1);
                    }
                    dup2(fid, STDOUT_FILENO);
                    close(fid);
                    LogDebug("command %s duplicate %s to stdout",
                            cmds[i].name.c_str(), name.c_str());
                }
            }

            // Close all the pipe descriptors
            for (size_t j = 0; j < cmds.size() - 1; ++j)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute the command
            if (registry->IsCommandMain(cmds[i].name))
            {
                exit(0);
            }

            auto status = registry->ExecuteCommand(cmds[i].name,
                                                          cmds[i].arg_list);
            if (status == CmdStatus::Notfound)  // System command
            {
                char *argv[cmds[i].arg_list.size() + 1];
                for (size_t j = 0; j  < cmds[i].arg_list.size(); ++j)
                {
                    argv[j] = const_cast<char*>(cmds[i].arg_list[j].c_str());
                }
                argv[cmds[i].arg_list.size()] = NULL;
                int flag = execvp(cmds[i].name.c_str(), argv);
                if (flag < 0)
                {
                    ErrorPrint(ShellError::ExecError,
                            cmds[i].name + ": " + strerror(errno));
                    exit(-1);
                }
            }
            else if (status == CmdStatus::Fail)
            {
                ErrorPrint(ShellError::ExecError,
                        cmds[i].name + ": " + registry->error_info());
                exit(-1);
            }
            else
            {
                exit(0);
            }
        }
        else  // fail to fork
        {
            ErrorPrint(ShellError::ForkFail, cmds[i].name);
            return FuncStatus::Error;
        }
    }

    // Close all the pipe descriptors
    for (size_t i = 0; i < cmds.size() - 1; ++i)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    if (is_bg)  // background
    {
        int job_num = handler->InsertBackgroundJob(current_job);
        handler->PrintJob(job_num);
    }
    else  // foreground
    {
        handler->fg_job = current_job;
        auto &pid_list = handler->fg_job.pids;
        bool is_success = true;
        while (!pid_list.empty())
        {
            auto pid = pid_list.front();
            int status;
            int val = waitpid(pid, &status, 0);
            if (val > 0)
            {
                LogDebug("process %d is waited successfully", pid);
                if (!WIFEXITED(status)
                    || (WIFEXITED(status) && WEXITSTATUS(status) != 0))
                {
                    is_success = false;
                }
            }
            else if (errno == ECHILD)  // pid not found
            {
                LogDebug("process %d not found", pid);
                is_success = false;
            }
            else
            {
                ErrorPrint(ShellError::UnknownError, "waitpid");
                is_success = false;
            }
            pid_list.pop_front();
        }

        if (cmds.size() == 1 && registry->IsCommandMain(cmds[0].name))
        {
            auto status = registry->ExecuteCommand(cmds[0].name,
                                                          cmds[0].arg_list);
            if (status == CmdStatus::Fail)
            {
                ErrorPrint(ShellError::ExecError,
                        cmds[0].name + ": " + registry->error_info());
                return FuncStatus::Error;
            }
        }

        if (!is_success)
        {
            return FuncStatus::Error;
        }
    }
    return FuncStatus::Success;
}
}  // namespace ghk
