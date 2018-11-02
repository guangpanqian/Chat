#pragma once
#define DLL_FUNCTION  AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
#define  WM_DATA_ARRIVED									(WM_USER + 1)
#define  WM_OFFLINE_MESSAGE_ARRIVED							(WM_USER + 2)
#define  WM_CLIENT_ACCEPT									(WM_USER + 3)
#define  WM_ONLINE_MESSAGE_ARRIVED							(WM_USER + 4)
#define  WM_ONLINE_MESSAGE									(WM_USER + 5)
#define  WM_SOCKET_INVALID									(WM_USER + 6)
#define  WM_CLIENT_LOST										(WM_USER + 7)
#define  WM_CHAT_DLG_DESTORY								(WM_USER + 8)