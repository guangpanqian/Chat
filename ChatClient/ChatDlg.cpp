// ChatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include "CommonDef.h"
#include "ChatLogDealer.h"

// CChatDlg 对话框
extern CChatClientApp theApp;

IMPLEMENT_DYNAMIC(CChatDlg, CDialog)

CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/,CClient *pClinet,u_short id,CString strName,USER_ONLINE *pUserOnline,CString strMessage)
	: CDialog(CChatDlg::IDD, pParent)
{
	m_pClient = pClinet;
	m_strName = strName;
	m_pUserOnline = pUserOnline;
	m_strNow = _T("");
	m_id = id;
	m_strRecord = strMessage;

	if (NULL != m_pClient)
	{
		// 在线消息发送给这个窗口
		m_pClient->Attach(PACKET_ID_MESSAGE_ONLINE,this);
	}

	m_pChatLogDealer = new ChatLogDealer(id,pUserOnline->id);
	m_pAllDlg = NULL;
}

void CChatDlg::ChangeClient(CClient *pClinet)
{
	// 如果与这个窗口关联的不是服务器，先销毁之
	if (theApp.server.pServer != m_pClient)
	{
		delete m_pClient;
	}
	// 设置新的客户端
	m_pClient = pClinet;

	// 如果更新的不是服务器，接受在线消息
	if (NULL != m_pClient && m_pClient!= theApp.server.pServer)
		m_pClient->Attach(PACKET_ID_MESSAGE_ONLINE,this);
}

CChatDlg::~CChatDlg()
{
	// 不要再通知这个窗口了
	if (NULL !=m_pClient)
	{
		m_pClient->Detach(PACKET_ID_MESSAGE_ONLINE);
	}

	// 存储聊天记录
	m_pChatLogDealer->WriteChatLog(m_strRecord);

	delete m_pChatLogDealer;
	m_pChatLogDealer = NULL;

	delete m_pAllDlg;
	m_pAllDlg = NULL;

}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NOW, m_strNow);
	DDX_Text(pDX, IDC_EDIT_RECORD, m_strRecord);
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CChatDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,&CChatDlg::OnBnClickedOnCancel)
	ON_MESSAGE(WM_ONLINE_MESSAGE,OnMessage)
	ON_MESSAGE(WM_SOCKET_INVALID,OnSocketInvalid)
	ON_BN_CLICKED(IDC_BUTTON_LOG, &CChatDlg::OnBnClickedButtonLog)
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CChatDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()

// CChatDlg 消息处理程序
void CChatDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_strNow.IsEmpty())
	{
		MessageBox("不能发送空消息","温馨提示",0);
		return;
	}
	
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	CString szText;
	szText.Format("%s  %02d:%02d:%02d\r\n  %s\r\n",m_strName,systemTime.wHour,systemTime.wMinute,systemTime.wSecond,m_strNow);
	Send2Peer(szText);

	m_strRecord = m_strRecord + szText;

	m_strNow = "";
	UpdateData(FALSE);
}

void CChatDlg::AppendMessage(CString message)
{
	m_strRecord = m_strRecord + message;
	SendMessage(WM_ONLINE_MESSAGE,0,0);
}

BOOL CChatDlg::OnInitDialog()
{
	SetTitle();
	
	if (m_pClient&&m_pClient != theApp.server.pServer)
		m_pClient->SetHostWnd(m_hWnd);

	m_pChatLogDealer->ReadChatLog(m_strRecord);

	UpdateData(FALSE);

	m_pAllDlg = new CAllLogDlg(this);
	m_pAllDlg->Create(CAllLogDlg::IDD,this);
	m_pAllDlg->ShowWindow(SW_HIDE);

	return TRUE;
}

void CChatDlg::SetTitle()
{
	CString szTitle;
	if (ONLINE == m_pUserOnline->status)
	{
		szTitle.Format("与%s聊天中",m_pUserOnline->userName);
	}
	else
	{
		szTitle.Format("%s不在线哦",m_pUserOnline->userName);
	}
	SetWindowText(szTitle);
}

void CChatDlg::ChangeClient()
{
	// 之前是在线状态，现在离线了，那么面向的连接是连接服务器
	if (ONLINE == m_pUserOnline->status)
	{
		m_pClient = theApp.server.pServer;
	}
	// 否则是面向的一个用户
	else
	{
		m_pClient = NULL;
	}
}

void CChatDlg::Send2Peer(const CString& szText)
{
	PPACKET_MESSAGE pPacket = new PACKET_MESSAGE;
	pPacket->headr.type =OFFLINE ==  m_pUserOnline->status? PACKET_ID_MESSAGE_OFFLINE:PACKET_ID_MESSAGE_ONLINE;
	pPacket->headr.len = sizeof(PACKET_MESSAGE);
	pPacket->userMessage.senderId = theApp.id;
	pPacket->userMessage.peerId = m_pUserOnline->id;
	strcpy_s(pPacket->userMessage.message,szText.GetString());
	if (NULL == m_pClient)
	{
		Connect2Client();
	}
	m_pClient->AddSendQueue((PPACKET_HDR)pPacket);
}

BOOL CChatDlg::RecvPacket(PPACKET_HDR packet )
{
	PACKET_MESSAGE packetWords;
	if (FALSE ==  m_pClient->Recv((char*)(&packetWords.userMessage),sizeof(PACKET_USER_MESSAGE)))
	{
		return FALSE;
	}

	CString szText;
	szText = packetWords.userMessage.message;
	m_strRecord = m_strRecord + szText;

	SendMessage(WM_ONLINE_MESSAGE,0,0);

	return TRUE;
}

LRESULT CChatDlg::OnMessage(WPARAM,LPARAM)
{
	UpdateData(FALSE);
	return 0;
}

void CChatDlg::OnBnClickedOnCancel()
{
	CDialog::OnCancel();

	::SendMessage(m_pParentWnd->GetSafeHwnd(),WM_CHAT_DLG_DESTORY,m_pUserOnline->id,0);

	if (m_pClient&&m_pClient != theApp.server.pServer)
	{
		shutdown(m_pClient->GetSocket(),2);	
	}
	delete this;
}

LRESULT CChatDlg::OnSocketInvalid(WPARAM wParam,LPARAM lParam)
{
	// 套接字无效了
	ChangeClient(NULL);
	return 0;
}

void CChatDlg::Connect2Client()
{
	SOCKET socketClinet = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == socketClinet)
	{
		MessageBox("创建客户端SOCKET失败","错误",0);
		return;
	}

	SOCKADDR_IN AddressPeer;
	AddressPeer.sin_family = AF_INET;

	AddressPeer.sin_port = m_pUserOnline->port;
	AddressPeer.sin_addr.s_addr = inet_addr(m_pUserOnline->ip);

	if (SOCKET_ERROR  == connect(socketClinet,PSOCKADDR(&AddressPeer),sizeof(SOCKADDR_IN)))
	{
		MessageBox("连接聊天SOCKET 失败","错误",0);
		return;
	}
	m_pClient =  theApp.server.AddSocket(socketClinet,AddressPeer);
	m_pClient->Attach(PACKET_ID_MESSAGE_ONLINE,this);
	m_pClient->SetHostWnd(m_hWnd);
}


void CChatDlg::OnBnClickedButtonLog()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_pAllDlg->IsWindowVisible())
	{
		CString strAllLog;
		m_pChatLogDealer->ReadAllChatLog(strAllLog);
		m_pAllDlg->SetLog(strAllLog);

		CRect rcClient;
		GetWindowRect(&rcClient);

		if (NULL != m_pAllDlg)
			m_pAllDlg->SetWindowPos(NULL,rcClient.left + rcClient.Width(),rcClient.top,0,0,SWP_NOSIZE);

		m_pAllDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pAllDlg->ShowWindow(SW_HIDE);
	}
}

void CChatDlg::OnMove(int x, int y)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	if (NULL != m_pAllDlg)
		m_pAllDlg->SetWindowPos(NULL,x + rcClient.Width(),y-24,0,0,SWP_NOSIZE);
}

void CChatDlg::OnSize(UINT nType, int cx, int cy)
{

}


void CChatDlg::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedOnCancel();
}


void CChatDlg::OnBnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedOk();
}
