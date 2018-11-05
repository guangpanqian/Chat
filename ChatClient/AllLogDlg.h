#pragma once


// CAllLogDlg 对话框

// 所有聊天记录窗口
class CAllLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CAllLogDlg)

public:
	CAllLogDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAllLogDlg();
	// 设置聊天记录
	void SetLog(CString strLog);
// 对话框数据
	enum { IDD = IDD_DIALOG_ALL_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 具体聊天记录
	CString m_strLog;
};
