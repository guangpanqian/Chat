#pragma once
#include <ObjBase.h>
#include <list>
#include <map>
using namespace  std;

// ���ݰ�����
interface ISocketObserver
{
	virtual BOOL  RecvPacket(PPACKET_HDR packet) = 0;
};

