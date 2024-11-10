#include <windows.h>
#include <thread>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>	// lock�� mutex�� ����ϸ� ��

// Code X
// Stack X
// Heap O
// Data O

// Mutal Exclusive (��ȣ��Ÿ�� : �� ���� �ϳ��� ����)
std::mutex m;	

std::vector<int> v;

// RAII (Resource Acquisition Is Initialization)

template<typename T>
class LockGuard
{
public:
	LockGuard(T& m) : _mutex(m)
	{
		_mutex.lock();
	}

	~LockGuard()
	{
		_mutex.unlock();
	}

private:
	T& _mutex;
};

void Push()
{
	for (int i = 0; i < 10000; i++)
	{
		LockGuard<std::mutex> lockGuard(m);
		//std::lock_guard<std::mutex> lockGuard(m);

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