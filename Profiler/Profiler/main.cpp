#include <iostream>
#include "src/Profiler.h"



void Dummy2()
{
	Prof.Enter("Dummy2");
	Prof.Leave();
}

void Dummy3()
{
	Prof.Enter("Dummy3");
	Prof.Leave();
}

void Dummy()
{
	Prof.Enter("Dummy");
	Dummy2();
	Prof.Leave();
}

int main(void)
{
	Prof.Enter("main");
	Dummy();
	Dummy3();

	std::cout << "Lel\n";
	return 0;
}