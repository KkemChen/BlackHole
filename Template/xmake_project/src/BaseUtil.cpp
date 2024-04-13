#include "BaseUtil.h"
#include "Log.h"

std::string kkem::appInfo() {
    // ANSI Escape Codes for text colors
#define RESET "\033[0m"
#define GREEN "\033[32m"
    std::stringstream ss;
    ss << GREEN <<
        R"( 
              _____                       
             |  __ \                      
             | |  | | ___ _ __ ___   ___  
             | |  | |/ _ \ '_ ` _ \ / _ \ 
             | |__| |  __/ | | | | | (_) |
             |_____/ \___|_| |_| |_|\___/ 
                                            )" << RESET << "\n" << std::endl;



    ss << "============================ ⭐Version Info⭐ ==============================" << std::endl;
    ss << std::setw(20) << std::left << "\t[Version]:" << std::setw(20) << APP_VERSION << std::endl;
    ss << std::setw(20) << std::left << "\t[Branch]:" << std::setw(20) << APP_GIT_BRANCH << std::endl;
    ss << std::setw(20) << std::left << "\t[Commit]:" << std::setw(20) << APP_GIT_COMMIT_LONG << std::endl;
    ss << std::setw(20) << std::left << "\t[Tag]:" << std::setw(20) << APP_GIT_TAG << std::endl;
    ss << std::setw(20) << std::left << "\t[Arch]:" << std::setw(20) << APP_ARCH << std::endl;
    ss << std::setw(20) << std::left << "\t[System]:" << std::setw(20) << APP_PLAT << std::endl;
    ss << std::setw(20) << std::left << "\t[Build Mode]:" << std::setw(20) << APP_MODE << std::endl;
    ss << std::setw(20) << std::left << "\t[Build Date]:" << std::setw(20) << APP_VERSION_BUILD << std::endl;
    ss << "============================================================================" << "\n\n" << std::endl;

#undef RESET
#undef GREEN
    return ss.str();
}

std::string kkem::gitCommitHash()
{
    return APP_GIT_COMMIT_LONG;
}

std::string kkem::appVersion()
{
    return APP_VERSION;
}

std::string kkem::appPath(bool isExe) {
    char buffer[1024 * 2 + 1] = { 0 };
    int n                     = -1;
#if defined(_WIN32)
    n = GetModuleFileNameA(isExe ? nullptr : (HINSTANCE)&__ImageBase, buffer, sizeof(buffer));
#elif defined(__MACH__) || defined(__APPLE__)
        n = sizeof(buffer);
        if (uv_exepath(buffer, &n) != 0) {
            n = -1;
        }
#elif defined(__linux__)
        n = readlink("/proc/self/exe", buffer, sizeof(buffer));
#endif

    std::string filePath;
    if (n <= 0) {
        filePath = "./";
    }
    else {
        filePath = buffer;
    }

#if defined(_WIN32)
    //windows下把路径统一转换层unix风格，因为后续都是按照unix风格处理的
    for (auto& ch : filePath) {
        if (ch == '\\') {
            ch = '/';
        }
    }
#endif //defined(_WIN32)

    return filePath;
}

std::string kkem::appDir(bool isExe) {
    auto path = appPath(isExe);
    return path.substr(0, path.rfind('/') + 1);
}

std::string kkem::appName(bool isExe) {
    auto path = appPath(isExe);
    return path.substr(path.rfind('/') + 1);
}

void kkem::semaphore::post(size_t n) {
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    _count += n;
    if (n == 1) {
        _condition.notify_one();
    }
    else {
        _condition.notify_all();
    }
}

void kkem::semaphore::wait() {
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    while (_count == 0) {
        _condition.wait(lock);
    }
    --_count;
}

kkem::TaskManager::~TaskManager()
{
    stopThread(true);
}

void kkem::TaskManager::setMaxTaskSize(size_t size)
{
    if (size < 3 || size >1000) {
        LOGFATAL() << "Async task size limited to 3 ~ 1000, now size is: " << size;
        throw std::runtime_error("Async task size is limited to 3 ~ 1000, but the provided size is: " + std::to_string(size));
    }

    _max_task = size;

}

void kkem::TaskManager::startThread(const std::string& name)
{
    _thread_name = name;
    _thread.reset(new std::thread([this]() {
        onThreadRun();
        }), [](std::thread* ptr) {
            ptr->join();
            delete ptr;
        });
}

void kkem::TaskManager::stopThread(bool drop_task)
{
    if (!_thread) {
        return;
    }
    {
        std::lock_guard<std::mutex> lck(_task_mtx);
        if (drop_task) {
            _exit = true;
            _task.clear();
        }
        _task.emplace_back([]() {
            throw ThreadExitException();
            });
    }
    _sem.post(10);
    _thread = nullptr;

}

bool kkem::TaskManager::addTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lck(_task_mtx);
        _task.emplace_back(std::move(task));
        if (_task.size() > _max_task) {
            LOGWARN() << "This thread task is too more, now drop task!";
            _task.pop_front();
        }
    }
    _sem.post();
    return true;

}

bool kkem::TaskManager::isEnabled() const
{
    return _thread.operator bool();
}

void kkem::TaskManager::onThreadRun()
{
    std::function<void()> task;
    _exit = false;
    while (!_exit) {
        _sem.wait();
        {
            std::unique_lock<std::mutex> lck(_task_mtx);
            if (_task.empty()) {
                continue;
            }
            task = _task.front();
            _task.pop_front();
        }

        try {
            task();
            task = nullptr;
        }
        catch (ThreadExitException& ex) {
            break;
        }
        catch (std::exception& ex) {
            LOGWARN() << ex.what();
            continue;
        }
        catch (...) {
            LOGWARN() << "Catch one unknown exception";
            throw;
        }
    }
    LOGINFO() << _thread_name << " exited!";

}
