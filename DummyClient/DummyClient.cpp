#include "pch.h"
#include <iostream>
using namespace std::chrono_literals;

// 클라
// 1) 소켓 생성
// 2) 서버에 연결 요청
// 3) 서버와 통신

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
	// protocol : 0
	// return : descriptor
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); << deprecated
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777); // 80 : HTTP

	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)	// 서버에 연결
		return 0;

	// ---------------------------
	// 연결 성공! 이제부터 데이터 송수신 가능!
	std::cout << "Connected To Server!" << std::endl;

	// --------------------------
	
	// 정리
	::closesocket(clientSocket);
	::WSACleanup();
}
