
// ChildView.h : CChildView 类的接口
//


#pragma once


// CChildView 窗口

#include <map>
class CClient;
#include "..\include\Package\packetDef.h"
#include "Interfaces.h"
#include "Client.h"
using namespace  std;

class CChatDlg;
class CChildView : public CWnd,public ISocketObserver
{
// 构造
public:
	CChildView(CClient *pClient);

// 特性
public:

// 操作
public:

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnOfflineArrived(WPARAM wParam,LPARAM lParam);
	LRESULT OnOnlineArrived(WPARAM wParam,LPARAM lParam);
	LRESULT OnSocketInvalid(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CListCtrl   m_lstCtlUsers;
	CBitmap m_bitmaps[2];
	CImageList m_ImageList;
	map<u_short,USER_ONLINE>m_mpUsers;
	map<u_short,int>m_mpId2Index;
	map<int,u_short>m_mpIndex2Id;
	vector<CChatDlg*>m_chatDlgs;
	map<int,SOCKET>m_mpClients;
	CClient *m_pClient;
	CClient *m_pPeer;
	map<u_short,vector<string>>m_offlineMessages;
	map<u_short,CChatDlg*>m_friendsChating;
private:
	void CreateChildren();
	void RequestOfflineMessage();
	void RequestUserList();
	void DealUsersList(PPACKET_HDR packet);
	void DealLastestUserList(PPACKET_HDR packet);
	void DealLoadOffline(PPACKET_HDR packet);
	void DispatchMessages(const PACKET_OFFLINE_MESSAGE_A&);
	void ShowOffflineMessages();
	void Connect2Client(int index,USER_ONLINE& userInfo,CString strMessage= "");
	void DealMessageOnLine();
public:
	virtual BOOL RecvPacket(PPACKET_HDR packet );
	LRESULT OnClientAccept(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnChatDlgDestory(WPARAM wParam,LPARAM lParam);

};

