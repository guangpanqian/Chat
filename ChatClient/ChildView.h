
// ChildView.h : CChildView ��Ľӿ�
//


#pragma once


// CChildView ����

#include <map>
class CClient;
#include "..\include\Package\packetDef.h"
#include "Interfaces.h"
#include "Client.h"
using namespace  std;

class CChatDlg;
class CChildView : public CWnd,public ISocketObserver
{
// ����
public:
	CChildView(CClient *pClient);

// ����
public:

// ����
public:

// ��д
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CChildView();

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// ˫��ĳ���û���������
	afx_msg void OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult);
	// ������Ϣ������
	LRESULT OnOfflineArrived(WPARAM wParam,LPARAM lParam);
	// ������Ϣ����
	LRESULT OnOnlineArrived(WPARAM wParam,LPARAM lParam);
	// SOCKET ��Ч��
	LRESULT OnSocketInvalid(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	// �����û��б�
	CListCtrl   m_lstCtlUsers;
	// ���ߡ�����ͼ��
	CBitmap m_bitmaps[2];
	CImageList m_ImageList;
	// userid �� �û���ϸ��Ϣ��ӳ��
	map<u_short,USER_ONLINE>m_mpUsers;
	// userid �� �û������е�ӳ��
	map<u_short,int>m_mpId2Index;
	// �û������е� userid��ӳ��
	map<int,u_short>m_mpIndex2Id;
	// ��������ĶԻ���
	vector<CChatDlg*>m_chatDlgs;
	// �û������е�����Ӧ��SOCKET��ӳ��
	map<int,SOCKET>m_mpClients;
	// ��ǰ������
	CClient *m_pClient;
	// �Է�������
	CClient *m_pPeer;
	// ������Ϣ��ӳ��
	map<u_short,vector<string>>m_offlineMessages;
	// ���촰�ڵ�ӳ��
	map<u_short,CChatDlg*>m_friendsChating;
private:
	// �����ӿؼ�
	void CreateChildren();
	// ����������Ϣ
	void RequestOfflineMessage();
	// �����û��б�
	void RequestUserList();
	// �����û��б�
	void DealUsersList(PPACKET_HDR packet);
	// ���������û��б�
	void DealLastestUserList(PPACKET_HDR packet);
	// ����������Ϣ
	void DealLoadOffline(PPACKET_HDR packet);
	// �������߰�
	void DispatchMessages(const PACKET_OFFLINE_MESSAGE_A&);
	// չʾ������Ϣ
	void ShowOffflineMessages();
	// ����������Ϣ
	void DealMessageOnLine();
public:
	// �����͹��������ݰ�
	virtual BOOL RecvPacket(PPACKET_HDR packet );
	// �ͻ������ӵ���
	LRESULT OnClientAccept(WPARAM wParam,LPARAM lParam);
	// ���촰��������
	afx_msg LRESULT OnChatDlgDestory(WPARAM wParam,LPARAM lParam);

};

