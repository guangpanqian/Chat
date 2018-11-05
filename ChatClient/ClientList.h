#pragma once
class CClient;
#include <list>
using namespace  std;

// �ͻ����б�
class CClientList
{

public:
	CClientList(void);
	~CClientList(void);

	// ��ӿͻ���
	void AddClient(CClient* pClient);
	// ɾ��һ���ͻ���
	void DeleteClient(CClient* pClient);
	// ��ȡһ���ͻ���
	CClient *GetClient(SOCKET socket);
	// ������пͻ���
	void DeleteAllClients();

private:
	// �ͻ����б�
	list<CClient*>m_clients;

};

