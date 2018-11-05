#pragma once
#include "..\include\Package\packetDef.h"
#include <string>
using namespace  std;

// �û���Ϣ
class UserInfo
{
public:
	UserInfo(void);
	~UserInfo(void);
	// ����
	void ReSet();
	// ״̬
	USER_STATUS m_status;
	// ID
	u_short m_id;
	// ����
	char m_name[USER_NAME_LENGTH];
	// SOCKET
	SOCKET m_socket;
	// �˿ں�
	u_short m_port;
	// IP
	string m_ip;
	// ��½ʱ��
	SYSTEMTIME m_loginTime;
	// ����ʱ��
	SYSTEMTIME m_heartTime;

};

