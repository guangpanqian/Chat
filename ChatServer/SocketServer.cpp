#include "StdAfx.h"
#include "SocketServer.h"
#include <WinSock2.h>
#include "SocketClient.h"
#include "..\include\Package\packetDef.h"
#include "ChatServer.h"
#include "CommonDef.h"
#include "MainFrm.h"

extern CChatServerApp theApp;

#define SERVICE_WAIT_TIME  1000

SocketServer::SocketServer(void)
	:m_hIcop(NULL)
	,m_bRunning(true)
	,m_socketServer(NULL)
	,m_hAcceptThread(NULL)
	,m_pHICOPThreads(NULL)
{
	InitializeCriticalSection(&m_csMpSocket);
}


SocketServer::~SocketServer(void)
{
	WaitForSingleObject(m_hAcceptThread,INFINITE);
	DeleteCriticalSection(&m_csMpSocket);
}

void SocketServer::StartWork()
{
	// 加载 SOCKET DLLs
	if (InitWinSock32Dlls())
	{
		// 启动服务
		StartService();
	}
}

bool SocketServer::InitWinSock32Dlls()
{
	WSADATA wsaData;
	return 0 == WSAStartup(MAKEWORD(2,2),&wsaData);
}

void SocketServer::StartService()
{
	// 初始化IO完成端口
	InitICOP();
	((CMainFrame*)(theApp.m_pMainWnd))->LogInfo("**********初始化完成端口完毕**********");

	// 初始化服务线程
	InitServiceThreads();
	((CMainFrame*)(theApp.m_pMainWnd))->LogInfo("**********初始化服务线程完毕**********");

	// 初始化监听线程
	InitServerSocket();
	((CMainFrame*)(theApp.m_pMainWnd))->LogInfo("**********初始化监听线程完毕**********");
}

void SocketServer::InitICOP()
{
	// 创建IO完成端口，IO线程默认与CPU个数相同，下面的步骤需要将该句柄与SOCKET关联到一起
	m_hIcop = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
}

void SocketServer::InitServiceThreads()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	m_pHICOPThreads = new HANDLE[systemInfo.dwNumberOfProcessors * 2];
	// 创建2倍CPU数个服务线程，比IO线程多是为了让CPU更加有效
	for (DWORD threadIndex = 0;threadIndex < systemInfo.dwNumberOfProcessors * 2;++threadIndex)
	{
		m_pHICOPThreads[threadIndex] = (HANDLE) _beginthreadex(NULL,0,ServiceProc,this,0,NULL);
	}
}

void SocketServer::InitServerSocket()
{
	// 创建一个与重叠结构相关的SOCKET
	m_socketServer = WSASocket(
								AF_INET,
								SOCK_STREAM,
								IPPROTO_TCP,
								NULL,
								NULL,
								WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == m_socketServer)
	{
		CleanSocketUp();
		return;
	}

	SOCKADDR_IN serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(CHAT_SERVER_PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	// 绑定该地址
	if (SOCKET_ERROR == ::bind(m_socketServer,PSOCKADDR(&serverAddress),sizeof(serverAddress)))
	{
		CleanSocketUp();
		return;
	}

	//	监听该套接字
	if (SOCKET_ERROR == listen(m_socketServer,SOMAXCONN))
	{
		CleanSocketUp();
		return;
	}

	//	启动Accept线程
	StartAccepting();

}

void SocketServer::CleanSocketUp()
{
	if (NULL != m_socketServer)
		closesocket(m_socketServer);

	WSACleanup();
}

void SocketServer::StartAccepting()
{
	m_hAcceptThread = (HANDLE)_beginthreadex(NULL,0,AcceptProc,this,0,NULL);
}

void SocketServer::StopWork()
{
	// 停止Accept线程
	StopAcceptThread();
	// 停止完成端口服务线程
	StopIconSeviceThread();
}

void SocketServer::StopAcceptThread()
{
	m_bRunning = false;
	WaitForSingleObject(m_hAcceptThread,INFINITE);
	CloseHandle(m_hAcceptThread);
}

void SocketServer::StopIconSeviceThread()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	for (DWORD threadIndex = 0;threadIndex < systemInfo.dwNumberOfProcessors * 2;++threadIndex)
	{
		// 向完成端口发取消消息
		PostQueuedCompletionStatus(m_hIcop,0,NULL,NULL);
	}

	WaitForMultipleObjects(systemInfo.dwNumberOfProcessors*2,m_pHICOPThreads,TRUE,INFINITE);

}

