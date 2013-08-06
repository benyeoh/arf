//==============================================================================
//
//		CMSegregatedListAllocator.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/10/2008
//
//==============================================================================

#include "CoreMemoryFramework.h"

#undef malloc
#undef free
#undef realloc
#undef calloc

#include <malloc.h>

_NAMESPACE_BEGIN

CMSegregatedListAllocator::CMSegregatedListAllocator()
	: m_CurPoolSize(0)
	, m_pFull(NULL)
{
	_LOOPi(NUM_OF_BINS)
	{
		m_pBins[i] = NULL;
	}
	
	_LOOPi(MAX_FREE_POOL_SIZE)
	{
		m_pFreePool[i] = NULL;
	}
	
	memset(&m_Metrics, 0, sizeof(CMMetrics));

	m_Lock.Initialize();
}

CMSegregatedListAllocator::~CMSegregatedListAllocator()
{
	// TODO: Track stats here
	DumpLeaks();

	_LOOPi(NUM_OF_BINS)
	{
		CMMemPage* pPage = m_pBins[i];
		if(pPage)
		{
			::free(pPage);			
		}
	}
	
	CMMemPage* pFullPage = m_pFull;
	while(pFullPage)
	{
		CMMemPage* pToFree = pFullPage;
		pFullPage = pToFree->pNext;

		::free(pToFree);
	}

	_LOOPi(m_CurPoolSize)
	{
		_DEBUG_ASSERT(m_pFreePool[i]);		
		::free(m_pFreePool[i]);
	}
}

inline uint 
CMSegregatedListAllocator::ChunkSizeToBin(uint size)
{
	_DEBUG_ASSERT(size > 0);
	
	uint binIndex = (size-1) >> BIN_GRANULARITY_SHIFT;
	return binIndex;
}

inline uint 
CMSegregatedListAllocator::BinToChunkSize(uint binIndex)
{
	uint chunkSize = (binIndex+1) << BIN_GRANULARITY_SHIFT;
	return chunkSize;
}

CMMemPage* 
CMSegregatedListAllocator::AllocPage(uint binIndex)
{
	_DEBUG_ASSERT(binIndex >= 0 && binIndex < NUM_OF_BINS);
	CMMemPage *pPage = NULL;
	uint addressOffset = sizeof(CMMemPage);

	if(m_CurPoolSize)
	{
		// Allocate from a free pool
		pPage = m_pFreePool[--m_CurPoolSize];
	}
	else
	{
		// Allocated a page from the heap
		// TODO: Switch to use VirtualAlloc and equivalent
		pPage = (CMMemPage*) ::malloc(PAGE_SIZE+addressOffset);
		_DEBUG_ASSERT(pPage && IsAlignedToXBytes(pPage, 8));
		_INC_METRIC_NUM(totalPageSizeUsed, (PAGE_SIZE+addressOffset));
		_INC_METRIC_NUM(numPageAllocs, 1);
	}
	
	// Allocate chunk links
	pPage->chunkSize = BinToChunkSize(binIndex);
	CMMemChunk** ppToLink = &(pPage->pFree);
	uint currentChunkOffset = 0;
	byte* pPageData = (byte*)pPage;
	while(currentChunkOffset <= PAGE_SIZE-pPage->chunkSize)
	{
		CMMemChunk* pChunk = (CMMemChunk*)(pPageData + addressOffset + currentChunkOffset);
		*ppToLink = pChunk;
		ppToLink = &(pChunk->pNext);
		pChunk->bookKeeping.pBase = pPage;
#ifdef _DEBUG
		pChunk->bookKeeping.numOfLines = 0;
		pChunk->bookKeeping.pFile = NULL;
#endif
		currentChunkOffset += pPage->chunkSize;
	}
	
	*ppToLink = NULL;
	pPage->pNext = NULL;
	pPage->pPrev = NULL;
	pPage->numChunksAllocated = 0;
	
	return pPage;
}

