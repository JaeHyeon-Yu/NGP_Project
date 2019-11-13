#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include "globals.h"
#include "ClientClass/ball.h"
#include "ClientClass/tower.h"
using namespace std;

DWORD WINAPI ProcessClient(LPVOID arg);
DWORD WINAPI RecvThread(LPVOID arg);

void Initialize(int idx);
void err_quit(char* msg) {
	// �����Լ� ���� ��� �� ����
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
	// �����Լ� ���� ���
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
Ball g_ballArr[2];
Tower g_towerArr[2];
SOCKET g_sockets[2]{ NULL, NULL };
//---------------------

int main() {
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// ������ IP�� �ܼ�â�� ����Ѵ�.
	HOSTENT* hostent;
	char name[20], addr[20];
	retval = gethostname(name, sizeof(name));
	if (retval == 0) {
		hostent = gethostbyname(name);
		strcpy(addr, inet_ntoa(*(IN_ADDR*)
			hostent->h_addr_list[0]));
	}
	cout << "Server's IP : " << addr << endl;

	// socket ����
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
	HANDLE hthread;

	while (true) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		printf("\n[TCP Server] Client Access : IP:%s,	Port:%d \n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// create thread
		hthread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hthread == NULL) closesocket(client_sock);
		else CloseHandle(hthread);

	}

	closesocket(listen_sock);
	WSACleanup();
	return 0;
}
DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET sock = (SOCKET)arg; 
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	int retval;
	HANDLE hthread;

	getpeername(sock, (SOCKADDR*)& clientaddr, &addrlen);

	//while (true) {
		// Client index ����
		int clientIdx{ -1 };
		for (int i = 0; i < MAX_USERS; ++i)
			if (g_sockets[i] == NULL) {
				cout << i << endl;
				clientIdx = i;
				g_sockets[i] = sock;
				break;
			}

		// Initialize
		Initialize(clientIdx);

		// 2�� Ȯ�� ---- Event ���
		++g_users;
		
		// ����ȭ ���������� ���� ����ڵ� �ݵ�� ��������
		while (g_users < MAX_USERS) {
			cout << "����" << clientIdx << endl;
		}
		cout << "����!" << clientIdx << endl;
	
		// 2�� ���� �� �ʱ� ���� ����
		if (g_users >= MAX_USERS) {
			retval = send(sock, (char*)& g_ballArr, sizeof(g_ballArr), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			//	break;
			}
			retval = send(sock, (char*)& g_ballArr, sizeof(g_ballArr), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			//	break;
			}
		}

		// ���� �����ϰ� RecvThread ����
		// create thread
		hthread = CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);
		if (hthread == NULL) closesocket(sock);
		else CloseHandle(hthread);
	//}
	return 0;
}
DWORD WINAPI RecvThread(LPVOID arg) {
	SOCKET sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	int retval;

	getpeername(sock, (SOCKADDR*)& clientaddr, &addrlen);

	cout << "asdfggasd" << endl;
	return 0;
}
void Initialize(int idx) {
	if (idx < 0 || idx >= MAX_USERS) return; 
	int tower_level = 1;

	g_ballArr[idx].Initialize(idx);
	g_towerArr[idx].Initialize(tower_level);
}
