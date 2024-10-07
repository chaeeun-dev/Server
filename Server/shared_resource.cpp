#include <windows.h>
#include <thread>
#include <iostream>
#include <atomic>

// Code X
// Stack X
// Heap O
// Data O

// thread에서 Heap, Data 영역을 조심해야 한다.
// 보자마자 데이터를 공유해서 사용할 수 있는 코드인지 확실하게 판별할 수 있어야 한다! 1초만에.

std::atomic<int> sum = 0;

void Test()
{
	for (int i = 0; i < 10000; i++)
	{
		int* p = new int();		// Heap이지만 공유해서 사용하지 않으므로 멀티 스레드 상황에서도 크래시가 나지 않는다.

		*p = 100;

		delete p;
	}
}

void Add()
{
	for (int i = 0; i < 100'0000; i++)
	{
		// All or Nothing (모두 되거나 아예 안 되거나)
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
	// int temp = sum;
	int temp = sum.load();	// 윗줄과 똑같은 뜻, 단지 atomic 타입이라는 힌트를 줄 뿐이다.
	
	// sum = 10;
	sum.store(10);	// 윗줄과 똑같은 뜻, 마찬가지로 atomic이라는 힌트를 준다.

	// 값을 넣을 때 그 이전의 값이 궁금하면
	int temp = sum.exchange(10);	// sum에 10을 넣어주고, temp에 이전 값을 넣어준다.

	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	std::cout << "sum == " << sum << std::endl;
}