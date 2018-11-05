#pragma once
#include <ObjBase.h>
#include <list>
#include <map>
using namespace  std;

// 数据包到了，接收数据的类继承此接口
interface ISocketObserver
{
	virtual BOOL  RecvPacket(PPACKET_HDR packet) = 0;
};

