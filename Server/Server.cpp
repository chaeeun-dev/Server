#include <iostream>
using namespace std;
#include <thread>
#include <vector>

// 메모리
// Code - 코드 영역은 수정하지 않기 때문에 문제 없음
// Stack - 쓰레드마다 고유한 스택 영역 사용하기에 문제 없음
//
// Heap O
// Data O
// 이 두 영역은 공유해서 사용할 수 있기 때문에 복잡함...

void HelloThread(int i)
{
	while (true)
	{
		cout << "Hello Thread" << i << endl;
	}
}

int main()
{
	vector<thread> threads;		// 여러 개의 쓰레드를 만들 때 vector 사용해도 됨

	for (int i = 0; i < 12; ++i)
	{
		threads.push_back(thread(HelloThread, i));
	}

	// 자식 쓰레드가 메인 쓰레드가 종료되어도 끝나지 않으면 오류 발생 -> join() 
	for (thread& t :threads)
	{
		int a = t.hardware_concurrency();	// CPU 코어 개수 알 수 있음 - 12개, 12개 이상으로 늘려도 빨라지지 않음
		t.join();
	}
}
