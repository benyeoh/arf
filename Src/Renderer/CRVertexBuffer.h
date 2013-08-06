//==============================================================================
//
//		CRVertexBuffer.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, March 25, 2006
//
//==============================================================================

#pragma once 


_NAMESPACE_BEGIN

class CRVertexBuffer : public CRResource< IRVertexBuffer >
{	
protected:
	uint m_VertexSize;
	uint m_NumVertices;
	uint m_StartVertex;
	RVertexDesc m_pVertexDesc[MAX_VERTEX_DESC];
	eRBufferUsage m_Usage;
	uint m_Size;
	
	uint m_TempStartVertex;
	uint m_TempNumVertices; 
	
protected:
	CRRenderer* m_pRenderer;
	
public:
	CRVertexBuffer(CRRenderer* pRenderer)
		: m_VertexSize(0)
		, m_StartVertex(0)
		, m_Size(0)
		, m_NumVertices(0)
		, m_TempStartVertex(0)
		, m_TempNumVertices(0)
		, m_Usage(BUF_DEFAULT)
		, m_pRenderer(pRenderer)
	{
		m_pVertexDesc[0].usage = VDU_END;
		m_pVertexDesc[0].usageIndex = 0;
	}
	
	virtual ~CRVertexBuffer()
	{
	}
	
private:
	void SetDescription(const RVertexDesc* pDesc);	
	void SetUsage(eRBufferUsage usage) { m_Usage = usage; }
	void SetNumVertices(uint numVertices) { m_NumVertices =  numVertices; }
	void SetSize(uint size) { m_Size = size; }
	void SetStartVertex(uint startVertex) { m_StartVertex = startVertex; }
	
protected:
	_PURE( byte* DoLock(uint startVertexToWrite, uint numVerticesToWrite) )
	_PURE( boolean DoUnlock() )
	_PURE( byte* DoLockImmediate(uint startVertexToWrite, uint numVerticesToWrite) )
	_PURE( boolean DoUnlockImmediate() )
	
public:
	void Initialize(const RVertexDesc* pDesc,
					uint numVertices,
					eRBufferUsage usage);
	
	void SetValidRange(uint startVertex, uint numVertices);
	
	byte* Lock(uint startVertexToWrite, uint numVerticesToWrite);
	boolean Unlock(boolean isSetValidRange=TRUE);

	// These are for write only updates
	// Values written using these are not available for read back
	byte* LockImmediate(uint startVertexToWrite, uint numVerticesToWrite);
	boolean UnlockImmediate(boolean isSetValidRange=TRUE);
	
	uint GetVertexSize() const { return m_VertexSize; }
	uint GetSize() const { return m_Size; }
	uint GetNumVertices() const { return m_NumVertices; }
	uint GetStartVertex() const { return m_StartVertex; }	
	eRBufferUsage GetUsage() const { return m_Usage; }

	const RVertexDesc* GetDescription() const ;
};


_DECLARE_SMARTPTR(CRVertexBuffer)

_NAMESPACE_END