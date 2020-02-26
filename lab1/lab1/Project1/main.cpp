#include <windows.h>
#include <string>
#include <iostream>
#include <memory>

struct ThreadParam
{
	size_t num;
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	ThreadParam* value = (ThreadParam*)lpParam;
	printf("����� �%i ��������� ���� ������\n", value->num);
	//std::cout << "����� �" << value->num << " ��������� ���� ������" << std::endl;

	delete(value);
	ExitThread(0); // ������� ������������� ��� ���������� ������ � 0
}

int main(int argc, char* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	size_t threadCount = std::stoul(argv[1]);
	HANDLE* handles = new HANDLE[threadCount];

	for (size_t i = 0; i < threadCount; i++)
	{
		ThreadParam* value = new ThreadParam();
		value->num = i + 1;
		handles[i] = CreateThread(NULL, 0, &ThreadProc, (LPVOID)value, CREATE_SUSPENDED, NULL);
	}

	for (size_t i = 0; i < threadCount; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(threadCount, handles, true, INFINITE);
	return 0;
}
