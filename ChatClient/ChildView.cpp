
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChildView.h"
#include "..\include\Package\packetDef.h"
#include "CommonDef.h"
#include "ChatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView(CClient *pClient)
{
	m_pClient = pClient;
	m_pPeer = NULL;

	// 注册需要处理的包ID
	m_pClient->Attach(PACKET_ID_USERS,this);
	m_pClient->Attach(PACKET_ID_STATUS_CHANGE,this);
	m_pClient->Attach (PACKET_ID_LOAD_OFFLINE,this);

}

CChildView::~CChildView()
{
	delete m_pClient;
	m_pClient = NULL;
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, 1000, &CChildView::OnNMDblclkListData)
	ON_MESSAGE(WM_OFFLINE_MESSAGE_ARRIVED,OnOfflineArrived)
	ON_MESSAGE(WM_ONLINE_MESSAGE_ARRIVED,OnOnlineArrived)
	ON_MESSAGE(WM_SOCKET_INVALID,OnSocketInvalid)
	ON_MESSAGE(WM_CHAT_DLG_DESTORY,OnChatDlgDestory)
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

	m_pClient->SetHostWnd(m_hWnd);

	// 创建子窗口
	CreateChildren();

	// 请求用户列表
	RequestUserList();

	// 请求离线消息
	RequestOfflineMessage();

	return 0;
}

void CChildView::CreateChildren()
{
	// 加载在线离线头像
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
	m_lstCtlUsers.InsertColumn(1,"员工编号", LVCFMT_LEFT,100);
	m_lstCtlUsers.InsertColumn(2,"姓名", LVCFMT_LEFT,85);
	m_lstCtlUsers.SetImageList(&m_ImageList,LVSIL_SMALL); 

	// timer 发送心跳包
	SetTimer(NULL,5000,NULL);
}

// 请求用户列表
void CChildView::RequestUserList()
{
	PPACKET_USERS packetUsers = new PACKET_USERS;
	ZeroMemory(packetUsers,sizeof(packetUsers));
	packetUsers->header.type = PACKET_ID_USERS;
	packetUsers->header.len = sizeof(PACKET_USERS);
	m_pClient->AddSendQueue(PPACKET_HDR(packetUsers));
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType,cx,cy);
	CRect rcClient;
	GetClientRect(&rcClient);
	m_lstCtlUsers.MoveWindow(&rcClient);
}
// 发送心跳包
void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	PPACKET_HEART packetHeart = new PACKET_HEART;
	ZeroMemory(packetHeart,sizeof(packetHeart));
	packetHeart->headr.len = sizeof(PACKET_HEART);
	packetHeart->headr.type = PACKET_ID_HEART;
	m_pClient->AddSendQueue(PPACKET_HDR(packetHeart));
}

BOOL CChildView::RecvPacket(PPACKET_HDR packet )
{
	switch(packet->type)
	{
	default:
		break;

	case PACKET_ID_USERS:
		DealUsersList(packet);
		break;

	case PACKET_ID_STATUS_CHANGE:
		DealLastestUserList(packet);
		break;

	case PACKET_ID_LOAD_OFFLINE:
		DealLoadOffline(packet);
		break;

	case PACKET_ID_MESSAGE_ONLINE:
		DealMessageOnLine();
		break;
	}
	return true;
}

void CChildView::DealUsersList(PPACKET_HDR packet)
{
	// 接受用户列表
	PACKET_USERS_A packetUsersA;
	ZeroMemory(&packetUsersA,sizeof(PACKET_USERS_A));
	m_pClient->Recv((char*) &packetUsersA.count,sizeof(u_short));

	packetUsersA.pUsers = new USER_ONLINE[packetUsersA.count];
	m_pClient->Recv((char*) packetUsersA.pUsers,sizeof(USER_ONLINE) * packetUsersA.count);
	int nCounts = packetUsersA.count;
	CString text;
	for (int userIndex = 0;userIndex <nCounts;++userIndex)
	{
		m_lstCtlUsers.InsertItem(userIndex,"");
		m_lstCtlUsers.SetItem(userIndex,0,LVIF_IMAGE,NULL,packetUsersA.pUsers[userIndex].status,0,0,0,0);
		text.Format("%d",packetUsersA.pUsers[userIndex].id);
		m_lstCtlUsers.SetItemText(userIndex,1,text);
		m_lstCtlUsers.SetItemText(userIndex,2,packetUsersA.pUsers[userIndex].userName);

		// userid -> index
		m_mpId2Index.insert(pair<u_short,int>(packetUsersA.pUsers[userIndex].id,userIndex));
		// index -> userid
		m_mpIndex2Id.insert(pair<int,u_short>(userIndex,packetUsersA.pUsers[userIndex].id));

		USER_ONLINE userOnline;
		memcpy(&userOnline,&packetUsersA.pUsers[userIndex],sizeof(userOnline));
		// userid -> userinfo
		m_mpUsers.insert(pair<u_short,USER_ONLINE>(packetUsersA.pUsers[userIndex].id,userOnline));
	}
	delete packetUsersA.pUsers;

}

