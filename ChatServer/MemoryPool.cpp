#include "StdAfx.h"
#include "MemoryPool.h"


CRITICAL_SECTION MemoryPool::csInstance;

void MemoryPool::InitCriticalSection()
{
	::InitializeCriticalSection(&csInstance);
}

void MemoryPool::DeleteCriticalSection()
{
	::DeleteCriticalSection(&csInstance);
}

MemoryPool::MemoryPool(void)
{
}


MemoryPool::~MemoryPool(void)
{
}
