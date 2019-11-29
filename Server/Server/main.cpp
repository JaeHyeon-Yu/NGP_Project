#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include "globals.h"
#include "ClientClass/ball.h"
#include "ClientClass/tower.h"
using namespace std;
struct ThreadArg {
	SOCKET sock;
	int clientIdx;
};
struct StagingPacket {
	Tower_Packet tPacket;
	Mouse_Packet mPacket;
};
DWORD WINAPI RecvThread(LPVOID arg);
DWORD WINAPI SendThread(LPVOID arg);

void Initialize(int idx);
void err_quit(char* msg) {
	// 소켓함수 오류 출력 후 종료
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(char* msg) {
	// 소켓함수 오류 출력
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	cout << msg << (char*)lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

// global val--------
int g_users{ 0 };
int g_gameState{ 0 };
Tower g_towerArr[2];
SOCKET g_sockets[2]{ NULL, NULL };
StagingPacket g_sPack[2]{ NULL, NULL };
HANDLE g_threadHandle[MAX_USERS];
//---------------------

int main() {
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// 서버의 IP를 콘솔창에 출력한다.
	HOSTENT* hostent;
	char name[20], addr[20];
	retval = gethostname(name, sizeof(name));
	if (retval == 0) {
		hostent = gethostbyname(name);
		strcpy(addr, inet_ntoa(*(IN_ADDR*)
			hostent->h_addr_list[0]));
	}
	cout << "Server's IP : " << addr << endl;

	// socket 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("WSASocket()");

	// bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)& serveraddr, sizeof(serveraddr));

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hthread[MAX_USERS];
	HANDLE sendThread;
	g_threadHandle[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_threadHandle[1] = CreateEvent(NULL, TRUE, FALSE, NULL);

	sendThread = CreateThread(NULL, 0, SendThread, NULL, 0, NULL);
	while (true) {
		if (g_users >= MAX_USERS) Sleep(999);

		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		printf("\n[TCP Server] Client Access : IP:%s,	Port:%d \n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Client index 배정
		int clientIdx{ -1 };
		for (int i = 0; i < MAX_USERS; ++i)
			if (g_sockets[i] == NULL) {
				clientIdx = i;
				g_sockets[i] = client_sock;
				break;
			}
		g_users++;

		// Initialize
		Initialize(clientIdx);

		// create thread
		ThreadArg* targ = new ThreadArg;
		targ->sock = client_sock;
		targ->clientIdx = clientIdx;
		hthread[clientIdx] = CreateThread(NULL, 0, RecvThread, (LPVOID)targ, 0, NULL);
		if (hthread == NULL) closesocket(client_sock);
		else CloseHandle(hthread);
	}

	closesocket(listen_sock);
	WSACleanup();
	return 0;
}

DWORD WINAPI RecvThread(LPVOID arg) {
	// Communication Val
	ThreadArg *tArg = reinterpret_cast<ThreadArg*>(arg);
	SOCKET sock = tArg->sock;
	int clientIdx = tArg->clientIdx;
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	int retval;
	int recvBytes{};
	char buf[BUFSIZE];

	getpeername(sock, (SOCKADDR*)& clientaddr, &addrlen);

	// 초기 데이터 전송
	//retval = send(sock, (char*)& g_towerArr, sizeof(g_towerArr), 0);
	//if (retval == SOCKET_ERROR) err_display("send()");
	retval = send(sock, (char*)& clientIdx, sizeof(int), 0);
	if (retval == SOCKET_ERROR) err_display("send()");

	while (true) {
		// Recv & Casting Tower Packet
		ZeroMemory(buf, BUFSIZE);
		retval = recv(sock, (char*)& recvBytes, sizeof(int), 0);
		if (retval == SOCKET_ERROR) err_display("recv()");
		retval = recv(sock, buf, recvBytes, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		buf[recvBytes] = '\0';
		Tower_Packet* tPacket = (Tower_Packet*)buf;

		// // Recv & Casting Mouse Packet
		// ZeroMemory(buf, BUFSIZE);
		// retval = recv(sock, (char*)& recvBytes, sizeof(int), 0);
		// if (retval == SOCKET_ERROR) err_display("recv()");
		// retval = recv(sock, buf, recvBytes, 0);
		// if (retval == SOCKET_ERROR) {
		// 	err_display("recv()");
		// 	break;
		// }
		// buf[recvBytes] = '\0';
		// Mouse_Packet* mPacket = (Mouse_Packet*)buf;

		// SendThread로 넘기기 전 대기 영역에 패킷을 저장해준다. 
		g_sPack[clientIdx].tPacket = *tPacket;
	//	g_sPack[clientIdx].mPacket = *mPacket;
		// Event 활성화
		SetEvent(g_threadHandle[clientIdx]);
	}
	return 0;
}
DWORD WINAPI SendThread(LPVOID arg) {

	int retval;
	int sendLen;
	while (true) {
		// Event 활성화까지 대기하기
		WaitForMultipleObjects(2, g_threadHandle, TRUE, INFINITE);

		for (int i = 0; i < MAX_USERS; ++i)
			// 패킷을 인자로 넣어서 Update
			g_towerArr[i].Update(g_sPack[i].tPacket);

		


		Tower_Packet tPacket[MAX_USERS];
		tPacket[0] = g_towerArr[0].MakePacket();
		tPacket[1] = g_towerArr[1].MakePacket();
	
		for (int i = 0; i < MAX_USERS; ++i) {
			// send Tower Packet
			sendLen = sizeof(tPacket);
			retval = send(g_sockets[i], (char*)& sendLen, sizeof(int), 0);
			if (retval == SOCKET_ERROR) err_display("send()");
			retval = send(g_sockets[i], (char*)& tPacket, sendLen, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}

		// Event 비활성화
		ResetEvent(g_threadHandle[0]);
		ResetEvent(g_threadHandle[1]);
	}
	return 0;
}
void Initialize(int idx) {
	if (idx < 0 || idx >= MAX_USERS) return; 
	int tower_level = EASY_1;

	g_towerArr[idx].Initialize(tower_level, idx);
}
