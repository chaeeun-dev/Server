#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <map>
#include <queue>


class Knight
{

};

using KnightRef = std::shared_ptr<Knight>;

void Test(KnightRef knight)
{
	
}

int main() 
{
	KnightRef knight(new Knight());	// 생성 후 잊고 지내면 됨

	Test(knight);
}

//1. RefCount - 스마트 포인터를 이용해 간접적으로 몇 번 참조 되었는지 RefCount로 확인할 수 있다.
//2. RefCount 수동 관리 ? -중간에 누가 끼어들 수 있어 위험하다.
//3. RefCount를 객체에 포함시킬 것인지(직접 구현할 때) ?
//4. SharedPtr은 왜 2번 문제를 해결해주나 ?
//5. SharedPtr과 this의 문제를 어떻게 해결할 것인가 ?