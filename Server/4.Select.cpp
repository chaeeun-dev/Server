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

// Select �� = (select �Լ��� �ٽ��� �Ǵ�)
// ���� �Լ� ȣ���� ������ ������ �̸� �� �� �ִ�!
// ���� ��Ȳ)
// ���Ź��ۿ� �����Ͱ� ���µ�, read �Ѵٰų�!
// �۽Ź��۰� �� á�µ�, write �Ѵٰų�!
// - ���ŷ ���� : ������ �������� �ʾƼ� ���ŷ�Ǵ� ��Ȳ ����
// - ����ŷ ���� : ������ �������� �ʾƼ� ���ʿ��ϰ� �ݺ� üũ�ϴ� ��Ȳ�� ����

// socket set
// 1) �б�[ ] ����[ ] ����(OOB)[ ] ���� ��� ���
// OutOfBand�� send() ������ ���� MSG_OOB�� ������ Ư���� ������
// �޴� �ʿ����� recv OOB ������ �ؾ� ���� �� ����
// 2) select(readSet, writeSet, exceptSet); -> ���� ����
// 3) ��� �ϳ��� ������ �غ�Ǹ� ���� -> �����ڴ� �˾Ƽ� ���ŵ�
// 4) ���� ���� üũ�ؼ� ����

// fd_set set;
// FD_ZERO : ����
// ex) FD_ZERO(set);
// FD_SET : ���� s�� �ִ´�
// ex) FD_SET(s, &set);
// FD_CLR : ���� s�� ����
// ex) FD_CLR(s, &set);
// FD_ISSET : ���� s�� set�� ��������� 0�� �ƴ� ���� �����Ѵ�

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

	vector<Session> sessions;
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
		// ���� �� �ʱ�ȭ
		FD_ZERO(&reads);

		// ListenSocket ���
		FD_SET(listenSocket, &reads);

		// ���� ���
		for (Session& s : sessions)
			FD_SET(s.socket, &reads);

		// [�ɼ�] ������ timeout ���� ���� ����
		int32 retVal = ::select(0, &reads, nullptr, nullptr, nullptr);
		if (retVal == SOCKET_ERROR)
			break;

		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

			if (clientSocket != INVALID_SOCKET)
			{
				// ���� ���ŷ�߾�� �ߴµ�... �ʰ� ����ŷ���� �϶��?
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;
				cout << "Client Connected" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}

		// ������ ���� üũ
		for (Session& s : sessions)
		{
			// Read
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen <= 0)
				{
					// TODO : sessions ����
					continue;
				}

				cout << "Recv Data = " << s.recvBuffer << endl;
				s.recvBytes = recvLen;
				cout << "RecvLen = " << recvLen << endl;
			}
		}
	}

	SocketUtils::Close(listenSocket);
}