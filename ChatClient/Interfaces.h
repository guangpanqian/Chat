#pragma once
#include <ObjBase.h>
#include <list>
#include <map>
using namespace  std;

// ���ݰ����ˣ��������ݵ���̳д˽ӿ�
interface ISocketObserver
{
	virtual BOOL  RecvPacket(PPACKET_HDR packet) = 0;
};

