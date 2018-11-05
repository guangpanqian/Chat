#include "StdAfx.h"
#include "EmployeeDBOperator.h"
#include "..\include\Package\packetDef.h"
#include "MainFrm.h"
#include "ChatServer.h"


extern CChatServerApp theApp;

EmployeeDBOperator::EmployeeDBOperator(void)
{
	connpool = ConnPool::GetInstance();
}


EmployeeDBOperator::~EmployeeDBOperator(void)
{
}

void EmployeeDBOperator::GetEmployees(vector<USER_BASE_INFO>&users)
{
	try
	{
		if (nullptr == connpool)
			return;

		// 从连接池中获取一个链接
		Connection *con =  connpool->GetConnection();

		// 从链接中获取一个状态，执行SQL用
		Statement *state = con->createStatement();

		// 存放查询出来的数据
		ResultSet *result = NULL;

		state->execute("use mysql");  
		state->execute("set charset gb2312");

		string sql = "select id,name from employee";
		result = state->executeQuery(sql);
		while(result->next())
		{
			USER_BASE_INFO user;
			user.userId = result->getInt("id");
			strcpy_s(user.szUserName,result->getString("name").c_str());
			users.push_back(user);
		}
		delete result;
		delete state;

		// 释放链接给链接池
		connpool->ReleaseConnection(con);
	}
	catch (sql::SQLException& e){
		char szException[1024] ={};
		sprintf_s(szException,"GetEmployees:Exception %s",e.what());
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szException);
	}
}


void EmployeeDBOperator::GetOfflineMessage(u_short id,vector<PACKET_USER_MESSAGE>& offlines)
{
	try
	{
		Connection *conn = connpool->GetConnection();
		Statement* state = conn->createStatement();
		ResultSet *result = NULL;
		state->execute("use mysql");
		state->execute("set charset gb2312");

		char szSQL[128] = {};
		sprintf_s(szSQL," select id,idfrom,idto,sentence from employee_offline_message where idto = %d ",id);
		result = state->executeQuery(szSQL);

		vector<u_short>ids;
		PACKET_USER_MESSAGE message;

		while(result->next())
		{
			ids.push_back(result->getInt("id"));

			ZeroMemory(&message,sizeof(message));
			message.senderId = result->getInt("idfrom");
			message.peerId = result->getInt("idto");
			result->getBlob("sentence")->read(message.message,sizeof(message.message));
			offlines.push_back(message);
		}

		// 将离线消息从数据库中清空
		vector<u_short>::iterator first = ids.begin();
		vector<u_short>::iterator last = ids.end();
		for(;first!=last;++first)
		{
			sprintf_s(szSQL,"delete from employee_offline_message where id = %d",*first);
			bool bsucceed = state->execute(szSQL);
			if (!bsucceed){
			continue;
			}
		}

		delete result;
		delete state;
		connpool->ReleaseConnection(conn);

	}catch (sql::SQLException& e){
			char szException[1024] ={};
			sprintf_s(szException,"GetOfflineMessage:Exception %s",e.what());
			((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szException);
	}
}

void EmployeeDBOperator::AddEmployee(string name,string password)
{
	try
	{
		Connection *conn = connpool->GetConnection();
		Statement *state = conn->createStatement();
		state->execute("use mysql");
		state->execute("set charset gb2312");
		char szSQL[64] = {};
		sprintf_s(szSQL,"insert into Employee values(null,'%s','%s')",name.c_str(),password.c_str());
		bool bSucceed = state->execute(szSQL);
		delete state;
		connpool->ReleaseConnection(conn);

	}catch (sql::SQLException& e){
	char szException[1024] ={};
	sprintf_s(szException,"AddEmployee:Exception %s",e.what());
	((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szException);
	}
}

void EmployeeDBOperator::WriteOfflineMessage(PPACKET_USER_MESSAGE message)
{
	try 
	{
		Connection *conn = connpool->GetConnection();
		Statement *state = conn->createStatement();
		state->execute("use mysql");
		state->execute("set charset gb2312");
		char szSQL[256] = {};
		sprintf_s(szSQL,"insert into employee_offline_message values(null,%d,%d,'%s')",message->senderId,message->peerId,message->message);
		bool bSucceed = state->execute(szSQL);
		delete state;
		connpool->ReleaseConnection(conn);
	}catch (sql::SQLException& e){
		char szException[1024] ={};
		sprintf_s(szException,"WriteOfflineMessage:Exception %s",e.what());
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szException);
	}
}

void EmployeeDBOperator::GetEmployeeName(u_short id,string password,char *userName)
{
	try
	{
		Connection *conn = connpool->GetConnection();
		Statement* state = conn->createStatement();
		state->execute("use mysql");
		state->execute("set charset gb2312");

		ResultSet *result = NULL;
		char szSQL[128] = {};
		sprintf_s(szSQL,"select name from employee where id = %d and password = '%s' ",id,password.c_str());
		result = state->executeQuery(szSQL);
		strcpy(userName,"");
		while(result->next())
		{
			string name = result->getString("name");
			lstrcpyn(userName,name.c_str(),name.length()+1);
		}
		delete result;
		delete state;
		connpool->ReleaseConnection(conn);
	}catch(sql::SQLException &e){
		char szException[1024] ={};
		sprintf_s(szException,"GetEmployeeName:Exception %s",e.what());
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szException);
	}
}
