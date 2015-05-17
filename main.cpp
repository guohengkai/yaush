/*************************************************************************
    > File Name: main.cpp
    > Author: Guo Hengkai
    > Description: Main program for YAUSH
    > Created Time: Sat 16 May 2015 10:34:26 PM CST
 ************************************************************************/
#include "yaush.h"

using ghk::Yaush;

int main()
{
    Yaush *shell = Yaush::GetInstance();
    shell->Loop();
    return 0;
}
