/*************************************************************************
    > File Name: error_util.cpp
    > Author: Guo Hengkai
    > Description: Error util function implementations
    > Created Time: Fri 29 May 2015 11:38:17 PM CST
 ************************************************************************/
#include "error_util.h"
#include <cstdio>
#include <string>
#include "common.h"

using std::string;

namespace ghk
{
void ErrorPrint(ShellError code, const string &info)
{
#ifdef DEBUG
    printf("Error: %d\n", code);
#endif
    string title, content;
    switch (code)
    {
        case ShellError::Syntax:
            title = SHELL_NAME;
            content = "unexpected token '" + info + "'";
            break;
        case ShellError::InvalidName:
            title = SHELL_NAME;
            content = "invalid " + info + " file name";
            break;
        case ShellError::InvalidEnd:
            title = SHELL_NAME;
            content = "invalid end with '" + info + "'";
            break;
        case ShellError::CmdNotFound:
            title = info;
            content = "command not found";
            break;
        case ShellError::PipeCreation:
            title = SHELL_NAME;
            content = "fail to create pipes for '" + info + "'";
            break;
        case ShellError::ForkFail:
            title = SHELL_NAME;
            content = "fail to fork process for '" + info + "'";
            break;
        case ShellError::FileError:
            title = SHELL_NAME;
            content = "fail to open file " + info;
            break;
        case ShellError::ExecError:
            title = SHELL_NAME;
            content = info;
            break;
        case ShellError::UnknownError:
            title = SHELL_NAME;
            content = "unknown error";
            if (!info.empty())
            {
                content += " with " + info;
            }
            break;
        default:
            title = SHELL_NAME;
            content = "unknown error";
    }

    fprintf(stderr, "%s: %s\n", title.c_str(), content.c_str());
}
}  // namespace ghk
