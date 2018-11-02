
// ChatServer.h : ChatServer Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "SocketServer.h"
#include "ConnectionPool.h"

// CChatServerApp:
// �йش����ʵ�֣������ ChatServer.cpp
//

class CChatServerApp : public CWinApp
{
public:
	CChatServerApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	// ���紦��
	SocketServer socketServer;
	// ���ݿ⴦��
	EmployeeDBOperator *pEmployeeDBOperator;
};

extern CChatServerApp theApp;
