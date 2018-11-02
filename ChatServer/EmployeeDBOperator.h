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
	ConnPool *connpool;
	
private:
	void InitMembers();
	bool CreateConnection();
	void CloseConnection();
public:
	void GetEmployees(vector<USER_BASE_INFO>&users);
	void GetOfflineMessage(u_short id,vector<PACKET_USER_MESSAGE>& messages);
	void AddEmployee(string name,string password);
	void GetEmployeeName(u_short id,string password,char *userName);
	void DelEmployee(u_short id);
	void UpdateEmployee(u_short id,string name,string password);
	void WriteOfflineMessage(PPACKET_USER_MESSAGE message);
};

