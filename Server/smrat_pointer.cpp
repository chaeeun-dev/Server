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
	KnightRef knight(new Knight());	// ���� �� �ذ� ������ ��

	Test(knight);
}

//1. RefCount - ����Ʈ �����͸� �̿��� ���������� �� �� ���� �Ǿ����� RefCount�� Ȯ���� �� �ִ�.
//2. RefCount ���� ���� ? -�߰��� ���� ����� �� �־� �����ϴ�.
//3. RefCount�� ��ü�� ���Խ�ų ������(���� ������ ��) ?
//4. SharedPtr�� �� 2�� ������ �ذ����ֳ� ?
//5. SharedPtr�� this�� ������ ��� �ذ��� ���ΰ� ?