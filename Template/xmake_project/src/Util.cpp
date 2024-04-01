#include "Util.h"

void kkem::printVersionInfo() {
    // ANSI Escape Codes for text colors
#define RESET "\033[0m"
#define GREEN "\033[32m"

    std::cout << GREEN <<
        R"( 
              _____                       
             |  __ \                      
             | |  | | ___ _ __ ___   ___  
             | |  | |/ _ \ '_ ` _ \ / _ \ 
             | |__| |  __/ | | | | | (_) |
             |_____/ \___|_| |_| |_|\___/ 
                                            )" << RESET << "\n" << std::endl;



    std::cout << "============================ ⭐Version Info⭐ ==============================" << std::endl;
    std::cout << std::setw(20) << std::left << "\t[Version]:" << std::setw(20) << APP_VERSION << std::endl;
    std::cout << std::setw(20) << std::left << "\t[Branch]:" << std::setw(20) << APP_GIT_BRANCH << std::endl;
    std::cout << std::setw(20) << std::left << "\t[Commit]:" << std::setw(20) << APP_GIT_COMMIT_LONG << std::endl;
    std::cout << std::setw(20) << std::left << "\t[Tag]:" << std::setw(20) << APP_GIT_TAG << std::endl;
    std::cout << std::setw(20) << std::left << "\t[Arch]:" << std::setw(20) << APP_ARCH << std::endl;
    std::cout << std::setw(20) << std::left << "\t[System]:" << std::setw(20) << APP_PLAT << std::endl;
    std::cout << std::setw(20) << std::left << "\t[Build Mode]:" << std::setw(20) << APP_MODE << std::endl;
    std::cout << std::setw(20) << std::left << "\t[Build Date]:" << std::setw(20) << APP_VERSION_BUILD << std::endl;
    std::cout << "============================================================================" << "\n\n" << std::endl;

#undef RESET
#undef GREEN

}

std::string kkem::getGitCommitHash()
{
    return APP_GIT_COMMIT_LONG;
}

std::string kkem::getVersion()
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

