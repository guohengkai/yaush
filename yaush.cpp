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

    while (true)
    {
        line_read = handler_.Gets();
        if (!line_read)  // EOF
        {
            printf("\n");
            break;
        }
        else if (*line_read)  // Not empty string
        {
            Analysis(string(line_read));
        }
    }
}

bool Yaush::Analysis(const string &line)
{
    Scanner scanner;
    Parser parser;
    Executor executor;

    // Lexical analysis
    vector<string> tokens;
    if (!scanner.Scan(line, &tokens))
    {
        return false;
    }
    
    // Syntax analysis
    vector<CommandGroup> command_list;
    if (!parser.Parse(tokens, &command_list))
    {
        return false;
    }

    // Command execution
    if (!executor.Execute(command_list))
    {
        return false;
    }

    return true;
}
}  // namespace ghk
