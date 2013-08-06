//==============================================================================
//
//		CRIndexBuffer.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Wednesday, April 05, 2006
//
//==============================================================================

#include "RendererFramework.h"

_NAMESPACE_BEGIN

ushort* 
CRIndexBuffer::Lock(uint startIndexToWrite, uint numIndicesToWrite)
{
	ushort* pData = DoLock(startIndexToWrite, numIndicesToWrite);
		
	if(pData)
	{
		m_TempStartIndex = startIndexToWrite;
		m_TempNumIndices = numIndicesToWrite;
	}
	
	return pData;
}

boolean 
CRIndexBuffer::Unlock(boolean isSetValidRange)
{
	if(DoUnlock())
	{
		if(isSetValidRange && m_TempNumIndices > 0)
		{
			SetValidRange(m_TempStartIndex, m_TempNumIndices);
		}
		
		return TRUE;
	}
	
	return FALSE;
}

void 
CRIndexBuffer::Initialize(uint size, 
						eRBufferUsage usage)
{
	SetUsage(usage);
	SetSize(size);
	SetValidRange(0, 0);
}

void
CRIndexBuffer::SetValidRange(uint startIndex, uint numIndices)
{
	SetStartIndex(startIndex);
	SetNumIndices(numIndices);
}			

_NAMESPACE_END