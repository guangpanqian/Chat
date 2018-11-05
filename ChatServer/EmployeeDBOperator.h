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
	// ���ݿ����ӳأ���Ҫʹ�ó��������
	ConnPool *connpool;

public:
	// ��ȡ�û��б�
	void GetEmployees(vector<USER_BASE_INFO>&users);
	// ��ȡ���͸�ָ���û���������Ϣ
	void GetOfflineMessage(u_short id,vector<PACKET_USER_MESSAGE>& messages);
	// ����û������ݿ�
	void AddEmployee(string name,string password);
	// ��ȡ�û�����
	void GetEmployeeName(u_short id,string password,char *userName);
	// �����ݿ�д��������Ϣ
	void WriteOfflineMessage(PPACKET_USER_MESSAGE message);
	// ɾ���û�
	void DelEmployee(u_short id);
	// �����û���Ϣ
	void UpdateEmployee(u_short id, string name, string password);
};

