
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "ChatServer.h"
#include "ChildView.h"
#include <string>
#include "..\include\Package\packetDef.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView
CChildView::CChildView()
{

}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE,1002,OnSelChangeTab)
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码
	
	// 不要为绘制消息而调用 CWnd::OnPaint()
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 创建子窗口
	CreateChildren();

	// 加载用户列表
	GetEmployee();

	// 时钟检查心跳
	SetTimer(NULL,2*1000,NULL);

	return 0;
}

void CChildView::CreateChildren()
{
	m_bitmaps[ONLINE].LoadBitmap(IDB_BITMAP_ONLINE);
	m_bitmaps[OFFLINE].LoadBitmap(IDB_BITMAP_OFFLINE);

	m_ImageList.Create(16, 16,ILC_COLOR24|ILC_MASK, 4, 0);
	
	for (int i = 0 ; i < 2 ; i ++)
	{
		m_ImageList.Add(&m_bitmaps[i], RGB(0, 0, 0));
	}

	m_lstCtlUsers.Create(WS_VISIBLE|LVS_REPORT,CRect(0,0,0,0),this,1000);
	m_lstCtlUsers.SetExtendedStyle(m_lstCtlUsers.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES );
	m_lstCtlUsers.InsertColumn(0,"状态", LVCFMT_LEFT,36);
	m_lstCtlUsers.InsertColumn(1,"员工编号", LVCFMT_LEFT,70);
	m_lstCtlUsers.InsertColumn(2,"姓名", LVCFMT_LEFT,60);
	m_lstCtlUsers.InsertColumn(3,"IP地址", LVCFMT_LEFT,150);
	m_lstCtlUsers.InsertColumn(4,"端口号", LVCFMT_LEFT,100);
	m_lstCtlUsers.InsertColumn(5,"登录时间", LVCFMT_LEFT,160);
	m_lstCtlUsers.InsertColumn(6,"心跳时间", LVCFMT_LEFT,160);
	m_lstCtlUsers.SetImageList(&m_ImageList,LVSIL_SMALL); 

	m_lstCtlLog.Create(WS_VISIBLE|LVS_REPORT,CRect(0,0,0,0),this,1001);
	m_lstCtlLog.SetExtendedStyle(m_lstCtlLog.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );
	m_lstCtlLog.InsertColumn(0,"时间",LVCFMT_LEFT,160);
	m_lstCtlLog.InsertColumn(1,"事件",LVCFMT_LEFT,1400);
	m_lstCtlLog.ShowWindow(SW_HIDE);

	m_font.CreateFont(12,0,0,0,300,0,0,0,1,0,0,0,0,_T("宋体")); 
	m_tabCtl.Create(TCS_TABS|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,1002);
	m_tabCtl.InsertItem(0,"人员列表");
	m_tabCtl.InsertItem(1,"日志信息");
	m_tabCtl.SetFont(&m_font);

	m_tabCtl.SetCurSel(0);

}

void CChildView::GetEmployee()
{
	vector<USER_BASE_INFO>users;
	theApp.pEmployeeDBOperator->GetEmployees(users);
	size_t rowCounts = users.size();
	CString szText;
	for (int rowIndex = 0;rowIndex < rowCounts;++rowIndex)
	{
		m_lstCtlUsers.InsertItem(rowIndex,"");
		m_employee2ListIndex.insert(pair<u_short,int>(users[rowIndex].userId,rowIndex));
		m_lstCtlUsers.SetItem(rowIndex,0,LVIF_IMAGE,NULL,OFFLINE,0,0,0,0);

		szText.Format("%d",users[rowIndex].userId);
		m_lstCtlUsers.SetItemText(rowIndex,1,szText);
		m_lstCtlUsers.SetItemText(rowIndex,2,users[rowIndex].szUserName);

		UserInfo userInfo;
		userInfo.m_id = users[rowIndex].userId;
		strcpy_s(userInfo.m_name,users[rowIndex].szUserName);

		m_employees.insert(pair<u_short,UserInfo>(users[rowIndex].userId,userInfo));
	}
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType,cx,cy);
	
	m_lstCtlUsers.MoveWindow(0,20,cx,cy);
	m_lstCtlLog.MoveWindow(0,20,cx,cy);
	m_tabCtl.MoveWindow(0,0,cx,cy);
}

