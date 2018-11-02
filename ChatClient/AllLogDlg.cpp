// AllLogDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChatClient.h"
#include "AllLogDlg.h"
#include "afxdialogex.h"


// CAllLogDlg �Ի���

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


// CAllLogDlg ��Ϣ�������
