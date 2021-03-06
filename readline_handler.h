/*************************************************************************
    > File Name: readline_handler.h
    > Author: Guo Hengkai
    > Description: GNU Readline library handler class definition for YAUSH
    > Created Time: Sat 23 May 2015 10:45:51 AM CST
 ************************************************************************/
#ifndef YAUSH_READLINE_HANDLER_H_
#define YAUSH_READLINE_HANDLER_H_

#include <string>
#include <vector>

namespace ghk
{
class ReadlineHandler
{
public:
    ReadlineHandler();
    ~ReadlineHandler();
    char* Gets(bool is_reset);
    bool GetHistoryList(std::vector<std::string> *history_vec);

private:
    char *line_read_;
};
}  // namespace ghk

#endif  // YAUSH_READLINE_HANDLER_H_

