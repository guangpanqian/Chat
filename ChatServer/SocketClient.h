#pragma once
#include "CommonDef.h"
#include "EmployeeDBOperator.h"
class SocketClient
{
public:
	SocketClient(SOCKET socket,SOCKADDR_IN address);
	~SocketClient(void);

	bool AsyncSend();
	bool AsyncRecv();
	void HandlData(LPOVERLAPPED lpOverlapped,DWORD dwNumberOfBytesTransffed);
	const SOCKET & GetSocket() const;
	const SOCKADDR_IN &GetAddress() const;
private:
	SOCKET m_socket;
	SOCKADDR_IN m_address;
	IO_OPERATION_DATA m_io;
private:
	void DealPacketLogin();
	void DealPacketUsers(int packetType = PACKET_ID_USERS);
	void DealPacketHeart();
	void DealPacketOffline();
	void DealPacketLoadOffline();
};

