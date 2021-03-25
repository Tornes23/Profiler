#include <iostream>
#include "src/Profiler.h"


void Dummy()
{
	PROFILE("Dummy")
}

void Dummy2()
{
	PROFILER_ENTER("Dummy2")
	Dummy();
	PROFILER_LEAVE
}

void Dummy3()
{
	PROFILE("Dummy3")
}

void Dummy4()
{
	PROFILER_ENTER("Dummy4")
	PROFILER_LEAVE
}



int main(void)
{
	PROFILE("main")
	
	Dummy();
	Dummy2();
	Dummy3();
	Dummy4();

	PRINT_LOG("ProfilerLog.txt")
	return 0;
}