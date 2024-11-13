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

// CV는 커널 오브젝트가 아니라 유저레벨 오브젝트
std::condition_variable cv;

void Producer()
{
	while (true)
	{
		// Condition Variable
		// 1) Lock을 잡기
		// 2) 공유 변수 값 수정
		// 3) Lock을 풀고
		// 4) CV 통해서 통지
		
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
		// 1) Lock을 잡으려고 시도 (이미 잡혔으면 skip)
		// 2) 조건 확인
		// - 만족O => 바로 빠져 나와서 이어서 코드 진행
		// - 만족X => Lock을 풀어주고 대기 상태로 전환

		{
			int data = q.front();
			q.pop();
			std::cout << data << std::endl;
		}
	}
}

int main()
{
	// 커널 오브젝트
	// - Usage Count
	//   Signal (파란 불) / Non Signal (빨간 불)

	// Auto / Manual 둘 중 선택할 수 있음 -  bool
	hEvent = ::CreateEvent(NULL/*보안 속성*/, FALSE/*bManualReset*/, FALSE/*초기상태*/, NULL);

	std::thread t1(Producer);
	std::thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(hEvent);
}