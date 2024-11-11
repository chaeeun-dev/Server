#include <iostream>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <map>

// MMO 서버에서 crash 나는 원인
// 95% NULL
// 1% DeadLock
// 1% Use After Free
// 1% Memory Leak

class User
{

};

class UserManager
{
public:
	User* GetUser(int id)
	{
		std::unique_lock<std::mutex> guard(_lock);

		if (_users.find(id) == _users.end())
			return nullptr;

		return _users[id];
	}

private:
	std::map<int, User*> _users;
	std::mutex _lock;
};

std::mutex m1;	// Account Manager
std::mutex m2;	// User Manager

void Thread_1()
{
	for (int i = 0; i < 10000; i++)
	{
		std::lock_guard<std::mutex> lockGuard1(m1);
		std::lock_guard<std::mutex> lockGuard2(m2);
		// TODO
	}
}

void Thread_2()
{
	for (int i = 0; i < 10000; i++)
	{
		std::lock_guard<std::mutex> lockGuard2(m1);
		std::lock_guard<std::mutex> lockGuard1(m2);
	}
}

int main()
{
	std::thread t1(Thread_1);
	std::thread t2(Thread_2);

	t1.join();
	t2.join();

	std::cout << "Jobs Done" << std::endl;
}