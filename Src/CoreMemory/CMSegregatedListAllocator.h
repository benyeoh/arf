//==============================================================================
//
//		CMSegregatedListAllocator.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/8/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

const static uint MAX_CHUNK_SIZE = 8192;
const static uint PAGE_SIZE = 4096*16;
const static uint BIN_SIZE_GRANULARITY = 8;
const static uint BIN_GRANULARITY_SHIFT = 3;
const static uint MAX_FREE_POOL_SIZE = 128;
const static uint NUM_OF_BINS = MAX_CHUNK_SIZE / BIN_SIZE_GRANULARITY;

struct CMMemPage;

struct CMMemChunk
{
	struct BookKeeping
	{
		CMMemPage* pBase;
	
#ifdef _DEBUG
		const wchar* pFile;
		uint numOfLines;
#endif
	};
	
	BookKeeping bookKeeping; 
	CMMemChunk* pNext;
};

struct CMMemPage
{
	uint chunkSize;
	uint numChunksAllocated;
	CMMemPage* pNext;
	CMMemPage* pPrev;
	CMMemChunk* pFree;
}; 

class CMSegregatedListAllocator : public CRefCounted<ICMMemAllocator>
{
private:
	CMMetrics m_Metrics;
	CMMemPage* m_pBins[NUM_OF_BINS];
	CMMemPage* m_pFreePool[MAX_FREE_POOL_SIZE];
	CMMemPage* m_pFull;

	uint m_CurPoolSize;
	
	Mutex	m_Lock;

public:
	CMSegregatedListAllocator();
	~CMSegregatedListAllocator();

private:
	CMMemPage* AllocPage(uint binIndex);
	void FreePage(CMMemPage* pPage);

	uint ChunkSizeToBin(size_t size);
	uint BinToChunkSize(uint binIndex);
	
public:
	void* Alloc(size_t size, const wchar* pFile, uint lineNum);
	void Free(void* pChunk);
	const CMMetrics& GetMetrics();
	
	void DumpLeaks();
};

_DECLARE_SMARTPTR(CMSegregatedListAllocator)

_NAMESPACE_END