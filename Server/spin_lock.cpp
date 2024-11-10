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
		// 두 단게를 하나로 합치는 코드

		bool expected = false;
		bool desired = true;

		// SpinLock
		while (_flag.compare_exchange_strong(OUT expected, desired) == false)
		{
			expected = false;
		}

		// 의사 코드 - 이 코드가 위에서는 한 번에 일어난다.
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

		//while (_flag)		// 단계 1 - expected가 true면 아무것도 하지 않는다.
		//{
		//	// Do Nothing
		//}

		//_flag = true;		// 단계 2 - expected가 false면 desired를 true로 바꾼다.
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