#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <windows.h>

int x = 0;
int y = 0;
int r1 = 0;
int r2 = 0;

volatile bool ready = false;	// volatile : 코드 최적화를 하지 말아달라는 용도

void Thread_1()
{
	while (ready == false){ }	// 컴파일러가 while (false)로 코드 최적화를 할 수도 있다.

	std::cout << "Yeah!" << std::endl;
}

int main()
{
	std::thread t1(Thread_1);

	std::this_thread::sleep_for(std::chrono::seconds(1));  // 1초 대기

	ready = true;

	t1.join();
}