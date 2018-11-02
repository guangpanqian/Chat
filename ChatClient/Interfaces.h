#pragma once
#include <ObjBase.h>
#include <list>
#include <map>
using namespace  std;

// 数据包到了
interface ISocketObserver
{
	virtual BOOL  RecvPacket(PPACKET_HDR packet) = 0;
};

