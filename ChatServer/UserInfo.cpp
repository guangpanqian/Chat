#include "StdAfx.h"
#include "UserInfo.h"
#include <winbase.h>


UserInfo::UserInfo(void)
{
	ReSet();
}


UserInfo::~UserInfo(void)
{
}

void UserInfo::ReSet()
{
	m_socket = INVALID_SOCKET;
	m_status = OFFLINE;
	m_port = 0;
	m_ip = "";
	ZeroMemory(&m_loginTime,sizeof(m_loginTime));
	ZeroMemory(&m_heartTime,sizeof(m_heartTime));
}