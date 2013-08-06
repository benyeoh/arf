//==============================================================================
//
//		CRIndexBuffer.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, March 25, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRIndexBuffer : public CRResource< IRIndexBuffer >
{
protected:
	uint m_NumIndices;
	uint m_StartIndex;
	uint m_Size;		
	eRBufferUsage m_Usage;
	
	uint m_TempStartIndex;
	uint m_TempNumIndices;
	
protected:
	CRRenderer* m_pRenderer;
	
public:
	CRIndexBuffer(CRRenderer* pRenderer)	
		: m_NumIndices(0)
		, m_StartIndex(0)
		, m_Size(0)
		, m_TempNumIndices(0)
		, m_TempStartIndex(0)
		, m_Usage(BUF_DEFAULT)
		, m_pRenderer(pRenderer)
	{		
	}
	
	virtual ~CRIndexBuffer()
	{
	}
	
private:
	void SetSize(uint size) { m_Size = size; }
	void SetNumIndices(uint numIndices) { m_NumIndices = numIndices; }
	void SetUsage(eRBufferUsage usage) { m_Usage = usage; }
	void SetStartIndex(uint startIndex) { m_StartIndex = startIndex; }
	
protected:
	_PURE( boolean DoUnlock() )
	_PURE( ushort* DoLock(uint startIndex, uint numIndices) )	
		
public:
	void Initialize(uint size, 
					eRBufferUsage usage);	
	
	ushort* Lock(uint startIndexToWrite, uint numIndicesToWrite);
	boolean Unlock(boolean isSetValidRange=TRUE);
	
	void SetValidRange(uint startIndex, uint numIndices);	

	uint GetSize() const { return m_Size; }
	uint GetNumIndices() const { return m_NumIndices; }
	uint GetStartIndex() const { return m_StartIndex; }
	eRBufferUsage GetUsage() const { return m_Usage; }
};

_NAMESPACE_END