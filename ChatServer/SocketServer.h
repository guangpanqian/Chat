#pragma once
#include "ClientList.h"
#include <map>
using namespace  std;
class SocketClient;

class SocketServer
{
public:
	SocketServer(void);
	~SocketServer(void);
public:
	void StartWork();
	void StopWork();
	static  unsigned int  _stdcall ServiceProc(void *lpParam);
	static unsigned int _stdcall AcceptProc(void *lpParam);
	ClientList& GetClientList(){return m_clients;}
	void AddClient(SOCKET socketClient,SocketClient* pClient);
	void DeleteClient(SocketClient *pClient);
private:
	void StartService();
	bool InitWinSock32Dlls();
	void InitICOP();
	void InitServiceThreads();
	void InitServerSocket();
	void CleanSocketUp();
	void StartAccepting();
	void StopAcceptThread();
	void StopIconSeviceThread();
private:
	HANDLE m_hIcop;
	bool m_bRunning;
	ClientList m_clients;
	SOCKET m_socketServer;
	HANDLE m_hAcceptThread;
	HANDLE *m_pHICOPThreads;
	map<SOCKET,SocketClient*>m_mpSocket2Client;
	CRITICAL_SECTION m_csMpSocket;
};

