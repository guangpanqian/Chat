
// ChildView.h : CChildView ��Ľӿ�
//


#pragma once
#include "EmployeeDBOperator.h"
#include <map>
#include "..\include\Package\packetDef.h"
#include "UserInfo.h"
#include "CommonDef.h"
using namespace std;

// CChildView ����
class CChildView : public CWnd
{
// ����
public:
	CChildView();
// ����
public:

// ����
public:

// ��д
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CChildView();

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelChangeTab(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

private:
	CListCtrl   m_lstCtlUsers;
	CListCtrl   m_lstCtlLog;
	CTabCtrl   m_tabCtl;
	CFont       m_font;
	map<u_short,int>m_employee2ListIndex;
	map<u_short,UserInfo>m_employees;
	map<SOCKET,u_short>m_mpSocket2Id;

	CBitmap m_bitmaps[2];
	CImageList m_ImageList;

private:
	void CreateChildren();
	void GetEmployee();
	void CheckClientLostTouch();
	void SetOffLine(u_short userId);
	void RetSetMap(SOCKET socket);
	void AddEmployees();

public:
	void LogInfo(char* szMessage);
	LRESULT  OnAccept(WPARAM wParam,LPARAM lParam);
	LRESULT OnLogin(WPARAM wParam,LPARAM lParam);
	LRESULT OnHeart(WPARAM wParam,LPARAM lParam);	
	LRESULT OnClientLostTouch(WPARAM wParam,LPARAM lParam);

	const USER_STATUS& GetUserStatus(u_short userId) ;
	const u_short& GetUserPort(u_short userId) ;
	const string &GetUserIp(u_short userId);
	const map<u_short,UserInfo>&GetUsersMap()const;
};

