
// ChatClient.h : ChatClient Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "Server.h"


// CChatClientApp:
// �йش����ʵ�֣������ ChatClient.cpp
//
class SelectSocketCenter;
class CChatClientApp : public CWinApp
{
public:
	CChatClientApp();
	virtual ~CChatClientApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	// SOCKET ģ���װ
	Server server;
	// userid
	u_short id;
	// ����
	CString password;
	// IP
	CString serverIP;
	// �˿ں�
	u_short serverPort;

private:
	void InitMembers();
};

extern CChatClientApp theApp;
