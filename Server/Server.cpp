#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <windows.h>

// 서버 
// 1) 새로운 소켓 생성 (socket)
// 2) 소켓에 주소/포트 번호 지정 (bind)
// 3) 소켓 일 시키기 (listen)
// 4) 손님 접속 (accept)
// 5) 클라와 통신

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
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);	// 소켓 생성 - 핸드폰 개통
	if (listenSocket == 0)
		return 0;

	// 2) 주소/포트 번호 설정 (bind)
	// 연결할 목적지는? (IP주소 + Port) -> XX 아파트 YY 호
	SOCKADDR_IN serverAddr; // IPv4, 우리 레스토랑의 주소
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 아무 IP나
	serverAddr.sin_port = ::htons(7777); // 포트 번호, 번호 아무거나 지정하면 됨(쓰지 않을 것 같은 숫자로...) e.g. 80 : HTTP

	// host to network short
	// Little-Endian vs Big-Endian
	// ex) 0x12345678 4바이트 정수
	// low [0x78][0x56][0x34][0x12] high < little
	// low [0x12][0x34][0x56][0x78] high < big = network

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)	// 원하는 IP 주소와 포트 번호로 연결 - 핸드폰에 원하는 번호로 개통
		return 0;

	// 3) 업무 개시 (listen)
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)	// 업무 개시 - 핸드폰 전화 받을 단계
		return 0;

	// 4) 손님 맞이 (accept)
	while (true)
	{
		SOCKADDR_IN clientAddr;		// 상대의 주소
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);		// 손님이 입장하기 전까지 멈춰있음.	
		if (clientSocket == INVALID_SOCKET)
			return 0;

		// 손님 입장!
		char ip[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ip, sizeof(ip));		// IP 추출
		std::cout << "Client Connected! IP = " << ip << std::endl;

		// 5) TODO
		/*while (true)
		{
			char recvBuffer[100];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen <= 0)
				return 0;

			std::cout << "Recv Data : " << recvBuffer << std::endl;
			std::cout << "Recv Data Len : " << recvLen << std::endl;

			int32 resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
			if (resultCode == SOCKET_ERROR)
				return 0;
		}*/
	}

	// 정리하기
	::closesocket(listenSocket);
	::WSACleanup();
}
