#pragma once
#include "CommonDef.h"
#include "EmployeeDBOperator.h"
// ��װһ���ͻ��˶���������ݵķ��͡�����
class SocketClient
{
public:
	SocketClient(SOCKET socket,SOCKADDR_IN address);
	~SocketClient(void);

	// �첽����
	bool AsyncSend();
	// �첽����
	bool AsyncRecv();
	// ������յ������ݰ�
	void HandlData(LPOVERLAPPED lpOverlapped,DWORD dwNumberOfBytesTransffed);
	// ��ȡSOCKET
	const SOCKET & GetSocket() const;
	// ��ȡ��ַ
	const SOCKADDR_IN &GetAddress() const;
private:
	SOCKET m_socket;
	SOCKADDR_IN m_address;
	IO_OPERATION_DATA m_io;
private:
	// �����½��
	void DealPacketLogin();
	// �����û���Ϣ��
	void DealPacketUsers(int packetType = PACKET_ID_USERS);
	// ����������
	void DealPacketHeart();
	// �������������Ϣ��
	void DealPacketOffline();
	// �����ȡ������Ϣ��
	void DealPacketLoadOffline();
};

