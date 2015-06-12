/*************************************************************************
    > File Name: readline_handler.cpp
    > Author: Guo Hengkai
    > Description: GNU Readline library handler class implementation for YAUSH
    > Created Time: Sat 23 May 2015 10:55:15 AM CST
 ************************************************************************/
#include "readline_handler.h"
#include <unistd.h>
#include <string>
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>
#include "common.h"

using std::string;
using std::vector;

namespace ghk
{
ReadlineHandler::ReadlineHandler(): line_read_(nullptr)
{
    using_history();
}

ReadlineHandler::~ReadlineHandler()
{
    if (line_read_)
    {
        free(line_read_);
    }
}

char* ReadlineHandler::Gets(bool is_reset)
{
    if (line_read_)
    {
        free(line_read_);
        line_read_ = nullptr;
    }

    string shell_prompt;
    if (is_reset)
    {
        shell_prompt = getenv("USER") + string("@") + SHELL_NAME + ":"
            + getcwd(NULL, 1000) + "$ ";
    }
    else
    {
        shell_prompt = "> ";
    }
    line_read_ = readline(shell_prompt.c_str());

    if (line_read_ && *line_read_)
    {
        add_history(line_read_);
    }

    return line_read_;
}

bool ReadlineHandler::GetHistoryList(vector<string> *history_vec)
{
    HIST_ENTRY **his_list;
    his_list = history_list();
    if (his_list)
    {
        history_vec->clear();
        for (int i = 0; his_list[i]; ++i)
        {
            history_vec->push_back(string(his_list[i]->line));
        }
        return true;
    }
    else
    {
        return false;
    }
}
}  // namespace ghk
