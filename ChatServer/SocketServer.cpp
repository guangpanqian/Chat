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
	// ���� SOCKET DLLs
	if (InitWinSock32Dlls())
	{
		// ��������
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
	// ��ʼ��IO��ɶ˿�
	InitICOP();

	// ��ʼ�������߳�
	InitServiceThreads();

	// ��ʼ�������߳�
	InitServerSocket();
}

void SocketServer::InitICOP()
{
	// ����IO��ɶ˿ڣ�IO�߳�Ĭ����CPU������ͬ������Ĳ�����Ҫ���þ����SOCKET������һ��
	m_hIcop = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
}

void SocketServer::InitServiceThreads()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	m_pHICOPThreads = new HANDLE[systemInfo.dwNumberOfProcessors * 2];
	// ����2��CPU���������̣߳���IO�̶߳���Ϊ����CPU������Ч
	for (DWORD threadIndex = 0;threadIndex < systemInfo.dwNumberOfProcessors * 2;++threadIndex)
	{
		m_pHICOPThreads[threadIndex] = (HANDLE) _beginthreadex(NULL,0,ServiceProc,this,0,NULL);
	}
}

void SocketServer::InitServerSocket()
{
	// ����һ�����ص��ṹ��ص�SOCKET
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

	// �󶨸õ�ַ
	if (SOCKET_ERROR == ::bind(m_socketServer,PSOCKADDR(&serverAddress),sizeof(serverAddress)))
	{
		CleanSocketUp();
		return;
	}

	//	�������׽���
	if (SOCKET_ERROR == listen(m_socketServer,SOMAXCONN))
	{
		CleanSocketUp();
		return;
	}

	//	����Accept�߳�
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
	// ֹͣAccept�߳�
	StopAcceptThread();
	// ֹͣ��ɶ˿ڷ����߳�
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
		// ����ɶ˿ڷ�ȡ����Ϣ
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
		// ��ȡ��ɶ˿ڵ�״̬
		BOOL bGetQueueStatusSucceed = GetQueuedCompletionStatus(
			pServer->m_hIcop,
			&dwNumberOfBytesTransffed,
			(PULONG_PTR)&pClient,
			&lpOverlapped,
			INFINITE
			);

		// ����ֹͣ���в�����ɼ�ֵΪ�ղ����ص��ṹΪ�գ��߳�Ӧ���˳���
		if (!pServer->m_bRunning && NULL == pClient && NULL == lpOverlapped)
		{
			break;
		}

		// ���û�в鵽��ɶ˿ڵ�״̬
		if (!bGetQueueStatusSucceed)
		{
			// ��ʱ�ˣ�����˵�����ߵ����ǰ����INFINITE
			if (WAIT_TIMEOUT == GetLastError())
			{
				continue;
			}
			// û�鵽״̬��ɼ�ȴ��Ϊ�գ��׽����쳣��Ӧ��ɾ��
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
			// ��������Ϊ�գ�Ҳ��ɾ����
			if (0 == dwNumberOfBytesTransffed)
			{
				pServer->m_clients.DeleteClient(pClient);
				pClient = NULL;
			}

			// ���յ������ݣ�����֮
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

	// ��ʼ���ȴ�ʱ��
	struct timeval selectTime;  
	selectTime.tv_sec = 30;
	selectTime.tv_usec = 0;

	FD_SET socketSet;
	FD_SET socketRead;
	FD_ZERO(&socketSet);

	// ���������׽��ַ��뼯��
	FD_SET(pServer->m_socketServer,&socketSet);

	while(true)
	{
		Sleep(1);

		if (!pServer->m_bRunning)
		{
			break;
		}	

		// ��ռ���
		FD_ZERO(&socketRead);
		socketRead = socketSet;

		int ret  = select(0,&socketRead,NULL,NULL,&selectTime);
		if (SOCKET_ERROR == ret)
		{
			return -1;
		}

		for (int socketIndex = 0;socketIndex < ret;++socketIndex)
		{
			// �׽��ֻ���������
			if (FD_ISSET(socketSet.fd_array[socketIndex],&socketRead))
			{
				// �����׽����ж��¼�����
				if (socketSet.fd_array[socketIndex] == pServer->m_socketServer)
				{

					// Accept ���׽���
					SOCKADDR_IN clientAddress;
					int lenth = sizeof(SOCKADDR_IN);
					SOCKET socketClient = accept(pServer->m_socketServer,(PSOCKADDR)&clientAddress,&lenth);
					if (INVALID_SOCKET == socketClient)
						continue;

					// ֪ͨ���߳����׽��ֹ���
					::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_ACCEPT,WPARAM(&clientAddress),0);

					// ����һ���ͻ��˶��󲢷���ͻ�������
					SocketClient *pClient = new SocketClient(socketClient,clientAddress);
					pServer->AddClient(socketClient,pClient);

					// �����׽�������ɶ˿ڹ�����������������ȫ���ĵ�ַ����ȫ����¼��������ص���Ϣ���ȵ���ȡ��ȫ�˿�״̬ʱ������ȫ���ܻ�ȡ��
					// ���ﲻ����Ҫ one-client-one-thread, ��ģ����Ϊ�߳�Ӧ���и����ޣ�����ʹ��2��CPU�ķ����̳߳�
					// ����ͬһ���׽��֣��������ݺͽ������ݿ����ڲ�ͬ�߳���
					HANDLE hIcop = CreateIoCompletionPort((HANDLE)socketClient,pServer->m_hIcop,(ULONG_PTR)pClient,0);
					
					// ����һ���첽�Ľ���
					if (!pClient->AsyncRecv())
					{
						// ���쳣�ˣ��ʹ������аѸÿͻ���ɾ����
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

	// ��������
	m_clients.AddClient(pClient);
}

void SocketServer::DeleteClient(SocketClient *pClient)
{
	// �Ƴ�����
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
