
// ChatServer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ChatServer.h"
#include "MainFrm.h"
#include "ConnectionPool.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChatServerApp

BEGIN_MESSAGE_MAP(CChatServerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CChatServerApp::OnAppAbout)
END_MESSAGE_MAP()


// CChatServerApp ����

CChatServerApp::CChatServerApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("ChatServer.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	pEmployeeDBOperator = NULL;
}

// Ψһ��һ�� CChatServerApp ����

CChatServerApp theApp;


// CChatServerApp ��ʼ��

BOOL CChatServerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	InitializeCriticalSection(&ConnPool::cslock);
	InitializeCriticalSection(&ConnPool::csInstance);

	// �������ݿ���������������Ӧ���ȴ���������ķ�����Ҫ�õ�����������ݿ�Ķ���
	pEmployeeDBOperator = new EmployeeDBOperator();

	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// ���������ؿ�ܼ�����Դ
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pFrame->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	m_pMainWnd->SetIcon( LoadIcon(IDR_MAINFRAME),FALSE);
	pFrame->SetWindowText("ͨѶ������");

	// ��������
	((CMainFrame*)(theApp.m_pMainWnd))->LogInfo("**********��������ʼ**********");
	socketServer.StartWork();
	((CMainFrame*)(theApp.m_pMainWnd))->LogInfo("**********�����������**********");

	return TRUE;
}

int CChatServerApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ

	// ����Ӧ����ֹͣ���������ٷ������ݿ�Ķ�����Ϊ����������ڷ������ݿ�
	socketServer.StopWork();
	delete pEmployeeDBOperator;

	// �������ݿ����ӳ�
	delete ConnPool::GetInstance();

	// �����ٽ�������
	DeleteCriticalSection(&ConnPool::cslock);
	DeleteCriticalSection(&ConnPool::csInstance);

	return CWinApp::ExitInstance();
}

// CChatServerApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CChatServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CChatServerApp ��Ϣ�������



