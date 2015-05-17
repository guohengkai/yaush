/*************************************************************************
    > File Name: parser.h
    > Author: Guo Hengkai
    > Description: Syntax analysis class definition for YAUSH
    > Created Time: Sun 17 May 2015 10:58:10 AM CST
 ************************************************************************/
#ifndef YAUSH_PARSER_H_
#define YAUSH_PARSER_H_

#include <string>
#include <vector>
#include "command.h"

namespace ghk
{
class Parser
{
public:
    Parser();
    bool Parse(const std::vector<std::string> &tokens,
            std::vector<CommandGroup> *command_list);
};
}  // namespace ghk

#endif  // YAUSH_PARSER_H_
