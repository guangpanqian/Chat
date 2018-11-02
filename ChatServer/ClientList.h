#pragma once
#include <list>
#include "SocketClient.h"
using namespace  std;

class ClientList
{

public:
	ClientList(void);
	~ClientList(void);
	void AddClient(SocketClient *pClient);
	void DeleteClient(SocketClient *pClient); 
	SOCKADDR_IN GetAddress(SOCKET socket);
private:
	void InitCriticalSection();
	void ReleaseCriticalSection();
	void DeleteAllClients();

	list<SocketClient*>m_clients;
	CRITICAL_SECTION m_csList;

};