unsigned int _stdcall SocketServer::ServiceProc(void *lpParam)
{
	while(true)
	{
		Sleep(1);

		SocketServer *pServer = (SocketServer*)lpParam;
		SocketClient* pClient = NULL;
		LPOVERLAPPED lpOverlapped;

		DWORD dwNumberOfBytesTransffed = 0;
		// 获取完成端口的状态
		BOOL bGetQueueStatusSucceed = GetQueuedCompletionStatus(
			pServer->m_hIcop,
			&dwNumberOfBytesTransffed,
			(PULONG_PTR)&pClient,
			&lpOverlapped,
			INFINITE
			);

		// 服务停止运行并且完成键值为空并且重叠结构为空，线程应该退出了
		if (!pServer->m_bRunning && NULL == pClient && NULL == lpOverlapped)
		{
			break;
		}

		// 如果没有查到完成端口的状态
		if (!bGetQueueStatusSucceed)
		{
			// 超时了？按理说不会走到这里，前面有INFINITE
			if (WAIT_TIMEOUT == GetLastError())
			{
				continue;
			}
			// 没查到状态完成键却不为空，套接字异常，应该删除
			else if (NULL != pClient)
			{
				pServer->m_clients.DeleteClient(pClient);
			}
			else
			{
				break;
			}
		}
		else
		{
			// 传输数据为空，也该删除了
			if (0 == dwNumberOfBytesTransffed)
			{
				pServer->m_clients.DeleteClient(pClient);
				pClient = NULL;
			}

			// 接收到了数据，处理之
			if (pClient)
			{
				pClient->HandlData(lpOverlapped,dwNumberOfBytesTransffed);
			}
		}
	}
	return 0;
}

unsigned int _stdcall SocketServer::AcceptProc(void *lpParam)
{
	SocketServer *pServer = (SocketServer*)lpParam;

	// 初始化等待时间
	struct timeval selectTime;  
	selectTime.tv_sec = 30;
	selectTime.tv_usec = 0;

	FD_SET socketSet;
	FD_SET socketRead;
	FD_ZERO(&socketSet);

	// 将监听的套接字放入集合
	FD_SET(pServer->m_socketServer,&socketSet);

	while(true)
	{
		Sleep(1);

		if (!pServer->m_bRunning)
		{
			break;
		}	

		// 清空集合
		FD_ZERO(&socketRead);
		socketRead = socketSet;

		int ret  = select(0,&socketRead,NULL,NULL,&selectTime);
		if (SOCKET_ERROR == ret)
		{
			return -1;
		}

		for (int socketIndex = 0;socketIndex < ret;++socketIndex)
		{
			// 套接字还在数组中
			if (FD_ISSET(socketSet.fd_array[socketIndex],&socketRead))
			{
				// 监听套接字有读事件发生
				if (socketSet.fd_array[socketIndex] == pServer->m_socketServer)
				{

					// Accept 该套接字
					SOCKADDR_IN clientAddress;
					int lenth = sizeof(SOCKADDR_IN);
					SOCKET socketClient = accept(pServer->m_socketServer,(PSOCKADDR)&clientAddress,&lenth);
					if (INVALID_SOCKET == socketClient)
						continue;

					// 通知主线程有套接字过来
					::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_ACCEPT,WPARAM(&clientAddress),0);

					// 创建一个客户端对象并放入客户端链表
					SocketClient *pClient = new SocketClient(socketClient,clientAddress);
					pServer->AddClient(socketClient,pClient);

					// 将该套接字与完成端口关联起来，并传入完全键的地址，完全键记录与链接相关的信息，等到获取完全端口状态时，该完全键能获取到
					// 这里不再需要 one-client-one-thread, 该模型认为线程应该有个上限，这里使用2倍CPU的服务线程池
					// 对于同一个套接字，发送数据和接收数据可以在不同线程中
					HANDLE hIcop = CreateIoCompletionPort((HANDLE)socketClient,pServer->m_hIcop,(ULONG_PTR)pClient,0);
					
					// 发起一个异步的接收
					if (!pClient->AsyncRecv())
					{
						// 出异常了，就从链表中把该客户端删除掉
						pServer->DeleteClient(pClient);
					}
				}
			}
		}
	}
	return 0;
}

void SocketServer::AddClient(SOCKET socketClient,SocketClient* pClient)
{
	EnterCriticalSection(&m_csMpSocket);
	m_mpSocket2Client.insert(pair<SOCKET,SocketClient*>(socketClient,pClient));
	LeaveCriticalSection(&m_csMpSocket);

	// 加入链表
	m_clients.AddClient(pClient);
}

void SocketServer::DeleteClient(SocketClient *pClient)
{
	// 移除链表
	m_clients.DeleteClient(pClient);

	EnterCriticalSection(&m_csMpSocket);

	map<SOCKET,SocketClient*>::iterator first = m_mpSocket2Client.begin();
	map<SOCKET,SocketClient*>::iterator last = m_mpSocket2Client.end();
	for (;first!=last;++first)
	{
		if ((*first).second == pClient)
		{
			m_mpSocket2Client.erase(first);
			break;
		}
	}
	LeaveCriticalSection(&m_csMpSocket);
}