void CChildView::OnSelChangeTab(NMHDR *pNMHDR,LRESULT *pResult)
{
	//获取所选择的页面
	int nCurSel = m_tabCtl.GetCurSel();
	if(nCurSel == 0)
	{	
		m_lstCtlUsers.ShowWindow(SW_SHOW);
		m_lstCtlLog.ShowWindow(SW_HIDE);
	}
	else if(nCurSel == 1)
	{	
		m_lstCtlUsers.ShowWindow(SW_HIDE);
		m_lstCtlLog.ShowWindow(SW_SHOW);
	}
	*pResult = 0;
}

LRESULT  CChildView::OnAccept(WPARAM wParam,LPARAM lParam)
{
	PSOCKADDR_IN pClientAddress = PSOCKADDR_IN(wParam);
	char szLog[64];
	sprintf_s(szLog,"IP:%s,Port:%d is connecting....",inet_ntoa(pClientAddress->sin_addr),ntohs(pClientAddress->sin_port));
	LogInfo(szLog);
	return 0;
}

LRESULT CChildView::OnLogin(WPARAM wParam,LPARAM lParam)
{
	SOCKET socket = SOCKET(lParam);
	u_short port = LOWORD(wParam);
	u_short userId = HIWORD(wParam);

	char szLog[64];
	sprintf_s(szLog,"%d is logining....",userId);
	LogInfo(szLog);

	// SOCKET 无效返回
	if (INVALID_SOCKET == socket)
	{
		return 0;
	}

	// socket 到用户ID的映射
	m_mpSocket2Id.insert(pair<SOCKET,u_short>(socket,userId));

	// 用户列中没这个用户，返回
	map<u_short,UserInfo>::iterator posUserInfo = m_employees.find(userId);
	if (m_employees.end() == posUserInfo)
	{
		return 0;
	}

	// 登陆成功了，更新这个用户的状态为在线
	posUserInfo->second.m_port = port;
	posUserInfo->second.m_ip     = inet_ntoa(theApp.socketServer.GetClientList().GetAddress(socket).sin_addr);
	posUserInfo->second.m_status = ONLINE;

	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	posUserInfo->second.m_loginTime = systemTime;
	posUserInfo->second.m_heartTime = systemTime;
	posUserInfo->second.m_socket = socket;

	// 查找该用户在界面中所在的行
	map<u_short,int>::iterator posIndex = m_employee2ListIndex.find(userId);
	if (m_employee2ListIndex.end() == posIndex)
	{
		return 0;
	}

	int nItem = posIndex->second;
	CString szText;

	// 更新下该用户的信息
	m_lstCtlUsers.SetItem(nItem,0,LVIF_IMAGE,NULL,ONLINE,0,0,0,0);
	szText = posUserInfo->second.m_ip.c_str();
	m_lstCtlUsers.SetItemText(nItem,3,szText);
	szText.Format("%d",ntohs(posUserInfo->second.m_port));
	m_lstCtlUsers.SetItemText(nItem,4,szText);
	szText.Format("%02d:%02d:%02d.%d",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds);
	m_lstCtlUsers.SetItemText(nItem,5,szText);
	szText.Format("%02d:%02d:%02d.%d",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds);
	m_lstCtlUsers.SetItemText(nItem,6,szText);

	return 0;
}

LRESULT CChildView::OnHeart(WPARAM wParam,LPARAM lParam)
{
	SOCKET socket = (SOCKET)wParam;

	map<SOCKET,u_short>::iterator posS2Id = m_mpSocket2Id.find(socket);
	if (m_mpSocket2Id.end() == posS2Id)
		return 0;

	u_short userId = posS2Id->second;

	map<u_short,int>::iterator posId2Index = m_employee2ListIndex.find(userId);
	if (m_employee2ListIndex.end() == posId2Index)
		return 0;

	int nItem = posId2Index->second;

	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	map<u_short,UserInfo>::iterator posUserInfo = m_employees.find(userId);
	if (m_employees.end() == posUserInfo)
		return 0;

	// 更新心跳时间
	posUserInfo->second.m_heartTime = systemTime;
	CString szText;
	szText.Format("%02d:%02d:%02d.%d",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds);
	m_lstCtlUsers.SetItemText(nItem,6,szText);

	return 0;
}

