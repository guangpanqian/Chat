#pragma once
#include <vector>
#include "Encrypter.h"
using namespace  std;

class ChatLogDealer
{
public:
	ChatLogDealer(u_short idMine,u_short idHis);
	~ChatLogDealer(void);
	// 保存聊天记录到文件
	void WriteChatLog(const CString&szChatLog);
	// 读取聊天记录到内存
	void ReadChatLog(CString &szChatLog);
	// 读取全部聊天记录
	void ReadAllChatLog(CString &szChatLog);

private:
	// 初始化聊天记录保存目录
	void InitFolderPath();
	// 从文件读取信息到内存
	void ReadBufferFromFile(const CString&szFileFullPath,CString&szBuffer);
	// 获取所有的聊天记录我呢见
	void FileAllChatLog2He(string path,vector<CString>&vFilePaths);
	// 当前用户的ID
	u_short m_idMine;
	// 聊天者的ID
	u_short m_idHis;
	// 根目录
	CString m_szFolderRoot;
	// 聊天记录加解密对象
	Encrypter m_encrypter;
};

