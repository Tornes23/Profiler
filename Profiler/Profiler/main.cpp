#include <iostream>
#include "src/Profiler.h"


void Dummy()
{
	PROFILE("Dummy")
	PROFILER_LEAVE
}

void Dummy2()
{
	PROFILE("Dummy2")
	Dummy();
	PROFILER_LEAVE
}

void Dummy3()
{
	PROFILE("Dummy3")
	PROFILER_LEAVE
}

void Dummy4()
{
	PROFILE("Dummy4")
	PROFILER_LEAVE
}



int main(void)
{
	PROFILE("main")

		auto& test = ProfilerSystem::Instance();

	Dummy();
	Dummy2();
	Dummy3();
	Dummy4();

	PRINT_LOG("ProfilerLog.txt")
	return 0;
}