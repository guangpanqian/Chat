
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once
#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	const USER_STATUS& GetUserStatus(u_short userId);
	const u_short& GetUserPort(u_short userId) ;
	const string &GetUserIp(u_short userId);
	const map<u_short,UserInfo>&GetUsersMap()const;
	void LogInfo(char* szMessage);
// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar        m_wndStatusBar;
	CChildView    m_wndView;
	int             m_currentConnections;
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LRESULT  OnAccept(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnLogin(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnHeart(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnClientLostTouch(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	// ����������
	void UpdateConnections();
};


