#pragma once

#include <mutex>
#include <functional>
#include <type_traits>

namespace kkem
{
    template <class T, int X = 0, int N = 1, class... Args>
    class SingletonPtr
    {
    public:
        static T* GetInstance(Args&&... args)
        {
            std::call_once(flag_, &SingletonPtr::Init<Args&&...>, std::forward<Args>(args)...);
            return instance_;
        }

    private:
        static void Destroy()
        {
            instance_->~T();
        }

        /* C++ 17 可直接用if constexpr
        template <class... ArgsT>
        static void Init(ArgsT&&... args)
        {
            using StorageType = std::aligned_storage_t<sizeof(T), alignof(T)>;
            static StorageType storage;
            if constexpr (std::is_constructible_v<T, ArgsT...>) {
                instance_ = new (&storage) T(std::forward<ArgsT>(args)...);
            } else {
                instance_ = new (&storage) T();
            }
            std::atexit(&SingletonPtr::Destroy);
        }
        */

        template <class... ArgsT>
        static auto Init(ArgsT&&... args) ->
            std::enable_if_t<std::is_constructible<T, ArgsT...>::value, void>
        {
            using StorageType = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
            static StorageType storage;
            instance_ = new (&storage) T(std::forward<ArgsT>(args)...);
            std::atexit(&SingletonPtr::Destroy);
        }

        template <class... ArgsT>
        static auto Init(ArgsT&&...) ->
            std::enable_if_t<!std::is_constructible<T, ArgsT...>::value, void>
        {
            using StorageType = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
            static StorageType storage;
            instance_ = new (&storage) T();
            std::atexit(&SingletonPtr::Destroy);
        }

        static std::once_flag flag_;
        static T* instance_;
    };

    template <class T, int X, int N, class... Args>
    std::once_flag SingletonPtr<T, X, N, Args...>::flag_;

    template <class T, int X, int N, class... Args>
    T* SingletonPtr<T, X, N, Args...>::instance_ = nullptr;

};//namespace kkem

