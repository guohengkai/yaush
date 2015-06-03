/*************************************************************************
    > File Name: command.h
    > Author: Guo Hengkai
    > Description: Command classes definition for YAUSH
    > Created Time: Sun 17 May 2015 11:18:22 AM CST
 ************************************************************************/
#ifndef YAUSH_COMMAND_H_
#define YAUSH_COMMAND_H_

#include <string>
#include <vector>

namespace ghk
{
enum CommandIOType
{
    Standard = 0,
    Pipe,
    File
};

struct Command
{
    Command()
    {
        arg_list.clear();
        for (int i = 0; i < 2; ++i)
        {
            io_type[i] = CommandIOType::Standard;
            io_file_name[i].clear();
        }
    }

    std::string name;
    std::vector<std::string> arg_list;
    CommandIOType io_type[2];  // Input and output type
    std::vector<std::string> io_file_name[2];  // For file type
};

enum CommandLogic
{
    Empty = 0,
    And,
    Or,
    Background  // Background job
};

struct CommandGroup
{
    CommandGroup()
    {
        cmd.clear();
        logic = CommandLogic::Empty;
    }

    std::vector<Command> cmd;
    std::string str;
    CommandLogic logic;
};
}

#endif  // YAUSH_COMMAND_H_
