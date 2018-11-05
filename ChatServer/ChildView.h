
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
	// �û��б�
	CListCtrl   m_lstCtlUsers;
	// ��־�б�
	CListCtrl   m_lstCtlLog;
	// �л��б��tab�ؼ�
	CTabCtrl   m_tabCtl;
	//	��ʾ����
	CFont       m_font;
	// userid ���б��е�ӳ��
	map<u_short,int>m_employee2ListIndex;
	// userid ���û���Ϣ��ӳ��
	map<u_short,UserInfo>m_employees;
	// SOCKET ���û�ID��ӳ��
	map<SOCKET,u_short>m_mpSocket2Id;
	// ���ߣ�����״̬ͼƬ
	CBitmap m_bitmaps[2];
	CImageList m_ImageList;

private:
	// �����ӿؼ�
	void CreateChildren();
	// ��ȡ�û��б�
	void GetEmployee();
	// ����Ƿ����û�����
	void CheckClientLostTouch();
	// ���ø��û�״̬Ϊ����
	void SetOffLine(u_short userId);
	// ����map״̬
	void RetSetMap(SOCKET socket);
	// ����û�
	void AddEmployees();

public:
	// ��¼��־��Ϣ������
	void LogInfo(char* szMessage);
	// �����ӹ���ʱ��Ӧ
	LRESULT  OnAccept(WPARAM wParam,LPARAM lParam);
	// ��½ʱ��Ӧ
	LRESULT OnLogin(WPARAM wParam,LPARAM lParam);
	// ��Ӧ����
	LRESULT OnHeart(WPARAM wParam,LPARAM lParam);	
	// ��Ӧĳ�û�����
	LRESULT OnClientLostTouch(WPARAM wParam,LPARAM lParam);

	// �����û�ID��ȡ�û�״̬
	const USER_STATUS& GetUserStatus(u_short userId) ;
	// �����û�ID��ȡ�û��˿ں�
	const u_short& GetUserPort(u_short userId) ;
	// �����û�ID��ȡ�û�IP
	const string &GetUserIp(u_short userId);
	// ��ȡ�û�ID���û���ϸ��Ϣ��map
	const map<u_short,UserInfo>&GetUsersMap()const;
};

