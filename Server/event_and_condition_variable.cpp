#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <map>
#include <queue>
using namespace std::chrono_literals;

std::mutex m;
std::queue<int> q;
HANDLE hEvent;

// CV�� Ŀ�� ������Ʈ�� �ƴ϶� �������� ������Ʈ
std::condition_variable cv;

void Producer()
{
	while (true)
	{
		// Condition Variable
		// 1) Lock�� ���
		// 2) ���� ���� �� ����
		// 3) Lock�� Ǯ��
		// 4) CV ���ؼ� ����
		
		{
			std::unique_lock<std::mutex> lock(m);
			q.push(100);
		}

		cv.notify_one();
		std::this_thread::sleep_for(100ms);
	}
}

void Consumer()
{
	while (true)
	{
		//::WaitForSingleObject(hEvent, INFINITE);
		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock, []() {return q.empty() == false; });
		// 1) Lock�� �������� �õ� (�̹� �������� skip)
		// 2) ���� Ȯ��
		// - ����O => �ٷ� ���� ���ͼ� �̾ �ڵ� ����
		// - ����X => Lock�� Ǯ���ְ� ��� ���·� ��ȯ

		{
			int data = q.front();
			q.pop();
			std::cout << data << std::endl;
		}
	}
}

int main()
{
	// Ŀ�� ������Ʈ
	// - Usage Count
	//   Signal (�Ķ� ��) / Non Signal (���� ��)

	// Auto / Manual �� �� ������ �� ���� -  bool
	hEvent = ::CreateEvent(NULL/*���� �Ӽ�*/, FALSE/*bManualReset*/, FALSE/*�ʱ����*/, NULL);

	std::thread t1(Producer);
	std::thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(hEvent);
}