void CChildView::DealLastestUserList(PPACKET_HDR packet)
{
	// 用户最新信息
	PACKET_USERS_A packetUsersA;
	ZeroMemory(&packetUsersA,sizeof(PACKET_USERS_A));
	m_pClient->Recv((char*) &packetUsersA.count,sizeof(u_short));

	packetUsersA.pUsers = new USER_ONLINE[packetUsersA.count];
	m_pClient->Recv((char*) packetUsersA.pUsers,sizeof(USER_ONLINE) * packetUsersA.count);

	int nCounts = packetUsersA.count;
	CString text;

	for (int packetUserIndex = 0;packetUserIndex <nCounts;++packetUserIndex)
	{
		u_short& userId  = packetUsersA.pUsers[packetUserIndex].id;
		USER_ONLINE &userInfo = m_mpUsers[userId];

		// 如果该用户的在线状态发生了改变
		if (userInfo.status !=  packetUsersA.pUsers[packetUserIndex].status)
		{
			// 更新该用户的在线与否状态
			int userIndex = m_mpId2Index[userId];
			m_lstCtlUsers.SetItem(userIndex,0,LVIF_IMAGE,NULL,packetUsersA.pUsers[packetUserIndex].status,0,0,0,0);
		
			// 查看与该用户聊天的窗口是否已经打开
			map<u_short,CChatDlg*>::iterator pos = m_friendsChating.find(userId);
			CChatDlg *pClient = NULL;

			if (pos != m_friendsChating.end())
				pClient = pos->second;

			if (NULL!= pClient)
				pClient->ChangeClient();

			// 更新该用户信息
			memcpy(&userInfo,&packetUsersA.pUsers[packetUserIndex],sizeof(USER_ONLINE));

			// 检查是否需要更改聊天窗口title
			if (NULL!= pClient)
				pClient->SetTitle();
		}

	}
	delete packetUsersA.pUsers;
}

void CChildView::DealLoadOffline(PPACKET_HDR packet)
{
	// 登陆成功后接收了离线消息，不需再接收了
	m_pClient->Detach(PACKET_ID_LOAD_OFFLINE);

	PACKET_OFFLINE_MESSAGE_A packetOfflineMessage;
	ZeroMemory(&packetOfflineMessage,sizeof(PACKET_OFFLINE_MESSAGE_A));
	m_pClient->Recv((char*)&packetOfflineMessage.counts,sizeof(u_short));
	packetOfflineMessage.messages =  new PACKET_USER_MESSAGE[packetOfflineMessage.counts];
	m_pClient->Recv((char*)packetOfflineMessage.messages,packetOfflineMessage.counts*sizeof(PACKET_USER_MESSAGE));

	DispatchMessages(packetOfflineMessage);
	SendMessage(WM_OFFLINE_MESSAGE_ARRIVED,WPARAM(0),LPARAM(0));

	delete packetOfflineMessage.messages;
}

LRESULT CChildView::OnOnlineArrived(WPARAM wParam,LPARAM lParam)
{
	// 可能是来自服务器也可能是来自用户，先取消关注该消息，如果是其他用户在聊天时再附加上
	m_pPeer->Detach(PACKET_ID_MESSAGE_ONLINE);

	PACKET_MESSAGE packetOnline;
	m_pPeer->Recv((char*)&packetOnline.userMessage,sizeof(packetOnline.userMessage));

	USER_ONLINE &userInfo = m_mpUsers[packetOnline.userMessage.senderId];
	CString strName(m_mpUsers[theApp.id].userName);
	CString strMessage(packetOnline.userMessage.message);

	map<u_short,CChatDlg*>::iterator pos = m_friendsChating.find(userInfo.id);
	// 并未与该用户进行聊天，需要弹出一个聊天窗口
	if ( pos == m_friendsChating.end())
	{
		CChatDlg  *pDlg = new CChatDlg(this,m_pPeer,theApp.id,strName,&userInfo,strMessage);
		pDlg->Create(CChatDlg::IDD,this);
		pDlg->ShowWindow(SW_SHOW);
		m_friendsChating.insert(pair<u_short,CChatDlg*>(userInfo.id,pDlg));
	}
	else
	{
		// 更新聊天内容，并可能调整聊天对象（服务器（离线）， 该用户（在线））
		pos->second->AppendMessage(strMessage);
		pos->second->ChangeClient(m_pPeer);
	}
	return 0;
}


LRESULT CChildView::OnSocketInvalid(WPARAM wParam,LPARAM lParam)
{
	// 这个窗口收到这个消息，就是服务器连接异常了
	PostQuitMessage(0);
	return 0;
}

