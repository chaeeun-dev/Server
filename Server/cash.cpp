#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>

int buffer[10000][10000];

int main()
{
	std::memset(buffer, 0, sizeof(buffer));		// 0으로 밀어 넣음

	{
		auto start = GetTickCount64();

		__int64 sum = 0;

		for (int i = 0; i < 10000; i++)
			for (int j = 0; j < 10000; j++)
				sum += buffer[i][j];

		auto end = GetTickCount64();

		std::cout << "Elapsed Tick" << (end - start) << std::endl;

	}

	{
		auto start = GetTickCount64();

		__int64 sum = 0;

		for (int i = 0; i < 10000; i++)
			for (int j = 0; j < 10000; j++)
				sum += buffer[j][i];	// j, i 순서만 바꿈

		auto end = GetTickCount64();

		std::cout << "Elapsed Tick" << (end - start) << std::endl;

	}

	// 결과 
	// Elapsed Tick94
	// Elapsed Tick375
}