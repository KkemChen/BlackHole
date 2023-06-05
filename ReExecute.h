#pragma once
#include <chrono>

template<typename Func, typename... Args>
void ReExecute(std::chrono::milliseconds interval, Func func, Args&&... args)
{
    // 等待指定时间间隔
    std::this_thread::sleep_for(interval);

    // 执行指定函数
    func(std::forward<Args>(args)...);
}


template<typename Func, typename... Args>
bool RetryUntilSuccess(std::chrono::seconds interval, int max_retry_times,
    Func func, Args&&... args)
{
    int retry_count = 0;
    while (retry_count < max_retry_times) {
        // 执行指定函数
        if (func(std::forward<Args>(args)...)) {
            return true;
        }
        ++retry_count;

        // 等待指定时间间隔
        std::this_thread::sleep_for(interval);
    }

    return false;
}