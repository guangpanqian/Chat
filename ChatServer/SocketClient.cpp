#include "StdAfx.h"
#include "SocketClient.h"
#include "..\include\Package\packetDef.h"
#include "ChatServer.h"
#include "MainFrm.h"

extern CChatServerApp theApp;
SocketClient::SocketClient(SOCKET socket,SOCKADDR_IN address)
	:m_socket(socket),
	m_address(address)
{
	ZeroMemory(&m_io,sizeof(IO_OPERATION_DATA));
}

SocketClient::~SocketClient(void)
{
	// 取消与该套接字相关的IO操作
	CancelIo((HANDLE)m_socket);

	// 关闭套接字句柄
	closesocket(m_socket);
}

const SOCKET & SocketClient::GetSocket() const
{
	return m_socket;
}

const SOCKADDR_IN &SocketClient::GetAddress() const
{
	return m_address;
}

bool SocketClient::AsyncSend()
{
	// 异步发送
	m_io.type = WRITE;
	m_io.wsaBuf.buf = m_io.bufffer;

	DWORD dwFlag = 0;
	if (SOCKET_ERROR ==  WSASend(m_socket,&m_io.wsaBuf,1,(DWORD*)&m_io.wsaBuf.len,dwFlag,&m_io.overlapped,NULL))
	{
		int errCode =  WSAGetLastError();
		return ERROR_IO_PENDING == errCode;
	}
	return true;
}

bool SocketClient::AsyncRecv()
{
	// 异步接收
	ZeroMemory(&m_io,sizeof(m_io));
	m_io.type = READ;
	m_io.wsaBuf.buf = m_io.bufffer;
	m_io.wsaBuf.len = MAX_BUF_SIZE;

	DWORD dwBytesRecv = 0;
	DWORD dwFlag = 0;
	if (SOCKET_ERROR == WSARecv(m_socket,&m_io.wsaBuf,1,&dwBytesRecv,&dwFlag,&m_io.overlapped,NULL))
	{
		return WSAGetLastError()  == ERROR_IO_PENDING;
	}
	return true;
}

void SocketClient::HandlData(LPOVERLAPPED lpOverlapped,DWORD dwNumberOfBytesTransffed)
{
	PIO_OPERATION_DATA  pIO = CONTAINING_RECORD(lpOverlapped,IO_OPERATION_DATA,overlapped);
	switch(pIO->type)
	{
	default:
		break;
		// IO完成端口读完成
	case READ:
		{
			PPACKET_HDR phdr = (PPACKET_HDR)pIO->bufffer;

			// 是否有包需要发送
			BOOL bHasPackets = TRUE;
			switch(phdr->type)
			{
			case PACKET_ID_LOGIN:
					DealPacketLogin();
				break;
			case PACKET_ID_USERS:
					DealPacketUsers();
				break;
			case PACKET_ID_HEART:
					DealPacketHeart();
					// 心跳包来了之后把当前所有用户的状态回馈给该用户，以让之能及时更新用户状态
					DealPacketUsers(PACKET_ID_STATUS_CHANGE);
				break;
			case PACKET_ID_MESSAGE_OFFLINE:
					DealPacketOffline();
					// 发送过来离线数据，这里直接入库了，不发送回馈包了
					bHasPackets = FALSE;
				break;
			case PACKET_ID_LOAD_OFFLINE:
					DealPacketLoadOffline();
				break;
			default:
				// 包ID有问题
				bHasPackets = FALSE;
				break;
			}
			if (bHasPackets)
				// 那么异步发送下数据
				AsyncSend();
			else
				// 继续发送一个异步接收请求
				AsyncRecv();
		}
		break;
		//	IO完成端口写数据完成
	case WRITE:
			AsyncRecv();
		break;
	}
}

void  SocketClient::DealPacketLogin()
{
	
	try
	{
		PPACKET_LOGIN pLogin = PPACKET_LOGIN(m_io.bufffer);
		memcpy(pLogin,m_io.bufffer,sizeof(PACKET_LOGIN));

		WPARAM wParam = MAKEWPARAM(pLogin->login.port,pLogin->login.userId);

		// 修改重叠IO结构，为下一步异步发送数据做好准备
		PPACKET_LOGIN_A loginA = (PPACKET_LOGIN_A)m_io.bufffer;
		loginA->header.type = PACKET_ID_LOGIN;
		loginA->header.len = sizeof(PACKET_LOGIN_A);
		m_io.wsaBuf.len = loginA->header.len;
	
		theApp.pEmployeeDBOperator->GetEmployeeName(pLogin->login.userId,pLogin->login.userPassword,loginA->loginA.userName);
		loginA->loginA.succeed = strlen(loginA->loginA.userName) > 0;
	
		// 通知主线程该用户登陆成功与否
		if (loginA->loginA.succeed)
		{
			::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_LOGIN,wParam,LPARAM(m_socket));
		}
		else
		{
			::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_LOGIN,wParam,LPARAM(INVALID_SOCKET));
		}

	}catch (CMemoryException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CFileException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch(...){
		char szExceptionMessage[128];
		sprintf_s(szExceptionMessage,"%s","Unhandled exception.");
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}
}

