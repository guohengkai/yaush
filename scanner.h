/*************************************************************************
    > File Name: scanner.h
    > Author: Guo Hengkai
    > Description: Lexical analysis class definition for YAUSH
    > Created Time: Sun 17 May 2015 10:54:13 AM CST
 ************************************************************************/
#ifndef YAUSH_SCANNER_H_
#define YAUSH_SCANNER_H_

#include <string>
#include <vector>

namespace ghk
{
class Scanner
{
public:
    Scanner();
    bool Scan(const std::string &line, std::vector<std::string> *tokens);
};
}  // namespace ghk

#endif  // YAUSH_SCANNER_H_