void 
CMSegregatedListAllocator::FreePage(CMMemPage* pPage)
{
	_DEBUG_ASSERT(pPage);
	if(m_CurPoolSize < MAX_FREE_POOL_SIZE)
	{
		m_pFreePool[m_CurPoolSize] = pPage;
		m_CurPoolSize++;
	}
	else
	{
		// Deallocate the page
		::free(pPage);
		_DEC_METRIC_NUM(totalPageSizeUsed, (PAGE_SIZE+sizeof(CMMemPage)));
		_DEC_METRIC_NUM(numPageAllocs, 1);
	}
}

void* 
CMSegregatedListAllocator::Alloc(size_t size, const wchar* pFile, uint lineNum)
{	
	size_t totalSize = size + sizeof(CMMemChunk::BookKeeping);
	
	if(totalSize > MAX_CHUNK_SIZE)
	{
		// Begin Crit Section
		m_Lock.Lock();

		// Allocate from the heap
		// TODO: Switch to use VirtualAlloc and equivalent
		
		// Need to offset by 4 bytes here to make it aligned eventually on 8 bytes because
		// of the book-keeping info for a memchunk
		totalSize += sizeof(uint);
		CMMemChunk* pToReturn = (CMMemChunk*) (((byte*)::malloc(totalSize)) + sizeof(uint));
		byte* pToReturnData = ((byte*)pToReturn);
		*(uint*)(pToReturnData-sizeof(uint)) =  (uint) totalSize;
		
		pToReturn->bookKeeping.pBase = (CMMemPage*) ((size_t)0xDEADBEEF);
#ifdef _DEBUG
		pToReturn->bookKeeping.numOfLines = lineNum;
		pToReturn->bookKeeping.pFile = pFile;
#endif		
		_INC_METRIC_NUM(totalHeapSizeUsed, ((uint)totalSize));
		
		m_Lock.Unlock();

		_DEBUG_ASSERT(IsAlignedToXBytes(pToReturnData + sizeof(CMMemChunk::BookKeeping), 8));
		return (pToReturnData + sizeof(CMMemChunk::BookKeeping));
	}

	uint binIndex = ChunkSizeToBin(totalSize);
	
	// Begin Crit Section
	m_Lock.Lock();

	CMMemPage* pPage = m_pBins[binIndex];
	if(!pPage)
	{	
		pPage = AllocPage(binIndex);
		_DEBUG_ASSERT(pPage);
		m_pBins[binIndex] = pPage;
	}
	
	pPage->numChunksAllocated++;
	
	CMMemChunk* pChunk = pPage->pFree;
	pPage->pFree = pChunk->pNext;
	byte* pChunkData = (byte*)pChunk;
	
	// If there are no more free chunks
	if(!pPage->pFree)
	{
		// Remove from the bin
		m_pBins[binIndex] = pPage->pNext;
		if(m_pBins[binIndex])
			m_pBins[binIndex]->pPrev = NULL;

		// Add to full list for bookkeeping
		pPage->pNext = m_pFull;
		if(m_pFull)
			m_pFull->pPrev = pPage;		
		pPage->pPrev = NULL;
		m_pFull = pPage;
	}	
	
#ifdef _DEBUG
	pChunk->bookKeeping.numOfLines = lineNum;
	pChunk->bookKeeping.pFile = pFile;
#endif

	_INC_METRIC_NUM(totalChunkSizeUsed, pPage->chunkSize);
	_INC_METRIC_NUM(numChunkAllocs, 1);
	
	// End Crit Section
	m_Lock.Unlock();

	_DEBUG_ASSERT(IsAlignedToXBytes(pChunkData + sizeof(CMMemChunk::BookKeeping), 8));
	return (pChunkData + sizeof(CMMemChunk::BookKeeping));
}

