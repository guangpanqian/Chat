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
	// 发送数据包
	BOOL Send();
	// 接收数据包
	BOOL Recv();
	// 接收指定长度的数据包
	BOOL Recv(char *pBuffer,int length);
	// 获取SOCKET
	const SOCKET& GetSocket() const;
	// 获取地址
	const SOCKADDR_IN& GetAddress() const;
	// 像发送队列里添加数据包
	void AddSendQueue(PPACKET_HDR packet);
	// 注册该类型的数据包
	void Attach(int packetType,ISocketObserver* pSocketObserver);
	// 取消该类型的数据包
	void Detach(int packetType);
	// 设置宿主窗口
	void SetHostWnd(HWND hWnd);
	// 是否应该被宿主窗口删除，如果这个客户端跟窗口关联，应该由该宿主窗口删除该客户端
	BOOL IsNeedDeleteByHost();
	// 通知宿主窗口此客户端应该被删除了
	void NotifyHostINeedDeleteNow();
private:
	// 处理数据包
	void HandlePacket(PPACKET_HDR packet);

private:
	// 宿主窗口句柄
	HWND m_HostWnd;
	// 该客户端套接字
	SOCKET m_socket;
	// 该客户端地址
	SOCKADDR_IN m_address;
	// 数据包发送队列
	queue<PPACKET_HDR>m_queueSend;
	// 临界变量
	CRITICAL_SECTION m_csQueue;
	CRITICAL_SECTION m_csDataObservers;
	// 数据包观察者们
	map<int,ISocketObserver*>m_dataObservers;
};