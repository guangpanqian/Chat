#include "StdAfx.h"
#include "Encrypter.h"
#include "..\include\Package\packetDef.h"
#include <iterator>

Encrypter::Encrypter(void)
{
	encryption_DES.SetKey((const unsigned char*)(DES_KEY),DES::KEYLENGTH);
	decryption_DES.SetKey((const unsigned char*)(DES_KEY),DES::KEYLENGTH);
}


Encrypter::~Encrypter(void)
{
}

void Encrypter::Encrypt(const string& szPlaintext,string&szCrpher )
{
	Process(szPlaintext,szCrpher,ENCRYPTION);
}

void Encrypter::Decrypt(const string&szCrpher, string& szPlaintext)
{
	Process(szCrpher,szPlaintext,DECRYPTION);
}

void Encrypter::Process(const string& strInput,string&strOutput,CipherDir cipherDir)
{
	BlockTransformation *pProcesser = NULL;

	if (ENCRYPTION == cipherDir)
		pProcesser = &encryption_DES;
	else
		pProcesser = &decryption_DES;

	unsigned char *pInput = new unsigned char[strInput.length()];
	memcpy(pInput,strInput.c_str(),strInput.length());

	int nBlockSize = strInput.length()/DES::BLOCKSIZE;
	int nProcessedLength = 0;
	for (int nBlockIndex = 0;nBlockIndex < nBlockSize;++nBlockIndex)
	{
		pProcesser->ProcessBlock(pInput + nBlockIndex*DES::BLOCKSIZE );
	}

	copy(pInput,pInput+strInput.length(),inserter(strOutput,strOutput.begin()));
	delete[] pInput;
}