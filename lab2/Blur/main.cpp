#pragma once
#define NOMINMAX
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>
#include <memory>
#include "bitmap_image.hpp"
#include "BpmBlurCollumnsCommand.h"


using namespace std;

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	unique_ptr<BpmBlurCollumnsCommand>* commandPtr = (unique_ptr<BpmBlurCollumnsCommand>*)lpParam;
	(*commandPtr)->Execute();

	ExitThread(0);
}

vector<unique_ptr<BpmBlurCollumnsCommand>> GetCommands(const bitmap_image& fromImage, bitmap_image& toImage, uint32_t threadCount)
{
	vector<unique_ptr<BpmBlurCollumnsCommand>> commandPtrArr;
	uint32_t collumnsCount = fromImage.width();
	if (collumnsCount < threadCount)
	{
		for (uint32_t i = 0; i < collumnsCount; ++i)
		{
			commandPtrArr.push_back(make_unique<BpmBlurCollumnsCommand>(fromImage, toImage, i, i + 1));
		}
	}
	else
	{
		uint32_t threadCollumnSize = collumnsCount / threadCount;

		for (uint32_t i = 0; i < threadCount - 1; ++i)
		{
			commandPtrArr.push_back(make_unique<BpmBlurCollumnsCommand>(fromImage, toImage, i * threadCollumnSize, (i + 1) * threadCollumnSize));
		}
		commandPtrArr.push_back(make_unique<BpmBlurCollumnsCommand>(fromImage, toImage, (threadCount - 1) * threadCollumnSize, collumnsCount));
	}

	return commandPtrArr;
}

void BlurImageAsync(string input, string output, uint32_t coreCount, uint32_t threadCount)
{
	bitmap_image image(input);
	if (!image)
	{
		cerr << "Error - Failed to open: " << input << endl;
		return;
	}
	bitmap_image resultImage(image.width(), image.height());

	vector<unique_ptr<BpmBlurCollumnsCommand>> commandPtrArr = GetCommands(image, resultImage, threadCount);

	vector<HANDLE> handles(commandPtrArr.size());
	uint64_t affinityMask = (1ull << (coreCount + 1ull)) - 1ull;
	for (size_t i = 0; i < commandPtrArr.size(); ++i)
	{
		handles[i] = CreateThread(NULL, 0, &ThreadProc, &commandPtrArr[i], CREATE_SUSPENDED, NULL);
		SetThreadAffinityMask(handles[i], affinityMask);
	}
	for (const auto& handle : handles)
	{
		ResumeThread(handle);
	}
	WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), true, INFINITE);

	resultImage.save_image(output);
}

void BlurImageSync(string input, string output)
{
	bitmap_image image(input);
	if (!image)
	{
		cerr << "Error - Failed to open: " << input << endl;
		return;
	}
	bitmap_image resultImage(image.width(), image.height());
	BpmBlurCollumnsCommand cmd(image, resultImage, 0, image.width());
	cmd.Execute();
	resultImage.save_image(output);
}

void BlurImage(string input, string output, uint32_t coreCount, uint32_t threadCount)
{
	if (threadCount == 1)
	{
		BlurImageSync(input, output);
	}
	else
	{
		BlurImageAsync(input, output, coreCount, threadCount);
	}
}

int main(int argc, char* argv[])
{
	std::clock_t c_start = std::clock();
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	string inputImage;
	string outputImage;
	uint32_t coreCount;
	uint32_t threadCount;

	try
	{
		inputImage = argv[1];
		outputImage = argv[2];
		coreCount = std::stoul(argv[3]);
		threadCount = std::stoul(argv[4]);
	}
	catch (const std::exception&)
	{
		cerr << "{input} {output} {coreCount} {threadCount}" << endl;
		return 1;
	}
	//inputImage = "i1.bmp";
	//outputImage = "i1out.bmp";
	//coreCount = 7ull;
	//threadCount = 8ull;

	BlurImageAsync(inputImage, outputImage, coreCount, threadCount);
	
	std::clock_t c_end = std::clock();
	cout << "Run time: " << c_end - c_start << " ms" << endl;
	return 0;
}
