#pragma once
#include <windows.h>//for query performance counter

struct Timer
{
	//Credits: https://stackoverflow.com/questions/1739259/how-to-use-queryperformancecounter
	double PCFreq = 0.0;
	__int64 CounterStart = 0;

	void StartCounter()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			printf("QueryPerformanceFrequency failed!\n");

		PCFreq = double(li.QuadPart) / 1000.0;

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}
	double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - CounterStart) / PCFreq;
	}
};