/*************************************************************************
    > File Name: yaush.h
    > Author: Guo Hengkai
    > Description: Shell class definition for YAUSH
    > Created Time: Sat 16 May 2015 10:36:37 PM CST
 ************************************************************************/
#ifndef YAUSH_YAUSH_H_
#define YAUSH_YAUSH_H_

#include <string>
#include <vector>
#include "common.h"
#include "readline_handler.h"

namespace ghk
{
class Yaush
{
public:
    static Yaush* GetInstance();
    static void Loop();

private:
    Yaush() {}  // Never be instantiated
    static FuncStatus Analysis(const std::string &line);
};
}  // namespace ghk

#endif  // YAUSH_YAUSH_H_
