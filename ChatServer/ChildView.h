
// ChildView.h : CChildView 类的接口
//


#pragma once
#include "EmployeeDBOperator.h"
#include <map>
#include "..\include\Package\packetDef.h"
#include "UserInfo.h"
#include "CommonDef.h"
using namespace std;

// CChildView 窗口
class CChildView : public CWnd
{
// 构造
public:
	CChildView();
// 特性
public:

// 操作
public:

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelChangeTab(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

private:
	// 用户列表
	CListCtrl   m_lstCtlUsers;
	// 日志列表
	CListCtrl   m_lstCtlLog;
	// 切换列表的tab控件
	CTabCtrl   m_tabCtl;
	//	显示字体
	CFont       m_font;
	// userid 到列表行的映射
	map<u_short,int>m_employee2ListIndex;
	// userid 到用户信息的映射
	map<u_short,UserInfo>m_employees;
	// SOCKET 到用户ID的映射
	map<SOCKET,u_short>m_mpSocket2Id;
	// 在线，离线状态图片
	CBitmap m_bitmaps[2];
	CImageList m_ImageList;

private:
	// 创建子控件
	void CreateChildren();
	// 获取用户列表
	void GetEmployee();
	// 检查是否有用户掉线
	void CheckClientLostTouch();
	// 设置该用户状态为离线
	void SetOffLine(u_short userId);
	// 重置map状态
	void RetSetMap(SOCKET socket);
	// 添加用户
	void AddEmployees();

public:
	// 记录日志信息到界面
	void LogInfo(char* szMessage);
	// 有连接过来时响应
	LRESULT  OnAccept(WPARAM wParam,LPARAM lParam);
	// 登陆时响应
	LRESULT OnLogin(WPARAM wParam,LPARAM lParam);
	// 响应心跳
	LRESULT OnHeart(WPARAM wParam,LPARAM lParam);	
	// 响应某用户离线
	LRESULT OnClientLostTouch(WPARAM wParam,LPARAM lParam);

	// 根据用户ID获取用户状态
	const USER_STATUS& GetUserStatus(u_short userId) ;
	// 根据用户ID获取用户端口号
	const u_short& GetUserPort(u_short userId) ;
	// 根据用户ID获取用户IP
	const string &GetUserIp(u_short userId);
	// 获取用户ID到用户详细信息的map
	const map<u_short,UserInfo>&GetUsersMap()const;
};

