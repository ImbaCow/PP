#define NOMINMAX
#include <windows.h>
#include <string>
#include <iostream>
#include <memory>
#include <ctime>
#include <fstream>
#include <limits>

struct ThreadParam
{
	size_t num;
	std::clock_t c_start;
	std::string outputName;
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	ThreadParam* value = (ThreadParam*)lpParam;
	std::cout << "Поток №" << value->num << " выполняет свою работу" << std::endl;
	std::ofstream output(value->outputName);

	for (uint16_t i = 0; i < std::numeric_limits<uint16_t>::max(); i++)
	{
		std::clock_t currTime = std::clock();
		output << currTime - value->c_start << std::endl;
	}

	delete(value);
	ExitThread(0); // функция устанавливает код завершения потока в 0
}

int main(int argc, char* argv[])
{
	std::clock_t c_start = std::clock();

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	size_t threadCount = 2u;
	HANDLE* handles = new HANDLE[threadCount];

	uint64_t affinityMask = 0b1;
	for (size_t i = 0; i < threadCount; i++)
	{
		ThreadParam* value = new ThreadParam();
		value->num = i + 1;
		value->c_start = c_start;
		value->outputName = "out" + std::to_string(i) + ".txt";
		handles[i] = CreateThread(NULL, 0, &ThreadProc, (LPVOID)value, CREATE_SUSPENDED, NULL);
		SetThreadAffinityMask(handles[i], affinityMask);

	}

	for (size_t i = 0; i < threadCount; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(threadCount, handles, true, INFINITE);
	return 0;
}
