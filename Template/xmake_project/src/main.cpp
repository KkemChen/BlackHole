#include <iostream>
#include "Log.h"
#include "Util.h"
#include <csignal>

using namespace kkem;
int main(int argc, char *argv[])
{
    kkem::printVersionInfo();

	char sz[] = "Hello, World!";	//Hover mouse over "sz" while debugging to see its contents
	std::cout << sz << std::endl;	//<================= Put a breakpoint here
	kkem::Logger::Get().init("log/demo.log", kkem::STDOUT | kkem::FILEOUT);

	static semaphore sem;
	std::signal(SIGINT, [](int) {
		LOGINFO() << "Exit";
		std::signal(SIGINT, SIG_IGN);
		sem.post();
		});

	sem.wait();
	return 0;
}
