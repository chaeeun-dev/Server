#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
using namespace std;
#include <atomic>
#include <mutex>
#include <windows.h>
#include "TestMain.h"
#include "ThreadManager.h"

const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	WSAOVERLAPPED overlapped = {};
};

int main()
{
	SocketUtils::Init();

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SocketUtils::SetReuseAddress(listenSocket, true);
	
	if (SocketUtils::BindAnyAddress(listenSocket, 7777) == false)
		return 0;

	SocketUtils::Listen(listenSocket);

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);

	// Overlapped IO (�񵿱� + ����ŷ)
	// - Overlapped �Լ��� �Ǵ� (WSARecv, WSASend)
	// - Overlapped �Լ��� �����ߴ��� Ȯ�� ��
	// -> ���������� ��� �� ó��
	// -> ���������� ������ Ȯ��

	// 1) �񵿱� ����� ����
	// 2) WSABUF �迭�� ���� �ּ� + ���� // Scatter-Gather
	// 3) ������/���� ����Ʈ ��
	// 4) �� �ɼ��ε� 0
	// 5) WSAOVERLAPPED ����ü �ּҰ�
	// 6) ������� �Ϸ�Ǹ� OS�� ȣ���� �ݹ� �Լ�
	// WSASend
	// WSARecv

	// Overlapped �� (�̺�Ʈ ���)
	// - �񵿱� ����� �����ϴ� ���� ���� + ���� �ޱ� ���� �̺�Ʈ ��ü ����
	// - �񵿱� ����� �Լ� ȣ�� (1���� ���� �̺�Ʈ ��ü�� ���� �Ѱ���)
	// - �񵿱� �۾��� �ٷ� �Ϸ���� ������, WSA_IO_PENDING ���� �ڵ�
	// �ü���� �̺�Ʈ ��ü�� signaled ���·� ���� �Ϸ� ���� �˷���
	// - WSAWaitForMultipleEvents �Լ� ȣ���ؼ� �̺�Ʈ ��ü�� signal �Ǻ�
	// - WSAGetOverlappedResult ȣ���ؼ� �񵿱� ����� ��� Ȯ�� �� ������ ó��

	// 1) �񵿱� ����
	// 2) �Ѱ��� overlapped ����ü
	// 3) ���۵� ����Ʈ ��
	// 4) �񵿱� ����� �۾��� ���������� �������?
	// false
	// 5) �񵿱� ����� �۾� ���� �ΰ� ����. ���� ��� �� ��.
	// WSAGetOverlappedResult

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// ���� �ִ� ��Ȳ
			return 0;
		}

		Session session = Session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << "Client Connected !" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, nullptr) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Pending
					::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
					::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
				}
				else
				{
					// TODO : ���� �ִ� ��Ȳ
					break;
				}
			}

			cout << "Data Recv = " << session.recvBuffer << endl;
			cout << "Data Recv Len = " << recvLen << endl;
		}

		::closesocket(session.socket);
		::WSACloseEvent(wsaEvent);
	}

	SocketUtils::Close(listenSocket);
}