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
    Command();

    std::string name;
    std::vector<std::string> arg_list;
    CommandIOType io_type[2];  // Input and output type
    std::string io_file_name[2];  // For file type
    bool is_bg;  // Background command
};

enum CommandLogic
{
    Empty = 0,
    And,
    Or
};

struct CommandGroup
{
    CommandGroup();

    std::vector<Command> cmd;
    CommandLogic logic;
};
}

#endif  // YAUSH_COMMAND_H_
