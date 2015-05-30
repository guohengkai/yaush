/*************************************************************************
    > File Name: scanner.cpp
    > Author: Guo Hengkai
    > Description: Lexical analysis class implementation for YAUSH using FSM
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

// Use finite state machine for lexical analysis
// Support symbols: \ | & > < (space) " ' ;
FuncStatus Scanner::Scan(const string &line, vector<Token> *tokens)
{
    if (tokens == nullptr)
    {
        return FuncStatus::Error;
    }

    tokens->clear();
    Token token{"", false};
    ScanStage stage = ScanStage::Normal;
    char quote;

    for (size_t i = 0; i < line.length(); ++i)
    {
        char ch = line[i];
        switch (stage)
        {
            case ScanStage::Normal:
                if (IsSpace(ch))
                {
                    AddToken(token, tokens);
                }
                else if (IsQuote(ch))
                {
                    quote = ch;
                    stage = ScanStage::Quote;
                }
                else if (IsSpecial(ch))
                {
                    AddToken(token, tokens);
                    token.word += ch;
                    token.is_special = true;
                    stage = ScanStage::Special;
                }
                else if (IsBreak(ch))  // Line break
                {
                    AddToken(token, tokens);
                    stage = ScanStage::End;
                }
                else if (IsEscape(ch))
                {
                    stage = ScanStage::Escape;
                }
                else
                {
                    token.word += ch;
                }
                break;
            case ScanStage::Special:  // 5 symbols: | & ; > <
                if (IsSpecial(ch))
                {
                    token.word += ch;
                }
                else
                {
                    AddToken(token, tokens);
                    stage = ScanStage::Normal;
                    --i;  // Use normal stage to deal with current char
                }
                break;
            case ScanStage::Quote:  // Escape with quote
                if (ch == quote)  // End escape
                {
                    stage = ScanStage::Normal;
                }
                else
                {
                    token.word += ch;
                }
                break;
            case ScanStage::Escape:  // Escape with backslash
                token.word += ch;
                stage = ScanStage::Normal;
                break;
            default:
                // Impossible to reach
                return FuncStatus::Error;
        }
    }

    if (stage == ScanStage::End)
    {
#ifdef DEBUG
        PrintTokens(*tokens);
#endif
        return FuncStatus::Success;
    }
    else  // Need more input
    {
#ifdef DEBUG
        PrintStage(stage);
#endif
        return FuncStatus::WaitInput;
    }
}

void Scanner::AddToken(Token &token, vector<Token> *tokens)
{
    if (!token.word.empty())  // Ignore space
    {
        tokens->push_back(token);
        token.word = "";
        token.is_special = false;
    }
}

#ifdef DEBUG
void Scanner::PrintTokens(const vector<Token> &tokens)
{
    printf("Total tokens: %zu\n", tokens.size());
    for (auto token: tokens)
    {
        if (token.is_special) printf("[");
        for (auto ch: token.word)
        {
            if (IsBreak(ch))
            {
                printf("\\n");
            }
            else
            {
                printf("%c", ch);
            }
        }
        if (token.is_special) printf("]");
        printf("\n");
    }
    string line(35, '-');
    printf("%s\n", line.c_str());
}

void Scanner::PrintStage(ScanStage stage)
{
    switch (stage)
    {
        case ScanStage::Normal:
            printf("Normal stage.\n");
            break;
        case ScanStage::Special:
            printf("Special stage.\n");
            break;
        case ScanStage::Quote:
            printf("Quote stage.\n");
            break;
        case ScanStage::Escape:
            printf("Escape stage.\n");
            break;
        case ScanStage::End:
            printf("End stage.\n");
            break;
        default:
            printf("Unknown stage.\n");
            break;
    }
}
#endif
}  // namespace ghk
