#include "StdAfx.h"
#include "ChatLogDealer.h"
#include <io.h>
#include <iterator>


ChatLogDealer::ChatLogDealer(u_short idMine,u_short idHis)
	:m_idMine(idMine),
	m_idHis(idHis)
{
	InitFolderPath();
}


ChatLogDealer::~ChatLogDealer(void)
{

}

void ChatLogDealer::InitFolderPath()
{
	m_szFolderRoot.Format("%d",m_idMine);
	if (!::PathFileExists(m_szFolderRoot))
		::CreateDirectory(m_szFolderRoot,NULL);
}

void ChatLogDealer::WriteChatLog(const CString&szChatLog)
{
	
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	CString szFolderName;
	szFolderName.Format("%s\\%04d%02d%02d",m_szFolderRoot,systemTime.wYear,systemTime.wMonth,systemTime.wDay);
	if (!::PathFileExists(szFolderName))
		::CreateDirectory(szFolderName,NULL);

	CString szFileFullName;
	szFileFullName.Format("%s\\%d.dat",szFolderName,m_idHis);
	try
	{
		CFile  FileChatLog;
		string strPlainText;
		strPlainText = szChatLog.GetString();
		string strCrpher;
		m_encrypter.Encrypt(strPlainText,strCrpher);

		if (FileChatLog.Open(szFileFullName,CFile::modeWrite|CFile::modeCreate))
		{
			FileChatLog.Write(strCrpher.c_str(),strCrpher.length()+1);
			FileChatLog.Close();
		}

	}
	catch (CMemoryException* e){
	}
	catch (CFileException* e){
	}
	catch (CException* e){
	}
}


void ChatLogDealer::ReadChatLog(CString &szChatLog)
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	CString szFileFullName;
	szFileFullName.Format("%s\\%04d%02d%02d\\%d.dat",m_szFolderRoot,systemTime.wYear,systemTime.wMonth,systemTime.wDay,m_idHis);
	ReadBufferFromFile(szFileFullName,szChatLog);
	
}

void GetDateFromFilePath(const CString&strFilePath,CString& strDate)
{
	int index = strFilePath.ReverseFind('\\');
	int startIndex = index - 8;
	int endIndex = index - 1;
	strDate = strFilePath.Mid(startIndex,8);
	strDate = CString("----------------------")+strDate.Left(4) +"Äê"+strDate.Mid(4,2)+"ÔÂ"+strDate.Right(2)+"ÈÕ----------------------";
}

void ChatLogDealer::ReadAllChatLog(CString &szChatLog)
{
	vector<CString>vAllFolders;
	string path = m_szFolderRoot.GetString();
	FileAllChatLog2He(path,vAllFolders);

	vector<CString>::iterator first = vAllFolders.begin();
	vector<CString>::iterator last = vAllFolders.end();
	CString strLogDate;
	for(;first!=last;++first)
	{
		GetDateFromFilePath(*first,strLogDate);
		szChatLog =  szChatLog +strLogDate+"\r\n";
		ReadBufferFromFile(*first,szChatLog);
	}
}

void ChatLogDealer::ReadBufferFromFile(const CString&szFileFullPath,CString&szBuffer)
{
	try
	{
		CFile  FileChatLog;

		string strPlainText;
		string strCrpher;
	
	
		if (FileChatLog.Open(szFileFullPath,CFile::modeRead))
		{
			CString strLine;
			int nFileLength = FileChatLog.GetLength() + 1;
			unsigned char *szCrpher = new unsigned char[nFileLength];
			ZeroMemory(szCrpher,nFileLength);

			while(FileChatLog.Read(szCrpher,nFileLength))
			{
				copy(szCrpher,szCrpher+nFileLength,inserter(strCrpher,strCrpher.begin()));
				m_encrypter.Decrypt(strCrpher,strPlainText);
				szBuffer = szBuffer + strPlainText.c_str() + "\n";
			}
			delete []szCrpher;
			FileChatLog.Close();
		}
	}catch (CMemoryException* e){}
	catch (CFileException* e){}
	catch (CException* e){}
}

void ChatLogDealer::FileAllChatLog2He(string path,vector<CString>&files)
{
	long   hFile   =   0;  
	struct _finddata_t fileinfo;  
	string p;  
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
	{  
		do  
		{  
			if((fileinfo.attrib &  _A_SUBDIR))  
			{  
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
					FileAllChatLog2He( p.assign(path).append("\\").append(fileinfo.name), files );  
			}  
			else  
			{  
				if  (atol(fileinfo.name) == m_idHis)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name).c_str());  
				}
			}  
		}while(_findnext(hFile, &fileinfo)  == 0);  
		_findclose(hFile);  
	}  
}