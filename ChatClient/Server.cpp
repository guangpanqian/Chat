#include "StdAfx.h"
#include "Server.h"
#include "..\include\Package\packetDef.h"
#include "CommonDef.h"
#include "ChatClient.h"
#include "Client.h"
extern CChatClientApp theApp;

Server::Server(void)
	:socketServer(INVALID_SOCKET)
	,bRunning(TRUE)
	,pServer(NULL)
	,portListen(0)
{
	InitializeCriticalSection(&csSet);
}


Server::~Server(void)
{
	DeleteCriticalSection(&csSet);
}

BOOL Server::StartWork()
{
	// 加载SOCKET DLL
	if (!InitSocketDLLs())
		return FALSE;

	// 初始化套接字
	if (!InitServerSocket())
		return FALSE;

	// 监听套接字，跟在线用户聊天时，直接链接
	if (!InitListenSocket())
		return FALSE;

	// 启动select服务
	StartService();

	return TRUE;
}

BOOL Server::InitSocketDLLs()
{
	WSAData wsaData;
	if (0 != WSAStartup(MAKEWORD(2,2),&wsaData))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL Server::InitServerSocket()
{
	// 创建流式套接字
	socketServer = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == socketServer)
	{
		CleanUp();
		return FALSE;
	}

	SOCKADDR_IN addressServer;
	addressServer.sin_family = AF_INET;
	addressServer.sin_port = htons(CHAT_SERVER_PORT);
	addressServer.sin_addr.s_addr = inet_addr(CHAT_SERVER_IP);
	int ret = connect(socketServer,PSOCKADDR(&addressServer),sizeof(addressServer));
	if (ret == SOCKET_ERROR)
	{
		CleanUp();
		return FALSE;
	}

	// 将链接服务器的套接字加入链表
	pServer = new CClient(socketServer,addressServer);
	clients.AddClient(pServer);

	return TRUE;
}

BOOL Server::InitListenSocket()
{
	// 创建监听套接字， 监听其他用户的连接
	socketListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == socketListen)
	{
		CleanUp();
		return FALSE;
	}

	SOCKADDR_IN addressListen;
	addressListen.sin_family = AF_INET;
	addressListen.sin_port = 0;
	addressListen.sin_addr.s_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(socketListen,PSOCKADDR(&addressListen),sizeof(addressListen)))
	{
		int errCode = WSAGetLastError();
		MessageBox(NULL,"绑定SOCKET失败.","错误",0);
		return FALSE;
	}

	int length = sizeof(SOCKADDR_IN);
	getsockname(socketListen,PSOCKADDR(&addressListen),&length);
	portListen = addressListen.sin_port;

	if (SOCKET_ERROR == listen(socketListen,SOMAXCONN))
	{
		int errCode = WSAGetLastError();
		MessageBox(NULL,"监听SOCKET失败.","错误",0);
		return FALSE;
	}
	return TRUE;
}

void Server::CleanUp()
{
	WSACleanup();
	if (INVALID_SOCKET != socketServer)
		closesocket(socketServer);

	DeleteAllClients();
}

void Server::StopWork()
{
	bRunning = FALSE;
	WaitForSingleObject(hSelectThread,INFINITE);
	CloseHandle(hSelectThread);

	CleanUp();
}

unsigned int _stdcall SelectThreadProc(void *param)
{
	Server *pServer = (Server*)param;
	FD_SET &setAll =  pServer->setAll;
	FD_SET setRead;
	FD_SET setWrite;

	EnterCriticalSection(&pServer->csSet);
	FD_ZERO(&setAll);
	// 将监听套接字、服务端套接字都放入集合
	FD_SET(pServer->socketServer,&setAll);
	FD_SET(pServer->socketListen,&setAll);
	LeaveCriticalSection(&pServer->csSet);

	// 设置超时时间
	struct timeval selectTime;
	selectTime.tv_sec = 5;
	selectTime.tv_usec = 0;

	while(pServer->bRunning)
	{
		Sleep(1);

		FD_ZERO(&setRead);
		FD_ZERO(&setWrite);
		setRead = setAll;
		setWrite = setAll;

		// 获取读套接字、写套接字的状态
		int selectRet = select(0,&setRead,&setWrite,NULL,&selectTime);
		if (SOCKET_ERROR == selectRet)
		{
			PostQuitMessage(0);
			return -1;
		}

		for (u_int socketIndex = 0;socketIndex < setAll.fd_count;++socketIndex)
		{
			SOCKET &currentSocket = setAll.fd_array[socketIndex];

			// 这个套接字可以读取了
			if (FD_ISSET(currentSocket,&setRead))
			{
				// 如果是监听套接字，说明是连接来了……
				if (currentSocket == pServer->socketListen)
				{
					// 获取监听到的套接字
					SOCKADDR_IN clientAddress;
					int addressLen = sizeof(clientAddress);
					SOCKET socketAccept = accept(pServer->socketListen,(PSOCKADDR)&clientAddress,&addressLen);

					// 编入客户端链表
					CClient *pClient = new CClient(socketAccept,clientAddress);
					pServer->AddClient(socketAccept,pClient);

					// 通知主线程，有连接过来了
					SendMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_CLIENT_ACCEPT,WPARAM(pClient),LPARAM(0));
				}
				// 不是监听套接字，那么说明，有数据可以读
				else
				{
					// 获取当前客户端
					CClient *pClient = pServer->clients.GetClient(currentSocket);
					if (NULL != pClient)
					{
						// 读取数据
						if (!pClient->Recv())
						{
							pServer->DeleteClient(currentSocket,pClient);
						}
					}
				}
			}
			// 这个套接字可以写入了
			if (FD_ISSET(currentSocket,&setWrite))
			{
				CClient *pClient = pServer->clients.GetClient(currentSocket);
				if (NULL != pClient)
				{
					// 发送数据包
					if (!pClient->Send())
					{
						pServer->DeleteClient(currentSocket,pClient);
					}
				}
			}
		}
	}
	return 0;
}

void Server::AddClient(SOCKET socket, CClient *pClient)
{
	EnterCriticalSection(&csSet);
	// 需要把这个套接字放入到集合中，这样才能select到
	FD_SET(socket,&setAll);
	clients.AddClient(pClient);
	LeaveCriticalSection(&csSet);
}

void Server::DeleteClient(SOCKET socket, CClient *pClient)
{
	EnterCriticalSection(&csSet);
	// 从集合中把这个套接字删掉，不用再管理它了
	FD_CLR(socket,&setAll);
	clients.DeleteClient(pClient);
	LeaveCriticalSection(&csSet);
}

void Server::DeleteAllClients()
{
	EnterCriticalSection(&csSet);
	FD_ZERO(&setAll);
	clients.DeleteAllClients();
	LeaveCriticalSection(&csSet);
}

void Server::StartService()
{
	hSelectThread = (HANDLE)_beginthreadex(NULL,0,SelectThreadProc,this,0,NULL);
}

CClient* Server::AddSocket(SOCKET socket, SOCKADDR_IN address)
{
	CClient *pClient = NULL;
	EnterCriticalSection(&csSet);
	pClient = new CClient(socket,address); 
	clients.AddClient(pClient);
	FD_SET(socket,&setAll);
	LeaveCriticalSection(&csSet);
	return pClient;
}
