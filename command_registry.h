/*************************************************************************
    > File Name: command_registry.h
    > Author: Guo Hengkai
    > Description: Custom command factory class definition for YAUSH
    > Created Time: Sun 17 May 2015 11:52:15 AM CST
 ************************************************************************/
#ifndef YAUSH_COMMAND_REGISTRY_H_
#define YAUSH_COMMAND_REGISTRY_H_

#include <map>
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
    typedef std::map<std::string, CmdExecute> CmdRegistry;

    static CmdRegistry& Registry();
    static void AddCommand(const std::string &name, CmdExecute cmd);
    static CmdStatus ExecuteCommand(const std::string &name,
            const std::vector<std::string> &argv);
    static std::string CommandList();
    inline static const std::string& error_info() { return error_info_; }

private:
    CommandRegistry() {}  // Never be instantiated
    static std::string error_info_;
};

class CommandRegister
{
public:
    CommandRegister(const std::string &name, CommandRegistry::CmdExecute cmd)
    {
        CommandRegistry::AddCommand(name, cmd);
    }
};

#define REGISTER_COMMAND(name, cmd)     \
    static CommandRegister command_##name(#name, cmd)

bool CustomWhat(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(what, CustomWhat);

bool CustomCd(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(cd, CustomCd);

bool CustomAbout(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(about, CustomAbout);

bool CustomExit(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(exit, CustomExit);

bool CustomJobs(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(jobs, CustomJobs);

bool CustomFg(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(fg, CustomFg);

bool CustomBg(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(bg, CustomBg);

bool CustomImage(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(image, CustomImage);

bool CustomLoop(const std::string &name,
        const std::vector<std::string> &argv);
REGISTER_COMMAND(loop, CustomLoop);
}  // namespace ghk

#endif  // YAUSH_COMMAND_REGISTRY_H_
