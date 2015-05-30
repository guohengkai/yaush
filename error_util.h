/*************************************************************************
    > File Name: error_util.h
    > Author: Guo Hengkai
    > Description: Error util function definitions
    > Created Time: Fri 29 May 2015 11:33:00 PM CST
 ************************************************************************/
#ifndef YAUSH_ERROR_UTIL_H_
#define YAUSH_ERROR_UTIL_H_

#include <cstdio>
#include <string>

namespace ghk
{
enum ShellError
{
    Syntax = 0,
    InvalidName,
    InvalidEnd,
    NotFound
};

void ErrorPrint(ShellError code, const std::string &info);
}  // namespace ghk

#endif  // YAUSH_ERROR_UTIL_H_

