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
	// �������ݰ�
	BOOL Send();
	// �������ݰ�
	BOOL Recv();
	// ����ָ�����ȵ����ݰ�
	BOOL Recv(char *pBuffer,int length);
	// ��ȡSOCKET
	const SOCKET& GetSocket() const;
	// ��ȡ��ַ
	const SOCKADDR_IN& GetAddress() const;
	// ���Ͷ�����������ݰ�
	void AddSendQueue(PPACKET_HDR packet);
	// ע������͵����ݰ�
	void Attach(int packetType,ISocketObserver* pSocketObserver);
	// ȡ�������͵����ݰ�
	void Detach(int packetType);
	// ������������
	void SetHostWnd(HWND hWnd);
	// �Ƿ�Ӧ�ñ���������ɾ�����������ͻ��˸����ڹ�����Ӧ���ɸ���������ɾ���ÿͻ���
	BOOL IsNeedDeleteByHost();
	// ֪ͨ�������ڴ˿ͻ���Ӧ�ñ�ɾ����
	void NotifyHostINeedDeleteNow();
private:
	// �������ݰ�
	void HandlePacket(PPACKET_HDR packet);

private:
	// �������ھ��
	HWND m_HostWnd;
	// �ÿͻ����׽���
	SOCKET m_socket;
	// �ÿͻ��˵�ַ
	SOCKADDR_IN m_address;
	// ���ݰ����Ͷ���
	queue<PPACKET_HDR>m_queueSend;
	// �ٽ����
	CRITICAL_SECTION m_csQueue;
	CRITICAL_SECTION m_csDataObservers;
	// ���ݰ��۲�����
	map<int,ISocketObserver*>m_dataObservers;
};