#include "StdAfx.h"
#include "Client.h"
#include "CommonDef.h"

CClient::CClient(SOCKET socket,SOCKADDR_IN address)
	:m_socket(socket),
	m_address(address)
	,m_HostWnd(NULL)
{
	InitializeCriticalSection(&m_csQueue);
	InitializeCriticalSection(&m_csDataObservers);
}


CClient::~CClient(void)
{
	closesocket(m_socket);
	DeleteCriticalSection(&m_csQueue);
	DeleteCriticalSection(&m_csDataObservers);
}

void CClient::SetHostWnd(HWND hWnd)
{
	m_HostWnd = hWnd;
}

BOOL CClient::IsNeedDeleteByHost()
{
	return NULL!= m_HostWnd;
}

void CClient::NotifyHostINeedDeleteNow()
{
	::SendMessage(m_HostWnd,WM_SOCKET_INVALID,0,0);
}

BOOL CClient::Send()
{
	if (m_queueSend.empty())
	{
		Sleep(10);
		return TRUE;
	}

	PPACKET_HDR packet = NULL;
	EnterCriticalSection(&m_csQueue);
	packet = m_queueSend.front();
	m_queueSend.pop();
	LeaveCriticalSection(&m_csQueue);

	int nSendThisTime = 0;
	int nLeft = packet->len;
	int nTotalSend = 0;

	while(nLeft > 0)
	{
		nSendThisTime = send(m_socket,(char*)packet+ nTotalSend,nLeft,0);
		if (nSendThisTime <= 0)
		{
			return FALSE;
		}

		nLeft -= nSendThisTime;
		nTotalSend += nSendThisTime;
	}

	delete packet;
	return TRUE;
}

BOOL CClient::Recv()
{
	PACKET_HDR packetHeader;
	if (!Recv((char*)&packetHeader,sizeof(packetHeader)))
		return FALSE;

	// 读取完处理该数据包
	HandlePacket(&packetHeader);

	return TRUE;
}

BOOL CClient::Recv(char *pBuffer,int length)
{
	int nRecvThisTime = 0;
	int nLeft = length;
	int nTotalRecv = 0;
	while(nLeft > 0)
	{
		nRecvThisTime = recv(m_socket,pBuffer + nTotalRecv,nLeft,0);
		if (nRecvThisTime <= 0)
		{
			return FALSE;
		}

		nTotalRecv += nRecvThisTime;
		nLeft -= nRecvThisTime;
	}
	return TRUE;
}

void CClient::HandlePacket(PPACKET_HDR packet)
{
	map<int,ISocketObserver*>::iterator pos;
	EnterCriticalSection(&m_csDataObservers);
	pos = m_dataObservers.find(packet->type);
	LeaveCriticalSection(&m_csDataObservers);

	if (pos != m_dataObservers.end())
	{
		// 该观察者处理之
		pos->second->RecvPacket(packet);
	}
}

const SOCKET& CClient::GetSocket() const
{
	return m_socket;
}

const SOCKADDR_IN& CClient::GetAddress() const
{
	return m_address;
}

void CClient::AddSendQueue(PPACKET_HDR packet)
{
	EnterCriticalSection(&m_csQueue);
	m_queueSend.push(packet);
	LeaveCriticalSection(&m_csQueue);
}

void CClient::Attach(int packetType,ISocketObserver* pSocketObserver)
{
	// 设置观察者
	EnterCriticalSection(&m_csDataObservers);
	if (m_dataObservers.find(packetType) == m_dataObservers.end())
	{
		m_dataObservers.insert(pair<int,ISocketObserver*>(packetType,pSocketObserver));
	}
	else
	{
		ASSERT(FALSE);
	}
	LeaveCriticalSection(&m_csDataObservers);
}

void CClient::Detach(int packetType)
{
	// 取消观察者
	EnterCriticalSection(&m_csDataObservers);
	map<int,ISocketObserver*>::iterator pos = m_dataObservers.find(packetType);
	if (m_dataObservers.find(packetType) != m_dataObservers.end())
	{
		m_dataObservers.erase(pos);
	}
	LeaveCriticalSection(&m_csDataObservers);
}
