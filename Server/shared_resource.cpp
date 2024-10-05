#include <vector>
#include <windows.h>
#include <thread>
#include <iostream>

// thread에서 Heap, Data 영역을 조심해야 한다.

int sum = 0;	// sum은 Data 영역에 들어간다.

void Add()
{
	for (int i = 0; i < 100'0000; i++)
	{
		sum++;
	}
}

void Sub()
{
	for (int i = 0; i < 100'0000; i++)
	{
		sum--;
	}
}

int main()
{
	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	std::cout << "sum == " << sum << std::endl;

	// i가 100일 때 sum == 0
	// i가 100'0000일 때 sum == 178827 (정말 어이 없는 결과... 컴퓨터한테 뒷통수 맞은 기분이다)
}