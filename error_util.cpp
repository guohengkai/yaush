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
        case ShellError::NotFound:
            title = info;
            content = "command not found";
            break;
        default:
            title = SHELL_NAME;
            content = "unknown error";
    }

    fprintf(stderr, "%s: %s\n", title.c_str(), content.c_str());
}
}  // namespace ghk
