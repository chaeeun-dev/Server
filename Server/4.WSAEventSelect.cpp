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

// WSAEventSelect = (WSAEventSelect �Լ��� �ٽ��� �Ǵ�)
// ���ϰ� ���õ� ��Ʈ��ũ �̺�Ʈ�� [�̺�Ʈ ��ü]�� ���� ����

// �̺�Ʈ ��ü ���� �Լ���
// ���� : WSACreateEvent (���� ���� Manual-Reset + Non-Signaled ���� ����)
// ���� : WSACloseEvent
// ��ȣ ���� ���� : WSAWaitForMultipleEvents
// ��ü���� ��Ʈ��ũ �̺�Ʈ �˾Ƴ��� : WSAEnumNetworkEvents

// ���� <-> �̺�Ʈ ��ü ����
// WSAEventSelect(socket, event, networkEvents);
// - �����ִ� ��Ʈ��ũ �̺�Ʈ
// FD_ACCEPT : ������ Ŭ�� ���� accept
// FD_READ : ������ ���� ���� recv, recvfrom
// FD_WRITE : ������ �۽� ���� send, sendto
// FD_CLOSE : ��밡 ���� ����
// FD_CONNECT : ����� ���� ���� ���� �Ϸ�
// FD_OOB

// ���� ����
// WSAEventSelect �Լ��� ȣ���ϸ�, �ش� ������ �ڵ����� �ͺ��ŷ ��� ��ȯ
// accept() �Լ��� �����ϴ� ������ listenSocket�� ������ �Ӽ��� ���´�
// - ���� clientSocket�� FD_READ, FD_WRITE ���� �ٽ� ��� �ʿ�
// - �幰�� WSAEWOULDBLOCK ������ �� �� ������ ���� ó�� �ʿ�
// �߿�)
// - �̺�Ʈ �߻� ��, ������ ���� �Լ� ȣ���ؾ� ��
// - �ƴϸ� ���� ������ ���� ��Ʈ��ũ �̺�Ʈ�� �߻� X
// ex) FD_READ �̺�Ʈ ������ recv() ȣ���ؾ� �ϰ�, ���ϸ� FD_READ �� �� �ٽ� X

// 1) count, event
// 2) waitAll : ��� ��ٸ�? �ϳ��� �Ϸ� �Ǿ OK?
// 3) timeout : Ÿ�Ӿƿ�
// 4) ������ false
// return : �Ϸ�� ù��° �ε���
// WSAWaitForMultipleEvents

// 1) socket
// 2) eventObject : socket �� ������ �̺�Ʈ ��ü �ڵ��� �Ѱ��ָ�, �̺�Ʈ ��ü�� non-signaled
// 3) networkEvent : ��Ʈ��ũ �̺�Ʈ / ���� ������ ����
// WSAEnumNetworkEvents

const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
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

	vector<WSAEVENT> wsaEvents;
	vector<Session> sessions;
	sessions.reserve(100);

	WSAEVENT listenEvent = ::WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sessions.push_back(Session{ listenSocket });
	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
		return 0;

	while (true)
	{
		int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED)
			continue;

		index -= WSA_WAIT_EVENT_0;

		//::WSAResetEvent(wsaEvents[index]);

		WSANETWORKEVENTS networkEvents;
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
			continue;

		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			// Error-Check
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);

			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected" << endl;

				WSAEVENT clientEvent = ::WSACreateEvent();
				wsaEvents.push_back(clientEvent);
				sessions.push_back(Session{ clientSocket });
				if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
					return 0;
			}
		}

		// Client Session ���� üũ
		if (networkEvents.lNetworkEvents & FD_READ)
		{
			// Error-Check
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
				continue;

			Session& s = sessions[index];

			// Read
			int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
			if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
			{
				if (recvLen <= 0)
				{
					// TODO : sessions ����
					continue;
				}
			}

			s.recvBytes = recvLen;
			cout << "Recv Data = " << s.recvBuffer << endl;
			cout << "RecvLen = " << recvLen << endl;
		}
	}

	SocketUtils::Close(listenSocket);
}