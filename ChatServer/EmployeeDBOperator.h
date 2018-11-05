#pragma once

#include  <vector>
#include <string>
#include <WinSock2.h>
#include "..\include\Package\packetDef.h"
#include "ConnectionPool.h"
#include "CommonDef.h"

using namespace std;
class EmployeeDBOperator
{
public:
	EmployeeDBOperator(void);
	~EmployeeDBOperator(void);
private:
	// 数据库连接池，需要使用池里的连接
	ConnPool *connpool;

public:
	// 获取用户列表
	void GetEmployees(vector<USER_BASE_INFO>&users);
	// 获取发送给指定用户的离线信息
	void GetOfflineMessage(u_short id,vector<PACKET_USER_MESSAGE>& messages);
	// 添加用户到数据库
	void AddEmployee(string name,string password);
	// 获取用户姓名
	void GetEmployeeName(u_short id,string password,char *userName);
	// 往数据库写入离线消息
	void WriteOfflineMessage(PPACKET_USER_MESSAGE message);
	// 删除用户
	void DelEmployee(u_short id);
	// 更新用户信息
	void UpdateEmployee(u_short id, string name, string password);
};

