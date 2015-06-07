/*************************************************************************
    > File Name: command_registry.cpp
    > Author: Guo Hengkai
    > Description: Custom command factory class implementation for YAUSH
    > Created Time: Sun 17 May 2015 02:14:43 PM CST
 ************************************************************************/
#include "command_registry.h"
#include <map>
#include <string>

using std::map;
using std::string;
using std::vector;

namespace ghk
{
string CommandRegistry::error_info_;

CommandRegistry::CmdRegistry& CommandRegistry::Registry()
{
    static CmdRegistry* registry_ = new CmdRegistry();
    return *registry_;
}

void CommandRegistry::AddCommand(const string &name, CmdExecute cmd)
{
    CmdRegistry &registry = Registry();
    registry[name] = cmd;
}

CmdStatus CommandRegistry::ExecuteCommand(const string &name,
            const vector<string> &argv)
{
    CmdRegistry &registry = Registry();
    if (registry.count(name) != 1)
    {
        return CmdStatus::Notfound;
    }
    else
    {
        return (registry[name](name, argv) ? CmdStatus::Ok
                                           : CmdStatus::Fail);
    }
}

string CommandRegistry::CommandList()
{
    CmdRegistry &registry = Registry();
    string result;
    int cnt = 0;
    for (auto iter = registry.begin(); iter != registry.end(); ++iter, ++cnt)
    {
        if (cnt > 0)
        {
            result += ", ";
        }
        result += iter->first;
    }
    return result;
}

bool CustomWhat(const string &name, const vector<string> &argv)
{
    string result = CommandRegistry::CommandList();
    printf("Support custom commands: %s\n", result.c_str());
    return true;
}

bool CustomCd(const string &name, const vector<string> &argv)
{
    return false;
}

bool CustomAbout(const string &name, const vector<string> &argv)
{
    printf("****************************************************\n");
    printf("  > Shell Name: YAUSH\n");
    printf("  > Version:    1.0\n");
    printf("  > Author:     Guo Hengkai\n");
    printf("  > Email:      guohengkaighk@gmail.com\n");
    printf("  > Date:       2015.06.\n");
    printf("  > Website:    https://github.com/guohengkai/yaush\n");
    printf("****************************************************\n");
    return true;
}

bool CustomExit(const string &name, const vector<string> &argv)
{
    return false;
}

bool CustomJobs(const string &name, const vector<string> &argv)
{
    return false;
}

bool CustomFg(const string &name, const vector<string> &argv)
{
    return false;
}

bool CustomBg(const string &name, const vector<string> &argv)
{
    return false;
}

bool CustomImage(const string &name, const vector<string> &argv)
{
    return false;
}

bool CustomLoop(const string &name, const vector<string> &argv)
{
    return false;
}
}  // namespace ghk
