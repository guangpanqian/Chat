#pragma once
#include "CommonDef.h"
#include "EmployeeDBOperator.h"
// 封装一个客户端对象，完成数据的发送、接收
class SocketClient
{
public:
	SocketClient(SOCKET socket,SOCKADDR_IN address);
	~SocketClient(void);

	// 异步发送
	bool AsyncSend();
	// 异步接收
	bool AsyncRecv();
	// 处理接收到的数据包
	void HandlData(LPOVERLAPPED lpOverlapped,DWORD dwNumberOfBytesTransffed);
	// 获取SOCKET
	const SOCKET & GetSocket() const;
	// 获取地址
	const SOCKADDR_IN &GetAddress() const;
private:
	SOCKET m_socket;
	SOCKADDR_IN m_address;
	IO_OPERATION_DATA m_io;
private:
	// 处理登陆包
	void DealPacketLogin();
	// 处理用户信息包
	void DealPacketUsers(int packetType = PACKET_ID_USERS);
	// 处理心跳包
	void DealPacketHeart();
	// 处理添加离线信息包
	void DealPacketOffline();
	// 处理获取离线信息包
	void DealPacketLoadOffline();
};

