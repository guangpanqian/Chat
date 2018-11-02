
// MainFrm.cpp : CMainFrame 类的实现
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
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	m_currentConnections = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 创建一个视图以占用框架的工作区
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame 诊断

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


// CMainFrame 消息处理程序

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 将焦点前移到视图窗口
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 让视图第一次尝试该命令
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
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
	szCurrentSituations.Format("当前连接数:[%d]",m_currentConnections);
	m_wndStatusBar.SetPaneText(ID_SEPARATOR,szCurrentSituations,TRUE);
}
