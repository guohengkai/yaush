/*************************************************************************
    > File Name: parser.h
    > Author: Guo Hengkai
    > Description: Syntax analysis class definition for YAUSH
    > Created Time: Sun 17 May 2015 10:58:10 AM CST
 ************************************************************************/
#ifndef YAUSH_PARSER_H_
#define YAUSH_PARSER_H_

#include <vector>
#include "command.h"
#include "common.h"

namespace ghk
{
class Parser
{
public:
    Parser();
    FuncStatus Parse(const std::vector<Token> &tokens,
            std::vector<CommandGroup> *command_list);

private:
    enum ParseStage
    {
        Empty = 0,
        Normal,
        Wait
    };

    inline bool IsSupport(const std::string &str)
    {
        return str == "|" || str == "<" || str == ">" || str == "&"
            || str == ";" || str == "&&" || str == "||";
    }
    
    inline bool IsRedirect(const std::string &str)
    {
        return str == ">" || str == "<";
    }

#ifdef DEBUG
    void PrintCommandList(const std::vector<CommandGroup> &command_list);
#endif
};
}  // namespace ghk

#endif  // YAUSH_PARSER_H_
