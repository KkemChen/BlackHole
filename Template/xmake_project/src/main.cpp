#include <csignal>
#include <iostream>
#include "Log.h"
#include "BaseUtil.h"
#include <csignal>

using namespace kkem;
int main(int argc, char *argv[])
{
	kkem::Logger::Get().init("log/demo.log", kkem::STDOUT | kkem::FILEOUT);

	LOGTRACE() << "\n" << kkem::appInfo();

	char sz[] = "Hello, World!";
	LOGINFO() << sz;

	static semaphore sem;
	std::signal(SIGINT, [](int) {
		LOGINFO() << "Exit";
		std::signal(SIGINT, SIG_IGN);
		sem.post();
		});

	sem.wait();
	return 0;
}
