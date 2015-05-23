/*************************************************************************
    > File Name: readline_handler.cpp
    > Author: Guo Hengkai
    > Description: GNU Readline library handler class implementation for YAUSH
    > Created Time: Sat 23 May 2015 10:55:15 AM CST
 ************************************************************************/
#include "readline_handler.h"
#include <unistd.h>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>

using std::string;

namespace ghk
{
ReadlineHandler::ReadlineHandler(): line_read_(nullptr)
{
}

char* ReadlineHandler::Gets()
{
    if (line_read_)
    {
        free(line_read_);
        line_read_ = nullptr;
    }

    string shell_prompt = getenv("USER") + string("@yaush:")
        + getcwd(NULL, 1000) + "$ ";
    line_read_ = readline(shell_prompt.c_str());

    if (line_read_ && *line_read_)
    {
        add_history(line_read_);
    }

    return line_read_;
}
}  // namespace ghk
