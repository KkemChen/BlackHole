#pragma once
#include "Ver.h"

#include <functional>
#include <iomanip>
#include <iostream>
#include <shared_mutex>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
extern "C" const IMAGE_DOS_HEADER __ImageBase;
#else
#include <unistd.h>
#endif // defined(_WIN32)

#define wlock_(x) std::unique_lock<std::shared_timed_mutex> writerLock(x)
#define rlock_(x) std::shared_lock<std::shared_timed_mutex> readLock(x)
#define lock_(x) std::lock_guard<std::recursive_mutex> __guard__(x)

#undef RWMutex
#undef ExMutex
using RWMutex = std::shared_timed_mutex;
using ExMutex = std::recursive_mutex;


template<typename T, size_t N>
constexpr size_t ArraySize(T(&)[N]) {
    return N;
}


namespace kkem
{
    void printVersionInfo();    //输出由编译脚本生成的git信息等

    std::string getGitCommitHash();     //获取git_commit哈希值
    std::string getVersion();           //获取版本号

    std::string appPath(bool isExe = true);
    std::string appDir(bool isExe = true);
    std::string appName(bool isExe = true);

}//namespace kkem end



namespace kkem
{
    class semaphore
    {
    public:
        explicit semaphore(size_t initial = 0) { _count = 0; }

        ~semaphore() { }

        void post(size_t n = 1);

        void wait();

    private:
        size_t _count;
        std::recursive_mutex _mutex;
        std::condition_variable_any _condition;
    };
}//namespace kkem end
    




