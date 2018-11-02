#pragma once
#include <map>
using namespace std;

typedef union _free_list
{
	char *pData;
	union _free_list *pNext;
}FREE_LIST,*PFREE_LIST;

class MemoryPool
{
public:
	~MemoryPool(void);
	MemoryPool *GetInstance();
	char *Alloc(int nAllocSize);
	void Free(char* pBuf);

	static void InitCriticalSection();
	static void DeleteCriticalSection();
	static CRITICAL_SECTION csInstance;

protected:

private:
	MemoryPool(void);
	void InitMemoryPool();
	void DestoryMemoryPool();

	PFREE_LIST pFreeList[16];
	char *pPool;
	int nPoolStart;
	int nPoolEnd;
	map<char*,int>mpMemoryAlloced;
	CRITICAL_SECTION csMemory;
};

