#pragma once
#include <list>
#include "SocketClient.h"
using namespace  std;

// 维护一个客户端的列表，使用std::list是因为可能需要频繁的增删结点
// 注意线程安全问题增加临界区变量
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

