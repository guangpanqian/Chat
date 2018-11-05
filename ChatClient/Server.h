#pragma once
#include <WinSock2.h>
#include <queue>
#include <map>
#include "..\include\Package\packetDef.h"
#include "Interfaces.h"
#include "ClientList.h"
using namespace  std;
// ÿ������Ŀͻ���Ҳ����������������ʱ�������ͻ���ֱ�����ӣ����ӱ�˫�����û���
class Server
{
public:
	Server(void);
	~Server(void);
	// ��������
	BOOL StartWork();
	// ֹͣ����
	void StopWork();
	// �����߳�
	void StartService();
	// ɾ�����пͻ���
	void DeleteAllClients();
	// ��ӿͻ���
	CClient* AddSocket(SOCKET socket, SOCKADDR_IN address);
	// ɾ���ͻ���
	void DeleteClient(SOCKET socket, CClient *pClient);
	// ��ӿͻ���
	void AddClient(SOCKET socket, CClient *pClient);
private:
	// �����׽���
	void CleanUp();
	// ��ʼ��DLL
	BOOL InitSocketDLLs();
	// ��ʼ��������׽���
	BOOL InitServerSocket();
	// ��ʼ�������׽���
	BOOL InitListenSocket();
	// select�߳̾��
	HANDLE hSelectThread;

public:
	// �������׽���
	SOCKET socketServer;
	// �����׽���
	SOCKET socketListen;
	// �����˿�
	u_short portListen;
	// ����������
	BOOL bRunning;
	// SOCKET ����
	FD_SET setAll;
	// �ٽ�������
	CRITICAL_SECTION csSet;
	// �ͻ����б�
	CClientList clients;
	// ����������
	CClient *pServer; 
};

