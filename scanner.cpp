/*************************************************************************
    > File Name: scanner.cpp
    > Author: Guo Hengkai
    > Description: Lexical analysis class implementation for YAUSH
    > Created Time: Sun 17 May 2015 03:30:08 PM CST
 ************************************************************************/
#include "scanner.h"
#include <string>
#include <vector>
#include "common.h"

using std::string;
using std::vector;

namespace ghk
{
Scanner::Scanner()
{

}

FuncStatus Scanner::Scan(const string &line, vector<string> *tokens)
{
    // Ignore quote and \n in this version
    return FuncStatus::Error;
}
}  // namespace ghk