void CChildView::LogInfo(char* szMessage)
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	char szTime[256];
	sprintf_s(szTime,"%04d-%02d-%02d %02d:%02d:%02d.%d",
		systemTime.wYear,
		systemTime.wMonth,
		systemTime.wDay,
		systemTime.wHour,
		systemTime.wMinute,
		systemTime.wSecond,
		systemTime.wMilliseconds
		);

   int nRowCounts = m_lstCtlLog.GetItemCount();
   m_lstCtlLog.InsertItem(nRowCounts,"");
   m_lstCtlLog.SetItemText(nRowCounts,0,szTime);
   m_lstCtlLog.SetItemText(nRowCounts,1,szMessage);

}

const USER_STATUS& CChildView::GetUserStatus(u_short userId)
{
	ASSERT(m_employees.end() != m_employees.find(userId));
	return m_employees[userId].m_status;
}

const u_short& CChildView::GetUserPort(u_short userId) 
{
	ASSERT(m_employees.end() != m_employees.find(userId));
	return m_employees[userId].m_port;
}

const string &CChildView::GetUserIp(u_short userId)
{
	ASSERT(m_employees.end() != m_employees.find(userId));
	return m_employees[userId].m_ip;
}

const map<u_short,UserInfo>& CChildView::GetUsersMap() const
{
	return m_employees;
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	map<u_short,UserInfo>::iterator first = m_employees.begin();
	map<u_short,UserInfo>::iterator last = m_employees.end();
	
	for (;first!=last;++first)
	{
		u_short id = (*first).first;
		UserInfo &userInfo = (*first).second;

		if (userInfo.m_socket ==  INVALID_SOCKET)
			continue;

		COleDateTime dTimeS(userInfo.m_heartTime); 
		COleDateTime dTimeF(systemTime); 
		COleDateTimeSpan dTimeSpan = dTimeF - dTimeS; 
		int nSecnonSpan = int (dTimeSpan.GetTotalSeconds());

		if  (nSecnonSpan > CLIENT_HEART_MAX_SECONDS)
		{
			SetOffLine(id);
			RetSetMap(userInfo.m_socket);
			userInfo.ReSet();
		}
	}
}

LRESULT CChildView::OnClientLostTouch(WPARAM wParam,LPARAM lParam)
{
	SOCKET socket = (SOCKET)wParam;

	map<SOCKET,u_short>::iterator posId = m_mpSocket2Id.find(socket);
	if (m_mpSocket2Id.end() == posId)
		return 0;

	u_short userId = posId->second;

	map<u_short,UserInfo>::iterator posUserInfo = m_employees.find(userId);
	if (m_employees.end() == posUserInfo)
		return 0;

	UserInfo &userInfo = posUserInfo->second;

	SetOffLine(userId);
	RetSetMap(socket);
	userInfo.ReSet();

	return 0;
}

void CChildView::SetOffLine(u_short userId)
{
	map<u_short,int>::iterator posIndex = m_employee2ListIndex.find(userId);
	if (m_employee2ListIndex.end() == posIndex)
		return;

	int nItem = posIndex->second;
	m_lstCtlUsers.SetItem(nItem,0,LVIF_IMAGE,NULL,OFFLINE,0,0,0,0);
	m_lstCtlUsers.SetItemText(nItem,3,"");
	m_lstCtlUsers.SetItemText(nItem,4,"");
	m_lstCtlUsers.SetItemText(nItem,5,"");
	m_lstCtlUsers.SetItemText(nItem,6,"");
}

void CChildView::RetSetMap(SOCKET socket)
{
	map<SOCKET,u_short>::iterator pos = m_mpSocket2Id.find(socket);
	if (pos != m_mpSocket2Id.end())
	{
		m_mpSocket2Id.erase(pos);
	}
}