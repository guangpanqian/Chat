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

// 数据库连接池类，管理数据库连接，不需要每次都重新创建连接，设有最大连接数，防止连接过多……
class ConnPool {
private:
	// 当前连接数
	int curSize; 
	// 最大连接数
	int maxSize;
	// db 用户名
	string username;
	// db 账号
	string password;
	// db 地址
	string url;
	// list 维护连接池
	list<Connection*> connList;
	// 静态对象地址
	static ConnPool *connPool;
	// sql 驱动
	Driver*driver;
	// 创建连接
	Connection*CreateConnection();
	// 初始化连接
	void InitConnection(int iInitialSize);
	// 销毁连接
	void DestoryConnection(Connection *conn); 
	// 销毁连接池
	void DestoryConnPool(); 
	// 私有的构造函数，禁止外界直接访问，下面提供一个全局的接口
	ConnPool(string url, string user, string password, int maxSize);
public:
	~ConnPool();
	// 线程保护
	static CRITICAL_SECTION cslock;
	static CRITICAL_SECTION csInstance;
	// 从连接池中获取一个连接
	Connection*GetConnection();
	// 释放链接给连接池
	void ReleaseConnection(Connection *conn); 
	// 提供一个全局访问点获取连接池地址
	static ConnPool *GetInstance();
};