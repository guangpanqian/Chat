#pragma once
#include "ClientList.h"
#include <map>
using namespace  std;
class SocketClient;

// 封装服务器SOCKET 使用IO完成端口模型，select模型
class SocketServer
{
public:
	SocketServer(void);
	~SocketServer(void);
public:
	// 启动服务
	void StartWork();
	// 关闭服务
	void StopWork();
	// 完成端口服务线程
	static  unsigned int  _stdcall ServiceProc(void *lpParam);
	// 接收处理线程，关联完成端口与套接字
	static unsigned int _stdcall AcceptProc(void *lpParam);
	// 获取客户端列表
	ClientList& GetClientList(){return m_clients;}
	// 增加一个客户端
	void AddClient(SOCKET socketClient,SocketClient* pClient);
	// 删除一个客户端
	void DeleteClient(SocketClient *pClient);
private:
	// 启动服务
	void StartService();
	// 初始化DLL
	bool InitWinSock32Dlls();
	// 初始化完成端口
	void InitICOP();
	// 初始化完成端口服务线程
	void InitServiceThreads();
	// 初始化服务器套接字
	void InitServerSocket();
	// 套接字清理
	void CleanSocketUp();
	// 启动接收
	void StartAccepting();
	// 停止接收
	void StopAcceptThread();
	// 停止完成端口服务线程
	void StopIconSeviceThread();
private:
	// IO完成端口句柄
	HANDLE m_hIcop;
	// 服务器运行标识
	bool m_bRunning;
	// 客户端在线列表
	ClientList m_clients;
	// 服务器套接字
	SOCKET m_socketServer;
	// 接收线程句柄
	HANDLE m_hAcceptThread;
	// IO完成端口服务线程的句柄
	HANDLE *m_pHICOPThreads;
	// 套接字到客户端的映射
	map<SOCKET,SocketClient*>m_mpSocket2Client;
	// 线程保护变量
	CRITICAL_SECTION m_csMpSocket;
};

