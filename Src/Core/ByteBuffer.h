//==============================================================================
//
//		ByteBuffer.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, April 24, 2006
//
//==============================================================================

#pragma once 


_NAMESPACE_BEGIN

struct IByteBuffer : public IRefCounted
{
	_PURE( void ResizeBuffer(uint bufferSize) )
	_PURE( byte* GetData() )
	_PURE( const byte* GetData() const )
	_PURE( uint GetBufferSize() const )
	_PURE( uint GetDataLength() const )
	_PURE( void SetDataLength(uint length) )
};

_DECLARE_SMARTPTR(IByteBuffer)

class CByteBuffer : public CRefCounted< IByteBuffer >
{
private:
	byte* m_pData;
	uint m_DataLen;
	uint m_BufferSize;
	
public:
	CByteBuffer()
		: m_pData(NULL)
		, m_BufferSize(0)
		, m_DataLen(0)
	{
	}
	
	CByteBuffer(uint bufferSize)
		: m_pData(NULL)
		, m_BufferSize(0)
		, m_DataLen(0)
	{
		ResizeBuffer(bufferSize);
	}

	virtual ~CByteBuffer()
	{
		_DELETE_ARRAY(m_pData);
	}	
	
public:
	void ResizeBuffer(uint bufferSize)
	{
		byte* pData = _NEW byte[bufferSize];
		_DEBUG_ASSERT(pData);
		
		uint sizeToCopy = 0;
		if(m_pData)
		{
			sizeToCopy = bufferSize < m_DataLen ? bufferSize : m_DataLen;
			memcpy(pData, m_pData, sizeToCopy);
			
			_DELETE_ARRAY(m_pData);
		}
		
		m_pData = pData;
		
		m_BufferSize = bufferSize;
		m_DataLen = sizeToCopy;
	}
	
	const byte* GetData() const { return m_pData; }
	byte* GetData() { return m_pData; }
	uint GetBufferSize() const { return m_BufferSize; }
	uint GetDataLength() const { return m_DataLen; }
	void SetDataLength(uint length) { m_DataLen = length; }
};


_NAMESPACE_END