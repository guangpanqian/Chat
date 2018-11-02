// AllLogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "AllLogDlg.h"
#include "afxdialogex.h"


// CAllLogDlg 对话框

IMPLEMENT_DYNAMIC(CAllLogDlg, CDialog)

CAllLogDlg::CAllLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAllLogDlg::IDD, pParent)
{
}

CAllLogDlg::~CAllLogDlg()
{
}

void CAllLogDlg::SetLog(CString strLog)
{
	m_strLog = strLog;
	UpdateData(FALSE);
}

void CAllLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOG, m_strLog);
}


BEGIN_MESSAGE_MAP(CAllLogDlg, CDialog)
END_MESSAGE_MAP()


// CAllLogDlg 消息处理程序
