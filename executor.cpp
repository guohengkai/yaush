/*************************************************************************
    > File Name: executor.cpp
    > Author: Guo Hengkai
    > Description: Command executor class implementation for YAUSH
    > Created Time: Sun 17 May 2015 03:27:19 PM CST
 ************************************************************************/
#include "executor.h"
#include <vector>

using std::vector;

namespace ghk
{
Executor::Executor()
{

}

bool Executor::Execute(const vector<CommandGroup> &command_list)
{
    return false;
}

bool Executor::Execute(const CommandGroup &command_group)
{
    return false;
}
}  // namespace ghk
