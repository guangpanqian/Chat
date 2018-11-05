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
		perror("驱动连接出错;\n");
	} catch (std::runtime_error&e) {
		perror("运行出错了\n");
	}

	// 创建一个最大连接数一半的链接池
	this->InitConnection(maxSize / 2);
}

ConnPool*ConnPool::GetInstance()
{
	// 注意双重锁结构

	if (NULL == connPool) 
	{
		EnterCriticalSection(&csInstance);
		// 注意这里还有一次判空，因为如果不加的话，可能某个线程刚创建完，那么会再创建一个对象，不是单例了。。
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
	// 初始化连接池，放入链接池链表中
	Connection*conn;
	EnterCriticalSection(&cslock);
	for (int i = 0; i < iInitialSize; i++) 
	{
		conn = this->CreateConnection();
		if (conn) 
		{
			// 放入链接池
			connList.push_back(conn);
			++(this->curSize);
		}
		else 
		{
			perror("创建CONNECTION出错");
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
		perror("创建连接出错");
		return NULL;
	} catch (std::runtime_error&e) {
		perror("运行时出错");
		return NULL;
	}
}

Connection*ConnPool::GetConnection() 
{
	Connection*con;
	EnterCriticalSection(&cslock);

	// 如果池中还有水位
	if (connList.size() > 0) 
	{
		// 拿出第一个链接
		con = connList.front();
		connList.pop_front();

		// 如果链接无效，则创建一个
		if (con->isClosed() || !con->isValid()) 
		{
			delete con;
			con = this->CreateConnection();
		}

		// 这个链接是空的，池水位需要降低
		if (con == NULL)
		{
			--curSize;
		}

		LeaveCriticalSection(&cslock);
		return con;
	} 
	// 已经空了
	else
	{
		// 还没有到最大链接数
		if (curSize < maxSize)
		{
			// 创建一个新的链接
			con = this->CreateConnection();
			if (con) 
			{
				++curSize;
				LeaveCriticalSection(&cslock);
				return con;
			} 
			// 没创建成功，只能返回了
			else 
			{
				LeaveCriticalSection(&cslock);
				return NULL;
			}
		} 
		// 到最大链接数了，对不住了，先等等吧
		else
		{
			LeaveCriticalSection(&cslock);
			return NULL;
		}
	}
}

void ConnPool::ReleaseConnection(sql::Connection * conn) 
{
	// 放回链接池
	if (conn) 
	{
		EnterCriticalSection(&cslock);
		connList.push_back(conn);
		LeaveCriticalSection(&cslock);
	}
}

ConnPool::~ConnPool()
{
	// 析构的时候销毁连接池
	this->DestoryConnPool();
}


void ConnPool::DestoryConnPool() 
{
	// 关闭所有链接
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
	// 关闭链接
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