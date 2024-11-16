#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <windows.h>
using namespace std::chrono_literals;

// ���� 
// 1) ���ο� ���� ���� (socket)
// 2) Ŭ��� ���

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)		// winsock ������ �غ� �Ϸ�
		return 0;

	// 1) ���� ����
	// ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
	// protocol : 0
	// return : descriptor
	//int32 errorCode = ::WSAGetLastError();
	SOCKET listenSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	// ���� ���� - �ڵ��� ����
	if (listenSocket == 0)
		return 0;

	// 2) �ּ�/��Ʈ ��ȣ ���� (bind)
	// ������ ��������? (IP�ּ� + Port) -> XX ����Ʈ YY ȣ
	SOCKADDR_IN serverAddr; // IPv4, �츮 ��������� �ּ�
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // �ƹ� IP��
	serverAddr.sin_port = ::htons(7777); // ��Ʈ ��ȣ, ��ȣ �ƹ��ų� �����ϸ� ��(���� ���� �� ���� ���ڷ�...) e.g. 80 : HTTP


	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)	// ���ϴ� IP �ּҿ� ��Ʈ ��ȣ�� ���� - �ڵ����� ���ϴ� ��ȣ�� ����
		return 0;

	// 4) �մ� ���� (accept)
	while (true)
	{
		SOCKADDR_IN clientAddr;		// ����� �ּ�
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);

		// 5) TODO
		char recvBuffer[100];

		int32 recvLen = ::recvfrom(listenSocket, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&clientAddr, &addrLen);
		if (recvLen <= 0)
			break;

		std::cout << "Recv Data : " << recvBuffer << std::endl;
		std::cout << "Recv Data Len : " << recvLen << std::endl;

		std::this_thread::sleep_for(1s);
	
	}

	// �����ϱ�
	::closesocket(listenSocket);
	::WSACleanup();
}
