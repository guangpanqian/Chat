#pragma once
#include "Interfaces.h"
#include "Client.h"
#include "AllLogDlg.h"


// CChatDlg 对话框

class ChatLogDealer;
class CChatDlg : public CDialog,public ISocketObserver
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(CWnd* pParent = NULL,
		CClient *pClinet = NULL,
		u_short id = 0,
		CString strName="", 
		USER_ONLINE *pUserOnline = NULL,
		CString message = ""
		);   // 标准构造函数
	virtual ~CChatDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOnCancel();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// 有消息到了
	afx_msg LRESULT OnMessage(WPARAM,LPARAM);
	// 套接字无效了
	afx_msg LRESULT OnSocketInvalid(WPARAM wParam,LPARAM lParam);

	virtual BOOL OnInitDialog();
	// 对方在线、离线转换
	void ChangeClient(CClient *pClinet);
	// 增加新的消息
	void AppendMessage(CString message);
private:
	// 聊天对象信息
	USER_ONLINE *m_pUserOnline;
	// 聊天对象SOCKET封装
	CClient *m_pClient;
	// 聊天记录处理者
	ChatLogDealer *m_pChatLogDealer;
	// 聊天记录显示窗口
	CAllLogDlg *m_pAllDlg;
private:
	
public:
	// 更改在线离线时对象
	void ChangeClient();
	// 更改在线离线时标题
	void SetTitle();
private:
	// 发送消息给对方
	void Send2Peer(const CString& szText);
	// 接收数据包
	virtual BOOL RecvPacket(PPACKET_HDR packet );
	// 连接到对方
	void Connect2Client();
public:
	CString m_strNow;
	CString m_strRecord;
	CString m_strName;
	u_short m_id;
	afx_msg void OnBnClickedButtonLog();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonSend();
};
