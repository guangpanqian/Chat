
// ChatClient.h : ChatClient 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "Server.h"


// CChatClientApp:
// 有关此类的实现，请参阅 ChatClient.cpp
//
class SelectSocketCenter;
class CChatClientApp : public CWinApp
{
public:
	CChatClientApp();
	virtual ~CChatClientApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	// SOCKET 模块封装
	Server server;
	// userid
	u_short id;
	// 密码
	CString password;
	// IP
	CString serverIP;
	// 端口号
	u_short serverPort;

private:
	void InitMembers();
};

extern CChatClientApp theApp;
