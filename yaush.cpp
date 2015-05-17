/*************************************************************************
    > File Name: yaush.cpp
    > Author: Guo Hengkai
    > Description: Shell class implementation for YAUSH
    > Created Time: Sun 17 May 2015 03:22:24 PM CST
 ************************************************************************/
#include "yaush.h"

namespace ghk
{
Yaush* Yaush::GetInstance()
{
    static Yaush instance_;
    return &instance_;
}

void Yaush::Loop()
{

}
}  // namespace ghk
