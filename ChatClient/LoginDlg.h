#pragma once
#include "Interfaces.h"


// CLoginDlg 对话框

class CLoginDlg : public CDialog, public ISocketObserver
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL,CClient *pClient = NULL);   // 标准构造函数
	virtual ~CLoginDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	const BOOL IsLoginSucceed() const;
	void Login();
private:
	BOOL m_bLoginSucceed;
	HANDLE m_hRevcAlready;
	CClient *m_pClient;
public:
	afx_msg void OnBnClickedOk();
	UINT m_nId;
	CString m_strPwd;
	CString m_strName;

	virtual BOOL  RecvPacket(PPACKET_HDR packet) override;
};