void 
CMSegregatedListAllocator::Free(void* pChunk)
{
	CMMemChunk* pToFree = (CMMemChunk*) (((byte*)pChunk) - sizeof(CMMemChunk::BookKeeping));
	CMMemPage* pPage = pToFree->bookKeeping.pBase;
	if((size_t)pPage == 0xDEADBEEF)
	{
		// Begin Crit Section
		m_Lock.Lock();

		uint sizeToDealloc = (*(uint*)(((byte*)pToFree)-sizeof(uint)));
		::free( ((byte*)pToFree) - sizeof(uint));

		_DEC_METRIC_NUM(totalHeapSizeUsed, sizeToDealloc);

		m_Lock.Unlock();
		return;
	}
	
	uint chunkSize = pPage->chunkSize;
	
	// Begin Crit Section
	m_Lock.Lock();

	// Put the chunk at the head of the free list
	CMMemChunk* pNext = pPage->pFree;
	pToFree->pNext = pNext;
	pPage->pFree = pToFree;
		
#ifdef _DEBUG
	// Reset book-keeping information
	pToFree->bookKeeping.numOfLines = 0;
	pToFree->bookKeeping.pFile = NULL;
#endif
	
	// If the page was previously fully allocated
	if(!pNext)
	{	
		// Remove from the full list
		if(pPage->pPrev)
			pPage->pPrev->pNext = pPage->pNext;
		if(pPage->pNext)
			pPage->pNext->pPrev = pPage->pPrev;
		if(m_pFull == pPage)
			m_pFull = pPage->pNext;

		// Put it back in the head of the bin
		uint binIndex = ChunkSizeToBin(pPage->chunkSize);
		pPage->pPrev = NULL;
		pPage->pNext = m_pBins[binIndex];
		if(m_pBins[binIndex])
			m_pBins[binIndex]->pPrev = pPage;
		m_pBins[binIndex] = pPage;
	}
	
	pPage->numChunksAllocated--;
	
	
	// If the page has no more chunks allocated
	if(pPage->numChunksAllocated == 0)
	{
		// Free it only if there is a page in the same bin already
		uint binIndex = ChunkSizeToBin(pPage->chunkSize);
		if(m_pBins[binIndex] != pPage)
		{
			_DEBUG_ASSERT(pPage->pPrev);
			CMMemPage* pPrev = pPage->pPrev;
			CMMemPage* pNext = pPage->pNext;
			pPrev->pNext = pNext;
			if(pNext)
				pNext->pPrev = pPrev;				
			FreePage(pPage);			
		}
		else if(pPage->pNext)
		{
			m_pBins[binIndex] = pPage->pNext;
			m_pBins[binIndex]->pPrev = NULL;
			FreePage(pPage);
		}
	}
	
	_DEC_METRIC_NUM(numChunkAllocs, 1);
	_DEC_METRIC_NUM(totalChunkSizeUsed, chunkSize);

	// End Crit Section
	m_Lock.Unlock();
}

const CMMetrics& 
CMSegregatedListAllocator::GetMetrics()
{
	m_Metrics.numPagesFree = m_CurPoolSize;
	return m_Metrics;
}

void 
CMSegregatedListAllocator::DumpLeaks()
{
	_LOOPi(NUM_OF_BINS)
	{
		CMMemPage* pPage = m_pBins[i];
		if(pPage)
		{
#ifdef _DEBUG
			if(pPage->numChunksAllocated > 0)
			{
				uint offset = 0;
				byte* pPageData = (byte*)pPage;
				while(offset <= PAGE_SIZE-pPage->chunkSize)
				{
					CMMemChunk* pChunk = (CMMemChunk*)(pPageData + sizeof(CMMemPage) + offset);

					// Should output to appcallback here
					_DEBUG_ASSERT( !(pChunk->bookKeeping.pFile) );
					offset += pPage->chunkSize;
				}
			}
#endif		
		}
	}

	CMMemPage* pFullPage = m_pFull;
	while(pFullPage)
	{
#ifdef _DEBUG
		if(pFullPage->numChunksAllocated > 0)
		{
			uint offset = 0;
			byte* pPageData = (byte*)pFullPage;
			while(offset <= PAGE_SIZE-pFullPage->chunkSize)
			{
				CMMemChunk* pChunk = (CMMemChunk*)(pPageData + sizeof(CMMemPage) + offset);

				// Should output to appcallback here
				_DEBUG_ASSERT( !(pChunk->bookKeeping.pFile) );
				offset += pFullPage->chunkSize;
			}
		}
#endif		
		pFullPage = pFullPage->pNext;
	}

}

_NAMESPACE_END