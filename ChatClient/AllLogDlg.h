#pragma once


// CAllLogDlg �Ի���

// ���������¼����
class CAllLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CAllLogDlg)

public:
	CAllLogDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAllLogDlg();
	// ���������¼
	void SetLog(CString strLog);
// �Ի�������
	enum { IDD = IDD_DIALOG_ALL_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ���������¼
	CString m_strLog;
};
