// ChatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include "CommonDef.h"
#include "ChatLogDealer.h"

// CChatDlg �Ի���
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
		// ������Ϣ���͸��������
		m_pClient->Attach(PACKET_ID_MESSAGE_ONLINE,this);
	}

	m_pChatLogDealer = new ChatLogDealer(id,pUserOnline->id);
	m_pAllDlg = NULL;
}

void CChatDlg::ChangeClient(CClient *pClinet)
{
	// �����������ڹ����Ĳ��Ƿ�������������֮
	if (theApp.server.pServer != m_pClient)
	{
		delete m_pClient;
	}
	// �����µĿͻ���
	m_pClient = pClinet;

	// ������µĲ��Ƿ�����������������Ϣ
	if (NULL != m_pClient && m_pClient!= theApp.server.pServer)
		m_pClient->Attach(PACKET_ID_MESSAGE_ONLINE,this);
}

CChatDlg::~CChatDlg()
{
	// ��Ҫ��֪ͨ���������
	if (NULL !=m_pClient)
	{
		m_pClient->Detach(PACKET_ID_MESSAGE_ONLINE);
	}

	// �洢�����¼
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

// CChatDlg ��Ϣ�������
void CChatDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_strNow.IsEmpty())
	{
		MessageBox("���ܷ��Ϳ���Ϣ","��ܰ��ʾ",0);
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
		szTitle.Format("��%s������",m_pUserOnline->userName);
	}
	else
	{
		szTitle.Format("%s������Ŷ",m_pUserOnline->userName);
	}
	SetWindowText(szTitle);
}

void CChatDlg::ChangeClient()
{
	// ֮ǰ������״̬�����������ˣ���ô��������������ӷ�����
	if (ONLINE == m_pUserOnline->status)
	{
		m_pClient = theApp.server.pServer;
	}
	// �����������һ���û�
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
	// �׽�����Ч��
	ChangeClient(NULL);
	return 0;
}

void CChatDlg::Connect2Client()
{
	SOCKET socketClinet = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == socketClinet)
	{
		MessageBox("�����ͻ���SOCKETʧ��","����",0);
		return;
	}

	SOCKADDR_IN AddressPeer;
	AddressPeer.sin_family = AF_INET;

	AddressPeer.sin_port = m_pUserOnline->port;
	AddressPeer.sin_addr.s_addr = inet_addr(m_pUserOnline->ip);

	if (SOCKET_ERROR  == connect(socketClinet,PSOCKADDR(&AddressPeer),sizeof(SOCKADDR_IN)))
	{
		MessageBox("��������SOCKET ʧ��","����",0);
		return;
	}
	m_pClient =  theApp.server.AddSocket(socketClinet,AddressPeer);
	m_pClient->Attach(PACKET_ID_MESSAGE_ONLINE,this);
	m_pClient->SetHostWnd(m_hWnd);
}


void CChatDlg::OnBnClickedButtonLog()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnBnClickedOnCancel();
}


void CChatDlg::OnBnClickedButtonSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnBnClickedOk();
}
