#pragma once
#include <queue>
#include "..\include\Package\packetDef.h"
#include <map>
#include "Interfaces.h"
using namespace  std;

class CClient
{

public:
	CClient(SOCKET socket,SOCKADDR_IN address);
	~CClient(void);

public:
	BOOL Send();
	BOOL Recv();
	BOOL Recv(char *pBuffer,int length);
	const SOCKET& GetSocket() const;
	const SOCKADDR_IN& GetAddress() const;
	void AddSendQueue(PPACKET_HDR packet);
	void Attach(int packetType,ISocketObserver* pSocketObserver);
	void Detach(int packetType);
	void SetHostWnd(HWND hWnd);
	BOOL IsNeedDeleteByHost();
	void NotifyHostINeedDeleteNow();
private:
	void HandlePacket(PPACKET_HDR packet);

private:
	HWND m_HostWnd;
	SOCKET m_socket;
	SOCKADDR_IN m_address;
	queue<PPACKET_HDR>m_queueSend;
	CRITICAL_SECTION m_csQueue;
	CRITICAL_SECTION m_csDataObservers;
	map<int,ISocketObserver*>m_dataObservers;
};