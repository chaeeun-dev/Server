#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <windows.h>
using namespace std::chrono_literals;

// 서버 
// 1) 새로운 소켓 생성 (socket)
// 2) 클라와 통신

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)		// winsock 시작할 준비 완료
		return 0;

	// 1) 소켓 생성
	// ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
	// protocol : 0
	// return : descriptor
	//int32 errorCode = ::WSAGetLastError();
	SOCKET listenSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	// 소켓 생성 - 핸드폰 개통
	if (listenSocket == 0)
		return 0;

	// 2) 주소/포트 번호 설정 (bind)
	// 연결할 목적지는? (IP주소 + Port) -> XX 아파트 YY 호
	SOCKADDR_IN serverAddr; // IPv4, 우리 레스토랑의 주소
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 아무 IP나
	serverAddr.sin_port = ::htons(7777); // 포트 번호, 번호 아무거나 지정하면 됨(쓰지 않을 것 같은 숫자로...) e.g. 80 : HTTP


	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)	// 원하는 IP 주소와 포트 번호로 연결 - 핸드폰에 원하는 번호로 개통
		return 0;

	// 4) 손님 맞이 (accept)
	while (true)
	{
		SOCKADDR_IN clientAddr;		// 상대의 주소
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

	// 정리하기
	::closesocket(listenSocket);
	::WSACleanup();
}
