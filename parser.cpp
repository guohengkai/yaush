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
#include "error_util.h"

using std::string;
using std::vector;

namespace ghk
{
Parser::Parser()
{

}

// Use finite state machine for syntax analysis
FuncStatus Parser::Parse(const vector<Token> &tokens,
        vector<CommandGroup> *command_list)
{
    if (command_list == nullptr)
    {
        return FuncStatus::Error;
    }

    command_list->clear();
    ParseStage stage = ParseStage::Empty;
    CommandGroup group;
    Command cmd;
    bool redirect_input;
    string last_syntax;

    for (auto token: tokens)
    {
        switch (stage)
        {
            case ParseStage::Empty:
                if (token.is_special)  // Can not start with special syntax
                {
                    ErrorPrint(ShellError::Syntax, token.word);
                    return FuncStatus::Error;
                }
                else
                {
                    cmd.name = token.word;
                    cmd.arg_list.push_back(token.word);
                    stage = ParseStage::Normal;
                    group.str += cmd.name + " ";
                }
                break;
            case ParseStage::Normal:
                if (token.is_special)
                {
                    if (IsSupport(token.word))
                    {
                        if (IsRedirect(token.word))  // Redirect with > and <
                        {
                            stage = ParseStage::Wait;
                            redirect_input = (token.word == "<");
                            group.str += token.word + " ";
                        }
                        else if (token.word == "|")  // Pipe
                        {
                            if (cmd.io_type[1] == CommandIOType::Standard)
                            {
                                // Without redirecting
                                cmd.io_type[1] = CommandIOType::Pipe;
                            }
                            group.cmd.push_back(cmd);
                            cmd = Command();
                            cmd.io_type[0] = CommandIOType::Pipe;
                            stage = ParseStage::Empty;
                            group.str += token.word + " ";
                        }
                        else  // || && ; &
                        {
                            group.cmd.push_back(cmd);
                            cmd = Command();

                            if (token.word == "||")
                            {
                                group.logic = CommandLogic::Or;
                            }
                            else if (token.word == "&&")
                            {
                                group.logic = CommandLogic::And;
                            }
                            else if (token.word == "&")
                            {
                                group.logic = CommandLogic::Background;
                            }

                            group.str.pop_back();
                            command_list->push_back(group);
                            group = CommandGroup();
                            stage = ParseStage::Empty;
                        }

                        last_syntax = token.word;
                    }
                    else  // Undefined syntax
                    {
                        ErrorPrint(ShellError::Syntax, token.word);
                        return FuncStatus::Error;
                    }
                }
                else  // Arguments
                {
                    cmd.arg_list.push_back(token.word);
                    group.str += token.word + " ";
                }
                break;
            case ParseStage::Wait:
                if (token.is_special)  // Need file name, not syntax
                {
                    ErrorPrint(ShellError::InvalidName,
                            redirect_input ? "input" : "output");
                    return FuncStatus::Error;
                }
                else
                {
                    int idx = redirect_input ? 0 : 1;
                    cmd.io_type[idx] = CommandIOType::File;
                    cmd.io_file_name[idx].push_back(token.word);
                    stage = ParseStage::Normal;
                    group.str += token.word + " ";
                }
                break;
            default:
                return FuncStatus::Error;
        }
    }
    
    if (stage == ParseStage::Empty
            && (last_syntax != ";" && last_syntax != "&"))
    {
        ErrorPrint(ShellError::InvalidEnd, last_syntax);
        return FuncStatus::Error;
    }
    else if (stage == ParseStage::Wait)
    {
        ErrorPrint(ShellError::InvalidName,
                redirect_input ? "input" : "output");
        return FuncStatus::Error;
    }
    else  // Success
    {
        if (stage == ParseStage::Normal)
        {
            group.str.pop_back();
            group.cmd.push_back(cmd);
            command_list->push_back(group);
        }
#ifdef DEBUG
        PrintCommandList(*command_list);
#endif
        return FuncStatus::Success;
    }
}

#ifdef DEBUG
void Parser::PrintCommandList(const vector<CommandGroup> &command_list)
{
    printf("Total command groups: %zu\n", command_list.size());
    string space_cmd(10, ' ');
    size_t group_idx = 0;
    for (auto group: command_list)
    {
        size_t cmd_idx = 0;
        printf("{ cmds: ");
        for (auto cmd: group.cmd)
        {
            if (cmd_idx != 0)
            {
                printf("\n%s", space_cmd.substr(2).c_str());
            }
            printf("[ name: '%s'", cmd.name.c_str());
            if (!cmd.arg_list.empty())
            {
                printf("\n%sarg: ", space_cmd.c_str());
                for (size_t i = 0; i < cmd.arg_list.size() - 1; ++i)
                {
                    printf("'%s', ", cmd.arg_list[i].c_str());
                }
                printf("'%s'", cmd.arg_list.back().c_str());
            }
            for (int idx = 0; idx < 2; ++idx)
            {
                if (cmd.io_type[idx] == CommandIOType::Standard)
                {
                    continue;
                }
                printf("\n%s%s: ", space_cmd.c_str(), idx == 0 ? "in" : "out");
                if (cmd.io_type[idx] == CommandIOType::Pipe)
                {
                    printf("PIPE");
                }
                else
                {
                    for (size_t i = 0;
                            i < cmd.io_file_name[idx].size() - 1; ++i)
                    {
                        printf("'%s', ", cmd.io_file_name[idx][i].c_str());
                    }
                    printf("'%s'", cmd.io_file_name[idx].back().c_str());
                }
            }
            if (cmd_idx != group.cmd.size() - 1)
            {
                printf(" ] -->");
            }
            else
            {
                printf(" ]");
            }
            ++cmd_idx;
        }

        printf("\n  str: %s", group.str.c_str());

        if (group.logic != CommandLogic::Empty)
        {
            printf("\n  logic: ");
            switch (group.logic)
            {
                case CommandLogic::And:
                    printf("AND");
                    break;
                case CommandLogic::Or:
                    printf("OR");
                    break;
                case CommandLogic::Background:
                    printf("BG");
                    break;
                default:
                    printf("ERROR");
            }
        }
        if (group_idx != command_list.size() - 1)
        {
            printf(" } -------->\n");
        }
        else
        {
            printf(" }\n");
        }
        ++group_idx;
    }
    string line(35, '-');
    printf("%s\n", line.c_str());
}
#endif
}  // namespace ghk
