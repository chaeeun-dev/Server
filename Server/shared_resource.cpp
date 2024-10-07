#include <windows.h>
#include <thread>
#include <iostream>
#include <atomic>

// Code X
// Stack X
// Heap O
// Data O

// thread���� Heap, Data ������ �����ؾ� �Ѵ�.
// ���ڸ��� �����͸� �����ؼ� ����� �� �ִ� �ڵ����� Ȯ���ϰ� �Ǻ��� �� �־�� �Ѵ�! 1�ʸ���.

std::atomic<int> sum = 0;

void Test()
{
	for (int i = 0; i < 10000; i++)
	{
		int* p = new int();		// Heap������ �����ؼ� ������� �����Ƿ� ��Ƽ ������ ��Ȳ������ ũ���ð� ���� �ʴ´�.

		*p = 100;

		delete p;
	}
}

void Add()
{
	for (int i = 0; i < 100'0000; i++)
	{
		// All or Nothing (��� �ǰų� �ƿ� �� �ǰų�)
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
	int temp = sum.load();	// ���ٰ� �Ȱ��� ��, ���� atomic Ÿ���̶�� ��Ʈ�� �� ���̴�.
	
	// sum = 10;
	sum.store(10);	// ���ٰ� �Ȱ��� ��, ���������� atomic�̶�� ��Ʈ�� �ش�.

	// ���� ���� �� �� ������ ���� �ñ��ϸ�
	int temp = sum.exchange(10);	// sum�� 10�� �־��ְ�, temp�� ���� ���� �־��ش�.

	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	std::cout << "sum == " << sum << std::endl;
}