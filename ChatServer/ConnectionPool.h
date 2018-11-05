#pragma once
#include "mysql_connection.h"
#include "mysql_driver.h"
#include "cppconn/exception.h"
#include "cppconn/driver.h"
#include "cppconn/connection.h"
#include "cppconn/resultset.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/statement.h"
#include <list>

using namespace std;
using namespace sql;

// ���ݿ����ӳ��࣬�������ݿ����ӣ�����Ҫÿ�ζ����´������ӣ������������������ֹ���ӹ��࡭��
class ConnPool {
private:
	// ��ǰ������
	int curSize; 
	// ���������
	int maxSize;
	// db �û���
	string username;
	// db �˺�
	string password;
	// db ��ַ
	string url;
	// list ά�����ӳ�
	list<Connection*> connList;
	// ��̬�����ַ
	static ConnPool *connPool;
	// sql ����
	Driver*driver;
	// ��������
	Connection*CreateConnection();
	// ��ʼ������
	void InitConnection(int iInitialSize);
	// ��������
	void DestoryConnection(Connection *conn); 
	// �������ӳ�
	void DestoryConnPool(); 
	// ˽�еĹ��캯������ֹ���ֱ�ӷ��ʣ������ṩһ��ȫ�ֵĽӿ�
	ConnPool(string url, string user, string password, int maxSize);
public:
	~ConnPool();
	// �̱߳���
	static CRITICAL_SECTION cslock;
	static CRITICAL_SECTION csInstance;
	// �����ӳ��л�ȡһ������
	Connection*GetConnection();
	// �ͷ����Ӹ����ӳ�
	void ReleaseConnection(Connection *conn); 
	// �ṩһ��ȫ�ַ��ʵ��ȡ���ӳص�ַ
	static ConnPool *GetInstance();
};