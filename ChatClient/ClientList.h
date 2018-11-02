#pragma once
class CClient;
#include <list>
using namespace  std;

class CClientList
{

public:
	CClientList(void);
	~CClientList(void);

	void AddClient(CClient* pClient);
	void DeleteClient(CClient* pClient);
	CClient *GetClient(SOCKET socket);
	void DeleteAllClients();

private:
	list<CClient*>m_clients;

};

