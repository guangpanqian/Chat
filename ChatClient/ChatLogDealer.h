#pragma once
#include <vector>
#include "Encrypter.h"
using namespace  std;

class ChatLogDealer
{
public:
	ChatLogDealer(u_short idMine,u_short idHis);
	~ChatLogDealer(void);
	// ���������¼���ļ�
	void WriteChatLog(const CString&szChatLog);
	// ��ȡ�����¼���ڴ�
	void ReadChatLog(CString &szChatLog);
	// ��ȡȫ�������¼
	void ReadAllChatLog(CString &szChatLog);

private:
	// ��ʼ�������¼����Ŀ¼
	void InitFolderPath();
	// ���ļ���ȡ��Ϣ���ڴ�
	void ReadBufferFromFile(const CString&szFileFullPath,CString&szBuffer);
	// ��ȡ���е������¼���ؼ�
	void FileAllChatLog2He(string path,vector<CString>&vFilePaths);
	// ��ǰ�û���ID
	u_short m_idMine;
	// �����ߵ�ID
	u_short m_idHis;
	// ��Ŀ¼
	CString m_szFolderRoot;
	// �����¼�ӽ��ܶ���
	Encrypter m_encrypter;
};

