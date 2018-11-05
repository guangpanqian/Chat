#pragma once
#include <string>
#include "..\include\CryptoPP\des.h"
using namespace CryptoPP;
using namespace  std;

// 内容加密类
class Encrypter
{
public:
	Encrypter(void);
	~Encrypter(void);
	// 加密
	void Encrypt(const string& szPlaintext,string&szCrpher );
	// 解密
	void Decrypt(const string&szCrpher, string& szPlaintext);
private:
	void Process(const string& szPlaintext,string&szCrpher,CipherDir cipherDir );
	unsigned char Key[DES::DEFAULT_KEYLENGTH];
	DESEncryption encryption_DES;
	DESDecryption decryption_DES;
};

