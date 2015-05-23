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
#include "common.h"

namespace ghk
{
class Scanner
{
public:
    Scanner();
    FuncStatus Scan(const std::string &line, std::vector<Token> *tokens);
private:
    enum ScanStage
    {
        Normal = 0,
        Special,
        Quote,
        Escape,
        End
    };

    inline bool IsSpace(char ch)
    {
        return ch == ' ';
    }
    inline bool IsQuote(char ch)
    {
        return ch == '\'' || ch == '"';
    }
    inline bool IsSpecial(char ch)
    {
        return ch == '|' || ch == '&' || ch == ';';
    }
    inline bool IsBreak(char ch)
    {
        return ch == '\n';
    }
    inline bool IsEscape(char ch)
    {
        return ch == '\\';
    }
    void AddToken(Token &token, std::vector<Token> *tokens);
#ifdef DEBUG
    void PrintTokens(const std::vector<Token> &tokens);
    void PrintStage(ScanStage stage);
#endif
};
}  // namespace ghk

#endif  // YAUSH_SCANNER_H_
