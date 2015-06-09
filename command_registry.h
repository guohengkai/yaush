/*************************************************************************
    > File Name: command_registry.h
    > Author: Guo Hengkai
    > Description: Custom command factory class definition for YAUSH
    > Created Time: Sun 17 May 2015 11:52:15 AM CST
 ************************************************************************/
#ifndef YAUSH_COMMAND_REGISTRY_H_
#define YAUSH_COMMAND_REGISTRY_H_

#include <map>
#include <set>
#include <string>
#include <vector>

namespace ghk
{
enum CmdStatus
{
    Ok = 0,
    Fail,
    Notfound
};

class CommandRegistry
{
public:
    typedef bool (*CmdExecute)(const std::string &name,
            const std::vector<std::string> &argv);
    static CommandRegistry* GetInstance();
    void AddCommand(const std::string &name, CmdExecute cmd,
            bool is_main);
    CmdStatus ExecuteCommand(const std::string &name,
            const std::vector<std::string> &argv);
    std::string CommandList();
    inline const std::string& error_info() { return error_info_; }
    inline void set_error_info(const std::string &info)
    {
        error_info_ = info;
    }
    inline bool IsCommandMain(const std::string &cmd)
    {
        return (main_set_.count(cmd) > 0);
    }

private:
    CommandRegistry() {}  // Never be instantiated
    std::string error_info_;
    std::set<std::string> main_set_;
    std::map<std::string, CmdExecute> registry_;
};

class CommandRegister
{
public:
    CommandRegister(const std::string &name, CommandRegistry::CmdExecute cmd,
                        bool is_main)
    {
        CommandRegistry *registry = CommandRegistry::GetInstance();
        registry->AddCommand(name, cmd, is_main);
    }
};

#define REGISTER_COMMAND(name, cmd, is_main)     \
    static CommandRegister command_##name(#name, cmd, is_main)

bool CustomWhat(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(what, CustomWhat, false);

bool CustomCd(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(cd, CustomCd, true);

bool CustomAbout(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(about, CustomAbout, false);

bool CustomExit(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(exit, CustomExit, true);

bool CustomJobs(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(jobs, CustomJobs, false);

bool CustomFg(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(fg, CustomFg, false);

bool CustomBg(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(bg, CustomBg, false);

bool CustomHistory(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(history, CustomHistory, true);

bool CustomLoop(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(loop, CustomLoop, false);
}  // namespace ghk

#endif  // YAUSH_COMMAND_REGISTRY_H_
