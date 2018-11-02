#pragma once
#include "..\include\Package\packetDef.h"
#include <string>
using namespace  std;

class UserInfo
{
public:
	UserInfo(void);
	~UserInfo(void);
	void ReSet();
	USER_STATUS m_status;
	u_short m_id;
	char m_name[USER_NAME_LENGTH];
	SOCKET m_socket;
	u_short m_port;
	string m_ip;
	SYSTEMTIME m_loginTime;
	SYSTEMTIME m_heartTime;

};

