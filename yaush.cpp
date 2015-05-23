/*************************************************************************
    > File Name: yaush.cpp
    > Author: Guo Hengkai
    > Description: Shell class implementation for YAUSH
    > Created Time: Sun 17 May 2015 03:22:24 PM CST
 ************************************************************************/
#include "yaush.h"
#include "readline_handler.h"

namespace ghk
{
Yaush* Yaush::GetInstance()
{
    static Yaush instance_;
    return &instance_;
}

void Yaush::Loop()
{
    static ReadlineHandler handler_;
    char* line_read = nullptr;

    while (true)
    {
        line_read = handler_.Gets();
        if (!line_read)  // EOF
        {
            printf("\n");
            break;
        }
        else if (*line_read)  // Not empty string
        {
            printf("%s\n", line_read);
        }
    }
}
}  // namespace ghk
