#pragma once
#include "ClientList.h"
#include <map>
using namespace  std;
class SocketClient;

// ��װ������SOCKET ʹ��IO��ɶ˿�ģ�ͣ�selectģ��
class SocketServer
{
public:
	SocketServer(void);
	~SocketServer(void);
public:
	// ��������
	void StartWork();
	// �رշ���
	void StopWork();
	// ��ɶ˿ڷ����߳�
	static  unsigned int  _stdcall ServiceProc(void *lpParam);
	// ���մ����̣߳�������ɶ˿����׽���
	static unsigned int _stdcall AcceptProc(void *lpParam);
	// ��ȡ�ͻ����б�
	ClientList& GetClientList(){return m_clients;}
	// ����һ���ͻ���
	void AddClient(SOCKET socketClient,SocketClient* pClient);
	// ɾ��һ���ͻ���
	void DeleteClient(SocketClient *pClient);
private:
	// ��������
	void StartService();
	// ��ʼ��DLL
	bool InitWinSock32Dlls();
	// ��ʼ����ɶ˿�
	void InitICOP();
	// ��ʼ����ɶ˿ڷ����߳�
	void InitServiceThreads();
	// ��ʼ���������׽���
	void InitServerSocket();
	// �׽�������
	void CleanSocketUp();
	// ��������
	void StartAccepting();
	// ֹͣ����
	void StopAcceptThread();
	// ֹͣ��ɶ˿ڷ����߳�
	void StopIconSeviceThread();
private:
	// IO��ɶ˿ھ��
	HANDLE m_hIcop;
	// ���������б�ʶ
	bool m_bRunning;
	// �ͻ��������б�
	ClientList m_clients;
	// �������׽���
	SOCKET m_socketServer;
	// �����߳̾��
	HANDLE m_hAcceptThread;
	// IO��ɶ˿ڷ����̵߳ľ��
	HANDLE *m_pHICOPThreads;
	// �׽��ֵ��ͻ��˵�ӳ��
	map<SOCKET,SocketClient*>m_mpSocket2Client;
	// �̱߳�������
	CRITICAL_SECTION m_csMpSocket;
};

