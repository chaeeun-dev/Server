#include <windows.h>
#include <thread>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

// Code X
// Stack X
// Heap O
// Data O

class Lock
{
public:
	void lock()
	{
		// CAS (Compare-And-Swap)
		// �� �ܰԸ� �ϳ��� ��ġ�� �ڵ�

		bool expected = false;
		bool desired = true;

		// SpinLock
		while (_flag.compare_exchange_strong(OUT expected, desired) == false)
		{
			expected = false;
		}

		// �ǻ� �ڵ� - �� �ڵ尡 �������� �� ���� �Ͼ��.
		/*
		if (_flag == expected)
		{
			_flag = desired;
			return true;
		}
		else
		{
			expected = _flag;
			return false;
		}
		*/

		//while (_flag)		// �ܰ� 1 - expected�� true�� �ƹ��͵� ���� �ʴ´�.
		//{
		//	// Do Nothing
		//}

		//_flag = true;		// �ܰ� 2 - expected�� false�� desired�� true�� �ٲ۴�.
	}

	void unlock()
	{
		_flag = false;
	}

private:
	std::atomic<bool> _flag = false;
};

Lock m;
std::vector<int> v;

void Push()
{
	for (int i = 0; i < 10000; i++)
	{
		std::lock_guard <Lock> lockGuard(m);

		v.push_back(i);
	}
}

int main()
{
	v.reserve(1000000);

	std::thread t1(Push);
	std::thread t2(Push);

	t1.join();
	t2.join();

	std::cout << v.size() << std::endl;
}