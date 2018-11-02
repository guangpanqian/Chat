#pragma once
#include <vector>
#include "Encrypter.h"
using namespace  std;

class ChatLogDealer
{
public:
	ChatLogDealer(u_short idMine,u_short idHis);
	~ChatLogDealer(void);
	void WriteChatLog(const CString&szChatLog);
	void ReadChatLog(CString &szChatLog);
	void ReadAllChatLog(CString &szChatLog);

private:
	void InitFolderPath();
	void ReadBufferFromFile(const CString&szFileFullPath,CString&szBuffer);
	void FileAllChatLog2He(string path,vector<CString>&vFilePaths);
	u_short m_idMine;
	u_short m_idHis;
	CString m_szFolderRoot;
	Encrypter m_encrypter;
};

