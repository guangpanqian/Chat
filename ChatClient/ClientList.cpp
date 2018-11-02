#include "StdAfx.h"
#include "ClientList.h"
#include "Client.h"


CClientList::CClientList(void)
{
}

CClientList::~CClientList(void)
{
}

void CClientList::AddClient(CClient* pClient)
{
	m_clients.push_back(pClient);
}

void CClientList::DeleteClient(CClient* pClient)
{
	list<CClient*>::iterator first = m_clients.begin();
	list<CClient*>::iterator last = m_clients.end();
	for (;first!=last;++first)
	{
		if ((*first) == pClient)
		{
			m_clients.erase(first);

			if (pClient->IsNeedDeleteByHost())
			{
				// ֪ͨ���ڴ�������֮
				pClient->NotifyHostINeedDeleteNow();
			}
			else
			{
				// �⼸���پ�����
				delete pClient;
			}
			break;
		}
	}
}

CClient *CClientList::GetClient(SOCKET socket)
{
	CClient *pClient = NULL;
	list<CClient*>::iterator first = m_clients.begin();
	list<CClient*>::iterator last = m_clients.end();
	for (;first!=last;++first)
	{
		if (socket == (*first)->GetSocket())
		{
			pClient = *first;
			break;
		}
	}
	return pClient;
}

void CClientList::DeleteAllClients()
{
	list<CClient*>::iterator first = m_clients.begin();
	list<CClient*>::iterator last = m_clients.end();
	for (;first!=last;++first)
	{
		CClient* pClient = *first;
	
		if (pClient->IsNeedDeleteByHost())
		{
			// ֪ͨ���ڴ�������֮
			pClient->NotifyHostINeedDeleteNow();
		}
		else
		{
			// �⼸���پ�����
			delete pClient;
		}
	}
	m_clients.clear();
}