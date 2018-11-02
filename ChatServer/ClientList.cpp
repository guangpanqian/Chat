#include "StdAfx.h"
#include "ClientList.h"
#include "ChatServer.h"
extern CChatServerApp theApp;

ClientList::ClientList(void)
{
	InitCriticalSection();
}


ClientList::~ClientList(void)
{
	DeleteAllClients();
	ReleaseCriticalSection();
}

void ClientList::InitCriticalSection()
{
	InitializeCriticalSection(&m_csList);
}

void ClientList::ReleaseCriticalSection()
{
	DeleteCriticalSection(&m_csList);
}

void ClientList::DeleteAllClients()
{
	EnterCriticalSection(&m_csList);
	list<SocketClient*>::iterator first = m_clients.begin();
	list<SocketClient*>::iterator last = m_clients.end();
	for(;first!=last;++first)
	{
		delete *first;
	}
	m_clients.clear();
	LeaveCriticalSection(&m_csList);
}


void ClientList::AddClient(SocketClient *pClient)
{
	EnterCriticalSection(&m_csList);
	m_clients.push_back(pClient);
	LeaveCriticalSection(&m_csList);
}

void ClientList::DeleteClient(SocketClient *pClient)
{
	EnterCriticalSection(&m_csList);
	list<SocketClient*>::iterator first = m_clients.begin();
	list<SocketClient*>::iterator last = m_clients.end();
	for (;first!=last;++first)
	{
		if ((*first) == pClient)
		{
			// 通知主线程，该客户端掉线了
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_CLIENT_LOST_TOUCH,pClient->GetSocket(),0);
			delete pClient;
			m_clients.erase(first);
			break;
		}
	}
	LeaveCriticalSection(&m_csList);
}

SOCKADDR_IN ClientList::GetAddress(SOCKET socket)
{
	SOCKADDR_IN ret;
	EnterCriticalSection(&m_csList);
	list<SocketClient*>::iterator first = m_clients.begin();
	list<SocketClient*>::iterator last = m_clients.end();
	for(;first!=last;++first)
	{
		if ((*first)->GetSocket() == socket)
		{
			ret = (*first)->GetAddress();
			break;
		}
	}
	LeaveCriticalSection(&m_csList);
	return ret;
}
