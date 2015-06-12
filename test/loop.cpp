/*************************************************************************
    > File Name: loop.cpp
    > Author: Guo Hengkai
    > Description: Test background job for YAUSH
    > Created Time: Thu 11 Jun 2015 10:19:39 PM CST
 ************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

int main(int argc, char **argv)
{
    long long cnt = 0;
    long long limit = 1000000000000;
    if (argc > 1)
    {
        limit = atoi(argv[1]);
    }

    while (cnt < limit)
    {
        printf("Loop: %lld\n", cnt++);
        sleep(2);
    }

    return 0;
}
