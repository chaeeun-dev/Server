#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>

int x = 0;
int y = 0;
int r1 = 0;
int r2 = 0;

bool ready = false;

void Thread_1()
{
	while (ready == false){ }

	y = 1;	// Store y
	r1 = x;	// Load x
}

void Thread_2()
{
	while (ready == false){ }

	x = 1;	// Store x
	r2 = y;	// Load y
}

int main()
{
	int count = 0;

	while (true)
	{
		ready = false;
		count++;

		x = y = r1 = r2 = 0;

		std::thread t1(Thread_1);
		std::thread t2(Thread_2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)	
			break;
	}

	std::cout << count << std::endl;
}