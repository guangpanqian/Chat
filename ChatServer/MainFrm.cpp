
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "ChatServer.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_ACCEPT,OnAccept)
	ON_MESSAGE(WM_LOGIN,OnLogin)
	ON_MESSAGE(WM_HEART,OnHeart)
	ON_MESSAGE(WM_CLIENT_LOST_TOUCH,OnClientLostTouch)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	m_currentConnections = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ����һ����ͼ��ռ�ÿ�ܵĹ�����
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("δ�ܴ�����ͼ����\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ������ǰ�Ƶ���ͼ����
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ����ͼ��һ�γ��Ը�����
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ִ��Ĭ�ϴ���
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LRESULT  CMainFrame::OnAccept(WPARAM wParam,LPARAM lParam)
{
	++ m_currentConnections;
	UpdateConnections();
	return m_wndView.OnAccept(wParam,lParam);
}

LRESULT CMainFrame::OnLogin(WPARAM wParam,LPARAM lParam)
{
	return m_wndView.OnLogin(wParam,lParam);
}

const USER_STATUS& CMainFrame::GetUserStatus(u_short userId)
{
	return m_wndView.GetUserStatus(userId);
}

const u_short& CMainFrame::GetUserPort(u_short userId) 
{
	return m_wndView.GetUserPort(userId);
}

const string &CMainFrame::GetUserIp(u_short userId)
{
	return m_wndView.GetUserIp(userId);
}

const map<u_short,UserInfo>& CMainFrame::GetUsersMap()const
{
	return m_wndView.GetUsersMap();
}

LRESULT CMainFrame::OnHeart(WPARAM wParam,LPARAM lParam)
{
	return m_wndView.OnHeart(wParam,lParam);
}

void CMainFrame::LogInfo(char* szMessage)
{
	m_wndView.LogInfo(szMessage);
}

LRESULT CMainFrame::OnClientLostTouch(WPARAM wParam,LPARAM lParam)
{
	--m_currentConnections;
	UpdateConnections();
	return m_wndView.OnClientLostTouch(wParam,lParam);
}

void CMainFrame::UpdateConnections()
{
	CString szCurrentSituations;
	szCurrentSituations.Format("��ǰ������:[%d]",m_currentConnections);
	m_wndStatusBar.SetPaneText(ID_SEPARATOR,szCurrentSituations,TRUE);
}
