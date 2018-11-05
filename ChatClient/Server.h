#pragma once
#include <WinSock2.h>
#include <queue>
#include <map>
#include "..\include\Package\packetDef.h"
#include "Interfaces.h"
#include "ClientList.h"
using namespace  std;
// 每个聊天的客户端也做监听，在线聊天时，两个客户端直接连接，连接被双击的用户。
class Server
{
public:
	Server(void);
	~Server(void);
	// 启动服务
	BOOL StartWork();
	// 停止服务
	void StopWork();
	// 启动线程
	void StartService();
	// 删除所有客户端
	void DeleteAllClients();
	// 添加客户端
	CClient* AddSocket(SOCKET socket, SOCKADDR_IN address);
	// 删除客户端
	void DeleteClient(SOCKET socket, CClient *pClient);
	// 添加客户端
	void AddClient(SOCKET socket, CClient *pClient);
private:
	// 清理套接字
	void CleanUp();
	// 初始化DLL
	BOOL InitSocketDLLs();
	// 初始化服务端套接字
	BOOL InitServerSocket();
	// 初始化监听套接字
	BOOL InitListenSocket();
	// select线程句柄
	HANDLE hSelectThread;

public:
	// 服务器套接字
	SOCKET socketServer;
	// 监听套接字
	SOCKET socketListen;
	// 监听端口
	u_short portListen;
	// 监听在运行
	BOOL bRunning;
	// SOCKET 集合
	FD_SET setAll;
	// 临界区变量
	CRITICAL_SECTION csSet;
	// 客户端列表
	CClientList clients;
	// 服务器对象
	CClient *pServer; 
};

