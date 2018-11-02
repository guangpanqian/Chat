#pragma once
#include "Interfaces.h"
#include "Client.h"
#include "AllLogDlg.h"


// CChatDlg �Ի���

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
		);   // ��׼���캯��
	virtual ~CChatDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_CHAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOnCancel();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// ����Ϣ����
	afx_msg LRESULT OnMessage(WPARAM,LPARAM);
	// �׽�����Ч��
	afx_msg LRESULT OnSocketInvalid(WPARAM wParam,LPARAM lParam);

	virtual BOOL OnInitDialog();
	// �Է����ߡ�����ת��
	void ChangeClient(CClient *pClinet);
	// �����µ���Ϣ
	void AppendMessage(CString message);
private:
	// ���������Ϣ
	USER_ONLINE *m_pUserOnline;
	// �������SOCKET��װ
	CClient *m_pClient;
	// �����¼������
	ChatLogDealer *m_pChatLogDealer;
	// �����¼��ʾ����
	CAllLogDlg *m_pAllDlg;
private:
	
public:
	// ������������ʱ����
	void ChangeClient();
	// ������������ʱ����
	void SetTitle();
private:
	// ������Ϣ���Է�
	void Send2Peer(const CString& szText);
	// �������ݰ�
	virtual BOOL RecvPacket(PPACKET_HDR packet );
	// ���ӵ��Է�
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
