/*************************************************************************
    > File Name: executor.h
    > Author: Guo Hengkai
    > Description: Command executor class definition for YAUSH
    > Created Time: Sun 17 May 2015 11:11:32 AM CST
 ************************************************************************/
#ifndef YAUSH_EXECUTOR_H_
#define YAUSH_EXECUTOR_H_

#include <vector>
#include "command.h"

namespace ghk
{
class Executor
{
public:
    Executor();
    bool Execute(const std::vector<CommandGroup> &command_list);
private:
    bool Execute(const CommandGroup &command_group);
};
}  // namespace ghk

#endif  // YAUSH_EXECUTOR_H_
