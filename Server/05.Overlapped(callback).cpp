#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
//using namespace std;
#include <atomic>
#include <mutex>
#include <windows.h>
#include "TestMain.h"
#include "ThreadManager.h"

// Overlapped �� (Completion Routine �ݹ� ���)
// - �񵿱� ����� �����ϴ� ���� ����
// - �񵿱� ����� �Լ� ȣ�� (�Ϸ� ��ƾ�� ���� �ּҸ� �Ѱ��ش�)
// - �񵿱� �۾��� �ٷ� �Ϸ���� ������, WSA_IO_PENDING ���� �ڵ�
// - �񵿱� ����� �Լ� ȣ���� �����带 -> Alertable Wait ���·� �����
// ex) WaitForSingleObjectEx, WaitForMultipleObjectsEx, SleepEx, WSAWAitForMultipleEvents
// - �񵿱� IO �Ϸ�Ǹ�, �ü���� �Ϸ� ��ƾ ȣ��
// - �Ϸ� ��ƾ ȣ���� ��� ������, ������� Alertable Wait ���¿��� �������´�

// 1) ���� �߻��� 0 �ƴ� ��
// 2) ���� ����Ʈ ��
// 3) �񵿱� ����� �Լ� ȣ�� �� �Ѱ��� WSAOVERLAPPED ����ü�� �ּҰ�
// 4) 0
//void CompletionRoutine()

// Select ��
// - ����) ������/������ ����. 
// - ����) ���� ���� (�Ź� ��� ���), 64�� ����
// WSAEventSelect ��
// - ����) ���� �پ ����
// - ����) 64�� ����
// Overlapped (�̺�Ʈ ���)
// - ����) ����
// - ����) 64�� ����
// Overlapped (�ݹ� ���)
// - ����) ����
// - ����) ��� �񵿱� ���� �Լ����� ��� �������� ���� (accept). ����� Alertable Wait���� ���� ���� ����
// IOCP

// Reactor Pattern (~�ڴʰ�. ����ŷ ����. ���� ���� Ȯ�� �� -> �ڴʰ� recv send ȣ��)
// Proactor Pattern (~�̸�. Overlapped WSA~)

const int32 BUFSIZE = 1000;

struct Session
{
	WSAOVERLAPPED overlapped = {};
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	cout << "Data Recv Len Callback = " << recvLen << endl;
	// TODO : ���� ������ ����ٸ� WSASend()

	Session* session = (Session*)overlapped;
	cout << "Data Recv = " << session->recvBuffer << endl;
}

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
		//WSAEVENT wsaEvent = ::WSACreateEvent();

		cout << "Client Connected !" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Pending
					// Alertable Wait					
					::SleepEx(INFINITE, TRUE);
					//::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE);					
				}
				else
				{
					// TODO : ���� �ִ� ��Ȳ
					break;
				}
			}
			else
			{
				cout << "Data Recv Len = " << recvLen << endl;
			}
		}

		::closesocket(session.socket);
		//::WSACloseEvent(wsaEvent);
	}

	SocketUtils::Close(listenSocket);
}