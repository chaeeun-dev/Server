#include "pch.h"
#include <iostream>
using namespace std::chrono_literals;

int main()
{
	SocketUtils::Init();

	// ���ŷ ����
	// accept -> ������ Ŭ�� ���� ��
	// connect -> ���� ���� �������� ��
	// send -> ��û�� �����͸� �۽� ���ۿ� �������� ��
	// recv -> ���� ���ۿ� ������ �����Ͱ� �ְ�, �̸� �������� ���ۿ� �������� ��

	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	// ����ŷ ��������
	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); << deprecated
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777); // 80 : HTTP

	// Connect
	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			// ���� ����߾�� �ߴµ� ... �ʰ� ����ŷ �϶��?
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// �̹� ����� ���¶�� break;
			if (::WSAGetLastError() == WSAEISCONN)
				break;
		}
	}

	// Send
	while (true)
	{
		char sendBuffer[100] = "Hello I am Client!";
		int32 sendLen = sizeof(sendBuffer);

		if (::send(clientSocket, sendBuffer, sendLen, 0) == SOCKET_ERROR)
		{
			// ���� ����߾�� �ߴµ� ... �ʰ� ����ŷ �϶��?
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			std::cout << "Send Data ! Len = " << sendLen << std::endl;
		}

		std::this_thread::sleep_for(1s);	// 1�ʸ��� ������ ����
	}


	SocketUtils::Close(clientSocket);
}