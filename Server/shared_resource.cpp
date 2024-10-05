#include <vector>
#include <windows.h>
#include <thread>
#include <iostream>

// thread���� Heap, Data ������ �����ؾ� �Ѵ�.

int sum = 0;	// sum�� Data ������ ����.

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

	// i�� 100�� �� sum == 0
	// i�� 100'0000�� �� sum == 178827 (���� ���� ���� ���... ��ǻ������ ����� ���� ����̴�)
}