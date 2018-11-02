#pragma once
#include <WinSock2.h>
#include <queue>
#include <map>
#include "..\include\Package\packetDef.h"
#include "Interfaces.h"
#include "ClientList.h"
using namespace  std;

class Server
{
public:
	Server(void);
	~Server(void);

	BOOL StartWork();
	void NotifyQuit();
	void StopWork();
	void StartService();
	void DeleteAllClients();
	CClient* AddSocket(SOCKET socket, SOCKADDR_IN address);
	void DeleteClient(SOCKET socket, CClient *pClient);
	void AddClient(SOCKET socket, CClient *pClient);
private:
	void CleanUp();
	BOOL InitSocketDLLs();
	BOOL InitServerSocket();
	BOOL InitListenSocket();

	HANDLE hSelectThread;

public:
	SOCKET socketServer;
	SOCKET socketListen;
	u_short portListen;
	BOOL bRunning;
	FD_SET setAll;
	CRITICAL_SECTION csSet;

	CClientList clients;
	CClient *pServer; 
};

