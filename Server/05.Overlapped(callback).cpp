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

// Overlapped 모델 (Completion Routine 콜백 기반)
// - 비동기 입출력 지원하는 소켓 생성
// - 비동기 입출력 함수 호출 (완료 루틴의 시작 주소를 넘겨준다)
// - 비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드
// - 비동기 입출력 함수 호출한 쓰레드를 -> Alertable Wait 상태로 만든다
// ex) WaitForSingleObjectEx, WaitForMultipleObjectsEx, SleepEx, WSAWAitForMultipleEvents
// - 비동기 IO 완료되면, 운영체제는 완료 루틴 호출
// - 완료 루틴 호출이 모두 끝나면, 쓰레드는 Alertable Wait 상태에서 빠져나온다

// 1) 오류 발생시 0 아닌 값
// 2) 전송 바이트 수
// 3) 비동기 입출력 함수 호출 시 넘겨준 WSAOVERLAPPED 구조체의 주소값
// 4) 0
//void CompletionRoutine()

// Select 모델
// - 장점) 윈도우/리눅스 공통. 
// - 단점) 성능 최하 (매번 등록 비용), 64개 제한
// WSAEventSelect 모델
// - 장점) 비교적 뛰어난 성능
// - 단점) 64개 제한
// Overlapped (이벤트 기반)
// - 장점) 성능
// - 단점) 64개 제한
// Overlapped (콜백 기반)
// - 장점) 성능
// - 단점) 모든 비동기 소켓 함수에서 사용 가능하진 않음 (accept). 빈번한 Alertable Wait으로 인한 성능 저하
// IOCP

// Reactor Pattern (~뒤늦게. 논블로킹 소켓. 소켓 상태 확인 후 -> 뒤늦게 recv send 호출)
// Proactor Pattern (~미리. Overlapped WSA~)

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
	// TODO : 에코 서버를 만든다면 WSASend()

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

			// 문제 있는 상황
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
					// TODO : 문제 있는 상황
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