void SocketClient::DealPacketLoadOffline()
{
	try
	{
		PPACKET_OFFLINE_MESSAGE pOffLine = PPACKET_OFFLINE_MESSAGE(m_io.bufffer);
		vector<PACKET_USER_MESSAGE> offlineMessages;
		theApp.pEmployeeDBOperator->GetOfflineMessage(pOffLine->id,offlineMessages);
		int offlineCounts = int(offlineMessages.size());

		PPACKET_OFFLINE_MESSAGE_A packetOffline = (PPACKET_OFFLINE_MESSAGE_A)m_io.bufffer;
		ZeroMemory(packetOffline,sizeof(PACKET_OFFLINE_MESSAGE_A));

		int headerLen = sizeof(PACKET_OFFLINE_MESSAGE_A) - sizeof(PPACKET_USER_MESSAGE);
		packetOffline->header.type = PACKET_ID_LOAD_OFFLINE;
		packetOffline->header.len   = headerLen + offlineCounts * sizeof(PACKET_USER_MESSAGE);
		m_io.wsaBuf.len = packetOffline->header.len;
		packetOffline->counts = offlineCounts;

		PPACKET_USER_MESSAGE messages = (PPACKET_USER_MESSAGE)((char*)(packetOffline) + headerLen);

		for (int index = 0;index < offlineCounts;++index)
		{
			memcpy(&messages[index],&offlineMessages[index],sizeof(PACKET_USER_MESSAGE));
		}

	}catch (CMemoryException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CFileException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}
}

void SocketClient::DealPacketUsers(int packetType)
{
	try
	{
		// 从数据库加载所有用户信息
		vector<USER_BASE_INFO>employees;
		theApp.pEmployeeDBOperator->GetEmployees(employees);

		int employCounts = int(employees.size());
		int headerLen = sizeof(PACKET_USERS_A) -sizeof(USER_ONLINE*);
		int length = headerLen + employCounts*sizeof(USER_ONLINE);

		m_io.wsaBuf.len = length;
		PPACKET_USERS_A packetUsersA  = (PPACKET_USERS_A)m_io.bufffer;
		ZeroMemory(packetUsersA,sizeof(PACKET_USERS_A));

		packetUsersA->header.type = packetType;
		packetUsersA->count = employCounts;
		packetUsersA->header.len = length;

		USER_ONLINE *pUsers = (USER_ONLINE*)((char*)packetUsersA + headerLen);
		
		for (int userIndex = 0;userIndex < employCounts;++userIndex)
		{
			pUsers[userIndex].id = employees[userIndex].userId;
			strcpy_s(pUsers[userIndex].userName,employees[userIndex].szUserName);

			CMainFrame *pMainFrame = ((CMainFrame*)(theApp.m_pMainWnd));

			// 获取用户的状态、端口号、IP地址
			pUsers[userIndex].status = pMainFrame->GetUserStatus(employees[userIndex].userId);
			pUsers[userIndex].port = pMainFrame->GetUserPort(employees[userIndex].userId);
			strcpy_s(pUsers[userIndex].ip,pMainFrame->GetUserIp( employees[userIndex].userId).c_str());
		}

	}catch (CMemoryException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CFileException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}
}

void SocketClient::DealPacketOffline()
{
	try
	{
		PPACKET_MESSAGE pOfflineMessage = (PPACKET_MESSAGE)m_io.bufffer;
		// 把离线消息写入数据库
		theApp.pEmployeeDBOperator->WriteOfflineMessage(&pOfflineMessage->userMessage);

	}catch (CMemoryException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CFileException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}
}

void SocketClient::DealPacketHeart()
{
	try
	{
		//	通知主线程，心跳来了
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_HEART,WPARAM(m_socket),LPARAM(0));
	}catch (CMemoryException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CFileException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}catch (CException* e){
		char szExceptionMessage[128];
		e->GetErrorMessage(szExceptionMessage,128,NULL);
		((CMainFrame*)(theApp.m_pMainWnd))->LogInfo(szExceptionMessage);
	}
}
