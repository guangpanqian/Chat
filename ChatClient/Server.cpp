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
	// ����SOCKET DLL
	if (!InitSocketDLLs())
		return FALSE;

	// ��ʼ���׽���
	if (!InitServerSocket())
		return FALSE;

	// �����׽��֣��������û�����ʱ��ֱ������
	if (!InitListenSocket())
		return FALSE;

	// ����select����
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
	// ������ʽ�׽���
	socketServer = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == socketServer)
	{
		CleanUp();
		return FALSE;
	}

	SOCKADDR_IN addressServer;
	addressServer.sin_family = AF_INET;
	addressServer.sin_port = htons(theApp.serverPort);
	addressServer.sin_addr.s_addr = inet_addr(theApp.serverIP);
	int ret = connect(socketServer,PSOCKADDR(&addressServer),sizeof(addressServer));
	if (ret == SOCKET_ERROR)
	{
		CleanUp();
		return FALSE;
	}

	// �����ӷ��������׽��ּ�������
	pServer = new CClient(socketServer,addressServer);
	clients.AddClient(pServer);

	return TRUE;
}

BOOL Server::InitListenSocket()
{
	// ���������׽��֣� ���������û�������
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
		MessageBox(NULL,"��SOCKETʧ��.","����",0);
		return FALSE;
	}

	int length = sizeof(SOCKADDR_IN);
	getsockname(socketListen,PSOCKADDR(&addressListen),&length);
	portListen = addressListen.sin_port;

	if (SOCKET_ERROR == listen(socketListen,SOMAXCONN))
	{
		int errCode = WSAGetLastError();
		MessageBox(NULL,"����SOCKETʧ��.","����",0);
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
	// �������׽��֡�������׽��ֶ����뼯��
	FD_SET(pServer->socketServer,&setAll);
	FD_SET(pServer->socketListen,&setAll);
	LeaveCriticalSection(&pServer->csSet);

	// ���ó�ʱʱ��
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

		// ��ȡ���׽��֡�д�׽��ֵ�״̬
		int selectRet = select(0,&setRead,&setWrite,NULL,&selectTime);
		if (SOCKET_ERROR == selectRet)
		{
			PostQuitMessage(0);
			return -1;
		}

		for (u_int socketIndex = 0;socketIndex < setAll.fd_count;++socketIndex)
		{
			SOCKET &currentSocket = setAll.fd_array[socketIndex];

			// ����׽��ֿ��Զ�ȡ��
			if (FD_ISSET(currentSocket,&setRead))
			{
				// ����Ǽ����׽��֣�˵�����������ˡ���
				if (currentSocket == pServer->socketListen)
				{
					// ��ȡ���������׽���
					SOCKADDR_IN clientAddress;
					int addressLen = sizeof(clientAddress);
					SOCKET socketAccept = accept(pServer->socketListen,(PSOCKADDR)&clientAddress,&addressLen);

					// ����ͻ�������
					CClient *pClient = new CClient(socketAccept,clientAddress);
					pServer->AddClient(socketAccept,pClient);

					// ֪ͨ���̣߳������ӹ�����
					SendMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_CLIENT_ACCEPT,WPARAM(pClient),LPARAM(0));
				}
				// ���Ǽ����׽��֣���ô˵���������ݿ��Զ�
				else
				{
					// ��ȡ��ǰ�ͻ���
					CClient *pClient = pServer->clients.GetClient(currentSocket);
					if (NULL != pClient)
					{
						// ��ȡ����
						if (!pClient->Recv())
						{
							pServer->DeleteClient(currentSocket,pClient);
						}
					}
				}
			}
			// ����׽��ֿ���д����
			if (FD_ISSET(currentSocket,&setWrite))
			{
				CClient *pClient = pServer->clients.GetClient(currentSocket);
				if (NULL != pClient)
				{
					// �������ݰ�
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
	// ��Ҫ������׽��ַ��뵽�����У���������select��
	FD_SET(socket,&setAll);
	clients.AddClient(pClient);
	LeaveCriticalSection(&csSet);
}

void Server::DeleteClient(SOCKET socket, CClient *pClient)
{
	EnterCriticalSection(&csSet);
	// �Ӽ����а�����׽���ɾ���������ٹ�������
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
