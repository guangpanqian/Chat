
// ChatServer.h : ChatServer 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "SocketServer.h"
#include "ConnectionPool.h"

// CChatServerApp:
// 有关此类的实现，请参阅 ChatServer.cpp
//

class CChatServerApp : public CWinApp
{
public:
	CChatServerApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	// 网络处理
	SocketServer socketServer;
	// 数据库处理
	EmployeeDBOperator *pEmployeeDBOperator;
};

extern CChatServerApp theApp;
