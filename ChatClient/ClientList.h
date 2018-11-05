#pragma once
class CClient;
#include <list>
using namespace  std;

// 客户端列表
class CClientList
{

public:
	CClientList(void);
	~CClientList(void);

	// 添加客户端
	void AddClient(CClient* pClient);
	// 删除一个客户端
	void DeleteClient(CClient* pClient);
	// 获取一个客户端
	CClient *GetClient(SOCKET socket);
	// 清除所有客户端
	void DeleteAllClients();

private:
	// 客户端列表
	list<CClient*>m_clients;

};

