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

class ConnPool {
private:
	int curSize; 
	int maxSize;
	string username;
	string password;
	string url;
	list<Connection*> connList;
	static ConnPool *connPool;
	Driver*driver;

	Connection*CreateConnection();
	void InitConnection(int iInitialSize);
	void DestoryConnection(Connection *conn); 
	void DestoryConnPool(); 
	ConnPool(string url, string user, string password, int maxSize);
public:
	~ConnPool();
	static CRITICAL_SECTION cslock;
	static CRITICAL_SECTION csInstance;
	Connection*GetConnection();
	void ReleaseConnection(Connection *conn); 
	static ConnPool *GetInstance();
};