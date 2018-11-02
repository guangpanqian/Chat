#include "stdafx.h"
#include <stdexcept>
#include <exception>
#include <stdio.h>
#include "ConnectionPool.h"

using namespace std;
using namespace sql;

ConnPool *ConnPool::connPool = NULL;
CRITICAL_SECTION ConnPool::cslock;
CRITICAL_SECTION ConnPool::csInstance;

ConnPool::ConnPool(string url, string userName, string password, int maxSize)
{
	this->maxSize = maxSize;
	this->curSize = 0;
	this->username = userName;
	this->password = password;
	this->url = url;

	try {
		this->driver = sql::mysql::get_driver_instance();
	}catch (sql::SQLException&e){
		perror("�������ӳ���;\n");
	} catch (std::runtime_error&e) {
		perror("���г�����\n");
	}

	// ����һ�����������һ������ӳ�
	this->InitConnection(maxSize / 2);
}

ConnPool*ConnPool::GetInstance()
{
	if (NULL == connPool) 
	{
		EnterCriticalSection(&csInstance);
		if (NULL == connPool)
		{
			connPool = new ConnPool("tcp://127.0.0.1:3306", "root", "root", 100);
		}
		LeaveCriticalSection(&csInstance);
	}
	return connPool;
}

void ConnPool::InitConnection(int iInitialSize)
{
	// ��ʼ�����ӳأ��������ӳ�������
	Connection*conn;
	EnterCriticalSection(&cslock);
	for (int i = 0; i < iInitialSize; i++) 
	{
		conn = this->CreateConnection();
		if (conn) 
		{
			connList.push_back(conn);
			++(this->curSize);
		}
		else 
		{
			perror("����CONNECTION����");
		}
	}
	LeaveCriticalSection(&cslock);
}

Connection* ConnPool::CreateConnection()
{
	Connection*conn;
	try {
		conn = driver->connect(this->url, this->username, this->password);
		return conn;
	}catch (sql::SQLException&e) {
		perror("�������ӳ���");
		return NULL;
	} catch (std::runtime_error&e) {
		perror("����ʱ����");
		return NULL;
	}
}

Connection*ConnPool::GetConnection() 
{
	Connection*con;
	EnterCriticalSection(&cslock);

	// ������л���ˮλ
	if (connList.size() > 0) 
	{
		// �ó���һ������
		con = connList.front();
		connList.pop_front();

		// ���������Ч���򴴽�һ��
		if (con->isClosed() || !con->isValid()) 
		{
			delete con;
			con = this->CreateConnection();
		}

		// ��������ǿյģ���ˮλ��Ҫ����
		if (con == NULL)
		{
			--curSize;
		}

		LeaveCriticalSection(&cslock);
		return con;
	} 
	// �Ѿ�����
	else
	{
		// ��û�е����������
		if (curSize < maxSize)
		{
			// ����һ���µ�����
			con = this->CreateConnection();
			if (con) 
			{
				++curSize;
				LeaveCriticalSection(&cslock);
				return con;
			} 
			// û�����ɹ���ֻ�ܷ�����
			else 
			{
				LeaveCriticalSection(&cslock);
				return NULL;
			}
		} 
		// ������������ˣ��Բ�ס�ˣ��ȵȵȰ�
		else
		{
			LeaveCriticalSection(&cslock);
			return NULL;
		}
	}
}

void ConnPool::ReleaseConnection(sql::Connection * conn) 
{
	// �Ż����ӳ�
	if (conn) 
	{
		EnterCriticalSection(&cslock);
		connList.push_back(conn);
		LeaveCriticalSection(&cslock);
	}
}

ConnPool::~ConnPool()
{
	this->DestoryConnPool();
}


void ConnPool::DestoryConnPool() 
{
	// �ر���������
	list<Connection*>::iterator icon;
	EnterCriticalSection(&cslock);
	for (icon = connList.begin(); icon != connList.end(); ++icon)
	{
		this->DestoryConnection(*icon);
	}
	curSize = 0;
	connList.clear();
	LeaveCriticalSection(&cslock);
}

void ConnPool::DestoryConnection(Connection* conn) 
{
	// �ر�����
	if (conn)
	{
		try {
			conn->close();
		} catch (sql::SQLException&e) {
			perror(e.what());
		}catch (std::exception&e) {
			perror(e.what());
		}
		delete conn;
	}
}