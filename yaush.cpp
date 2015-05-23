/*************************************************************************
    > File Name: yaush.cpp
    > Author: Guo Hengkai
    > Description: Shell class implementation for YAUSH
    > Created Time: Sun 17 May 2015 03:22:24 PM CST
 ************************************************************************/
#include "yaush.h"
#include <string>
#include <vector>
#include "command.h"
#include "common.h"
#include "executor.h"
#include "parser.h"
#include "readline_handler.h"
#include "scanner.h"

using std::string;
using std::vector;

namespace ghk
{
Yaush* Yaush::GetInstance()
{
    static Yaush instance_;
    return &instance_;
}

void Yaush::Loop()
{
    static ReadlineHandler handler_;
    char* line_read = nullptr;
    string line;

    while (true)
    {
        line_read = handler_.Gets(line.empty());
        if (!line_read)  // EOF
        {
            printf("\n");
            break;
        }
        else if (*line_read)  // Not empty string
        {
            line += string(line_read) + '\n';
            if (Analysis(line) != FuncStatus::WaitInput)
            {
                line = "";
            }
        }
    }
}

FuncStatus Yaush::Analysis(const string &line)
{
    Scanner scanner;
    Parser parser;
    Executor executor;

    // Lexical analysis
    vector<Token> tokens;
    FuncStatus flag = scanner.Scan(line, &tokens);
    if (flag != FuncStatus::Success)
    {
        return flag;
    }
    
    // Syntax analysis
    vector<CommandGroup> command_list;
    flag = parser.Parse(tokens, &command_list);
    if (flag != FuncStatus::Success)
    {
        return flag;
    }

    // Command execution
    flag = executor.Execute(command_list);
    if (flag != FuncStatus::Success)
    {
        return flag;
    }

    return FuncStatus::Success;
}
}  // namespace ghk
