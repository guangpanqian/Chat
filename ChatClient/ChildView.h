
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
	// 双击某个用户跟他聊天
	afx_msg void OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult);
	// 离线消息包到了
	LRESULT OnOfflineArrived(WPARAM wParam,LPARAM lParam);
	// 在线消息到了
	LRESULT OnOnlineArrived(WPARAM wParam,LPARAM lParam);
	// SOCKET 无效了
	LRESULT OnSocketInvalid(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	// 所有用户列表
	CListCtrl   m_lstCtlUsers;
	// 在线、离线图标
	CBitmap m_bitmaps[2];
	CImageList m_ImageList;
	// userid 到 用户详细信息的映射
	map<u_short,USER_ONLINE>m_mpUsers;
	// userid 到 用户所在行的映射
	map<u_short,int>m_mpId2Index;
	// 用户所在行到 userid的映射
	map<int,u_short>m_mpIndex2Id;
	// 正在聊天的对话框
	vector<CChatDlg*>m_chatDlgs;
	// 用户所在行到它对应的SOCKET的映射
	map<int,SOCKET>m_mpClients;
	// 当前的连接
	CClient *m_pClient;
	// 对方的连接
	CClient *m_pPeer;
	// 离线消息的映射
	map<u_short,vector<string>>m_offlineMessages;
	// 聊天窗口的映射
	map<u_short,CChatDlg*>m_friendsChating;
private:
	// 创建子控件
	void CreateChildren();
	// 请求离线消息
	void RequestOfflineMessage();
	// 请求用户列表
	void RequestUserList();
	// 处理用户列表
	void DealUsersList(PPACKET_HDR packet);
	// 处理最新用户列表
	void DealLastestUserList(PPACKET_HDR packet);
	// 处理离线消息
	void DealLoadOffline(PPACKET_HDR packet);
	// 处理离线包
	void DispatchMessages(const PACKET_OFFLINE_MESSAGE_A&);
	// 展示离线消息
	void ShowOffflineMessages();
	// 处理在线消息
	void DealMessageOnLine();
public:
	// 处理发送过来的数据包
	virtual BOOL RecvPacket(PPACKET_HDR packet );
	// 客户端连接到了
	LRESULT OnClientAccept(WPARAM wParam,LPARAM lParam);
	// 聊天窗口销毁了
	afx_msg LRESULT OnChatDlgDestory(WPARAM wParam,LPARAM lParam);

};

