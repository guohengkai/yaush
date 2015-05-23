/*************************************************************************
    > File Name: parser.cpp
    > Author: Guo Hengkai
    > Description: Syntax analysis class definition for YAUSH
    > Created Time: Sun 17 May 2015 03:35:02 PM CST
 ************************************************************************/
#include "parser.h"
#include <string>
#include <vector>
#include "command.h"
#include "common.h"

using std::string;
using std::vector;

namespace ghk
{
Parser::Parser()
{

}

FuncStatus Parser::Parse(const vector<Token> &tokens,
        vector<CommandGroup> *command_list)
{
    return FuncStatus::Error;
}
}  // namespace ghk
