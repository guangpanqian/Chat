#pragma once
#include <shobjidl.h>
#include <ws2def.h>
#include "..\include\Package\packetDef.h"
class SocketClient;

const int MAX_BUF_SIZE = 1024*64;

// 异步操作类型
typedef enum _socket_type
{
	WRITE,
	READ
}SOCKET_TYPE;

// 重叠IO结构
typedef struct _io_operation_data
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	TCHAR bufffer[MAX_BUF_SIZE];
	SOCKET_TYPE type;
}IO_OPERATION_DATA,*PIO_OPERATION_DATA;

// 用户基本信息
typedef struct _user_base_info
{
	u_short userId;
	char szUserName[USER_NAME_LENGTH];
}USER_BASE_INFO,*PUSER_BASE_INFO;

// 定义一些消息
#define  WM_ACCEPT								(WM_USER + 1)
#define  WM_LOGIN								(WM_USER + 2)
#define  WM_HEART								(WM_USER + 3)
#define  WM_CLIENT_LOST_TOUCH					(WM_USER + 4)