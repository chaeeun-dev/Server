#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <windows.h>

int x = 0;
int y = 0;
int r1 = 0;
int r2 = 0;

volatile bool ready = false;	// volatile : �ڵ� ����ȭ�� ���� ���ƴ޶�� �뵵

void Thread_1()
{
	while (ready == false){ }	// �����Ϸ��� while (false)�� �ڵ� ����ȭ�� �� ���� �ִ�.

	std::cout << "Yeah!" << std::endl;
}

int main()
{
	std::thread t1(Thread_1);

	std::this_thread::sleep_for(std::chrono::seconds(1));  // 1�� ���

	ready = true;

	t1.join();
}