void CChildView::DealMessageOnLine()
{
	SendMessage(WM_ONLINE_MESSAGE_ARRIVED,0,0);
}

void CChildView::DispatchMessages(const PACKET_OFFLINE_MESSAGE_A& packetOfflineMessage)
{
	for (int messageIndex = 0;messageIndex < packetOfflineMessage.counts;++messageIndex)
	{
		map<u_short,vector<string>>::iterator pos =  m_offlineMessages.find(packetOfflineMessage.messages[messageIndex].senderId);
		if (pos == m_offlineMessages.end())
		{
			vector<string>messageParagraph;
			messageParagraph.push_back(packetOfflineMessage.messages[messageIndex].message);
			m_offlineMessages.insert(pair<u_short,vector<string>>(packetOfflineMessage.messages[messageIndex].senderId,messageParagraph));
		}
		else
		{
			// 他可能发了很多话，贴在后面
			pos->second.push_back(packetOfflineMessage.messages[messageIndex].message);
		}
	}
}

void MergeMessages(vector<string>&paragraph,CString& szMessage)
{
	vector<string>::iterator first = paragraph.begin();
	vector<string>::iterator last = paragraph.end();
	for(;first!=last;++first)
	{
		szMessage = szMessage + (*first).c_str();
	}
}

// 展示离线消息
void CChildView::ShowOffflineMessages()
{
	map<u_short,vector<string>>::iterator first = m_offlineMessages.begin();
	map<u_short,vector<string>>::iterator last = m_offlineMessages.end();
	for (;first!=last;++first)
	{
		USER_ONLINE &userInfo = m_mpUsers[(*first).first];
		CString szMessage = "";
		MergeMessages(first->second,szMessage);
		CClient *pClient = NULL;

		// 如果这个用户是离线的，那么接收的是离线消息，面对的好友是服务器
		if (OFFLINE == userInfo.status)
		{
			pClient = m_pClient;
		}

		CString strName(m_mpUsers[theApp.id].userName);
		CChatDlg  *pDlg = new CChatDlg(this,pClient,theApp.id,strName,&userInfo,szMessage);
		pDlg->Create(CChatDlg::IDD,this);
		pDlg->ShowWindow(SW_SHOW);

		// 记录下跟这位好友正在聊天
		m_friendsChating.insert(pair<u_short,CChatDlg*>(userInfo.id,pDlg));	
	}
}

void CChildView::OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (pNMItemActivate->hdr.code != NM_DBLCLK)
	{
		return;
	}

	USER_ONLINE &userInfo =  m_mpUsers[m_mpIndex2Id[pNMItemActivate->iItem]];
	CString strName(m_mpUsers[theApp.id].userName);

	if (userInfo.id == theApp.id)
	{
		MessageBox("不能跟自己聊天！","温馨提示",0);
		return;
	}

	// 如果跟这个用户还没打开聊天窗口
	if (m_friendsChating.find(userInfo.id) == m_friendsChating.end())
	{
		CClient *pClient = NULL;
		// 如果这个用户是离线的，那么接收的是离线消息，面对的好友是服务器
		if (OFFLINE ==  userInfo.status)
		{
			pClient = m_pClient;
		}

		CChatDlg  *pDlg = new CChatDlg(this,pClient,theApp.id,strName,&userInfo);
		pDlg->Create(CChatDlg::IDD,this);
		pDlg->ShowWindow(SW_SHOW);

		m_friendsChating.insert(pair<u_short,CChatDlg*>(userInfo.id,pDlg));
	}
}


void CChildView::RequestOfflineMessage()
{
	PPACKET_OFFLINE_MESSAGE pPacket = new PACKET_OFFLINE_MESSAGE;
	pPacket->header.type = PACKET_ID_LOAD_OFFLINE;
	pPacket->header.len = sizeof(PACKET_OFFLINE_MESSAGE);
	pPacket->id = theApp.id;
	m_pClient->AddSendQueue (PPACKET_HDR(pPacket));
}

LRESULT CChildView::OnOfflineArrived(WPARAM wParam,LPARAM lParam)
{
	ShowOffflineMessages();
	return 0;
}

LRESULT CChildView::OnClientAccept(WPARAM wParam,LPARAM lParam)
{
	m_pPeer = (CClient*)wParam;
	m_pPeer->Attach(PACKET_ID_MESSAGE_ONLINE,this);

	return 0;
}

LRESULT CChildView::OnChatDlgDestory(WPARAM wParam,LPARAM lParam)
{
	// 与某个好友的聊天窗口关闭
	u_short id = u_short(wParam);
	map<u_short,CChatDlg*>::iterator pos = m_friendsChating.find(id);
	if (pos != m_friendsChating.end())
			m_friendsChating.erase(pos);

	return 0;
}
