// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "Client.h"


// CLoginDlg 对话框

extern CChatClientApp theApp;

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/,CClient *pClient)
	: CDialog(CLoginDlg::IDD, pParent)
{
	m_bLoginSucceed = FALSE;
	m_nId = 1000;
	m_strPwd = _T("123456");
	m_hRevcAlready = CreateEvent(NULL,TRUE,FALSE,"");
	m_pClient = pClient;

	m_pClient->Attach(PACKET_ID_LOGIN,this);
}

CLoginDlg::~CLoginDlg()
{
	CloseHandle(m_hRevcAlready);
	m_pClient->Detach(PACKET_ID_LOGIN);
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ID, m_nId);
	DDX_Text(pDX, IDC_EDIT_PWD, m_strPwd);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序
const BOOL CLoginDlg::IsLoginSucceed() const
{
	return m_bLoginSucceed;
}

void CLoginDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	Login();
	CDialog::OnOK();
}

void CLoginDlg::Login()
{
	PPACKET_LOGIN packetLogin = new PACKET_LOGIN;
	ZeroMemory(packetLogin,sizeof(PACKET_LOGIN));
	packetLogin->header.type = PACKET_ID_LOGIN;
	packetLogin->header.len   = sizeof(PACKET_LOGIN);
	packetLogin->login.userId = m_nId;
	strcpy_s(packetLogin->login.userPassword,m_strPwd); 
	packetLogin->login.port = theApp.server.portListen;
	m_pClient->AddSendQueue((PPACKET_HDR)packetLogin);
	WaitForSingleObject(m_hRevcAlready,INFINITE);
}

BOOL  CLoginDlg::RecvPacket(PPACKET_HDR packet)
{
	PACKET_LOGIN_A  packetLoginA;
	ZeroMemory(&packetLoginA,sizeof(packetLoginA));
	packetLoginA.header.len = sizeof(packetLoginA);
	packetLoginA.header.type = PACKET_ID_LOGIN;

	if (m_pClient->Recv((char*)&packetLoginA.loginA,packetLoginA.header.len - sizeof(PACKET_HDR)))
	{
		m_bLoginSucceed = packetLoginA.loginA.succeed?TRUE:FALSE;
		if (m_bLoginSucceed)
			m_strName = packetLoginA.loginA.userName;
	}
	else
	{
		MessageBox("Recv from Server failed!","Socket Error",0);
		return FALSE;
	}
	SetEvent(m_hRevcAlready);

	return TRUE;
}


