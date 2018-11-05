
// ChildView.cpp : CChildView ���ʵ��
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

	// ע����Ҫ����İ�ID
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

// CChildView ��Ϣ�������

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
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	// TODO: �ڴ˴������Ϣ����������
	// ��ҪΪ������Ϣ������ CWnd::OnPaint()
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pClient->SetHostWnd(m_hWnd);

	// �����Ӵ���
	CreateChildren();

	// �����û��б�
	RequestUserList();

	// ����������Ϣ
	RequestOfflineMessage();

	return 0;
}

void CChildView::CreateChildren()
{
	// ������������ͷ��
	m_bitmaps[ONLINE].LoadBitmap(IDB_BITMAP_ONLINE);
	m_bitmaps[OFFLINE].LoadBitmap(IDB_BITMAP_OFFLINE);

	m_ImageList.Create(16, 16,ILC_COLOR24|ILC_MASK, 4, 0);

	for (int i = 0 ; i < 2 ; i ++)
	{
		m_ImageList.Add(&m_bitmaps[i], RGB(0, 0, 0));
	}

	m_lstCtlUsers.Create(WS_VISIBLE|LVS_REPORT,CRect(0,0,0,0),this,1000);
	m_lstCtlUsers.SetExtendedStyle(m_lstCtlUsers.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES );
	m_lstCtlUsers.InsertColumn(0,"״̬", LVCFMT_LEFT,36);
	m_lstCtlUsers.InsertColumn(1,"Ա�����", LVCFMT_LEFT,100);
	m_lstCtlUsers.InsertColumn(2,"����", LVCFMT_LEFT,85);
	m_lstCtlUsers.SetImageList(&m_ImageList,LVSIL_SMALL); 

	// timer ����������
	SetTimer(NULL,5000,NULL);
}

// �����û��б�
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
// ����������
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
	// �����û��б�
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
	// �û�������Ϣ
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

		// ������û�������״̬�����˸ı�
		if (userInfo.status !=  packetUsersA.pUsers[packetUserIndex].status)
		{
			// ���¸��û����������״̬
			int userIndex = m_mpId2Index[userId];
			m_lstCtlUsers.SetItem(userIndex,0,LVIF_IMAGE,NULL,packetUsersA.pUsers[packetUserIndex].status,0,0,0,0);
		
			// �鿴����û�����Ĵ����Ƿ��Ѿ���
			map<u_short,CChatDlg*>::iterator pos = m_friendsChating.find(userId);
			CChatDlg *pClient = NULL;

			if (pos != m_friendsChating.end())
				pClient = pos->second;

			if (NULL!= pClient)
				pClient->ChangeClient();

			// ���¸��û���Ϣ
			memcpy(&userInfo,&packetUsersA.pUsers[packetUserIndex],sizeof(USER_ONLINE));

			// ����Ƿ���Ҫ�������촰��title
			if (NULL!= pClient)
				pClient->SetTitle();
		}

	}
	delete packetUsersA.pUsers;
}

void CChildView::DealLoadOffline(PPACKET_HDR packet)
{
	// ��½�ɹ��������������Ϣ�������ٽ�����
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
	// ���������Է�����Ҳ�����������û�����ȡ����ע����Ϣ������������û�������ʱ�ٸ�����
	m_pPeer->Detach(PACKET_ID_MESSAGE_ONLINE);

	PACKET_MESSAGE packetOnline;
	m_pPeer->Recv((char*)&packetOnline.userMessage,sizeof(packetOnline.userMessage));

	USER_ONLINE &userInfo = m_mpUsers[packetOnline.userMessage.senderId];
	CString strName(m_mpUsers[theApp.id].userName);
	CString strMessage(packetOnline.userMessage.message);

	map<u_short,CChatDlg*>::iterator pos = m_friendsChating.find(userInfo.id);
	// ��δ����û��������죬��Ҫ����һ�����촰��
	if ( pos == m_friendsChating.end())
	{
		CChatDlg  *pDlg = new CChatDlg(this,m_pPeer,theApp.id,strName,&userInfo,strMessage);
		pDlg->Create(CChatDlg::IDD,this);
		pDlg->ShowWindow(SW_SHOW);
		m_friendsChating.insert(pair<u_short,CChatDlg*>(userInfo.id,pDlg));
	}
	else
	{
		// �����������ݣ������ܵ���������󣨷����������ߣ��� ���û������ߣ���
		pos->second->AppendMessage(strMessage);
		pos->second->ChangeClient(m_pPeer);
	}
	return 0;
}


LRESULT CChildView::OnSocketInvalid(WPARAM wParam,LPARAM lParam)
{
	// ��������յ������Ϣ�����Ƿ����������쳣��
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
			// �����ܷ��˺ܶ໰�����ں���
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

// չʾ������Ϣ
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

		// �������û������ߵģ���ô���յ���������Ϣ����Եĺ����Ƿ�����
		if (OFFLINE == userInfo.status)
		{
			pClient = m_pClient;
		}

		CString strName(m_mpUsers[theApp.id].userName);
		CChatDlg  *pDlg = new CChatDlg(this,pClient,theApp.id,strName,&userInfo,szMessage);
		pDlg->Create(CChatDlg::IDD,this);
		pDlg->ShowWindow(SW_SHOW);

		// ��¼�¸���λ������������
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
		MessageBox("���ܸ��Լ����죡","��ܰ��ʾ",0);
		return;
	}

	// ���������û���û�����촰��
	if (m_friendsChating.find(userInfo.id) == m_friendsChating.end())
	{
		CClient *pClient = NULL;
		// �������û������ߵģ���ô���յ���������Ϣ����Եĺ����Ƿ�����
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
	// ��ĳ�����ѵ����촰�ڹر�
	u_short id = u_short(wParam);
	map<u_short,CChatDlg*>::iterator pos = m_friendsChating.find(id);
	if (pos != m_friendsChating.end())
			m_friendsChating.erase(pos);

	return 0;
}
