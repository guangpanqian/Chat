#pragma once
#include "..\include\Package\packetDef.h"
#include <string>
using namespace  std;

// 用户信息
class UserInfo
{
public:
	UserInfo(void);
	~UserInfo(void);
	// 重置
	void ReSet();
	// 状态
	USER_STATUS m_status;
	// ID
	u_short m_id;
	// 姓名
	char m_name[USER_NAME_LENGTH];
	// SOCKET
	SOCKET m_socket;
	// 端口号
	u_short m_port;
	// IP
	string m_ip;
	// 登陆时间
	SYSTEMTIME m_loginTime;
	// 心跳时间
	SYSTEMTIME m_heartTime;

};

