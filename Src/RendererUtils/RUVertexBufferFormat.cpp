//==============================================================================
//
//		RUVertexBufferFormat.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/29/2007
//
//==============================================================================

#pragma once 

#pragma once

class RUVertexBufferFormat
{
private: 
	uint m_NumVertices;
	uint m_VertexSize;
	uint m_BufferSize;
	RVertexDesc m_pVertexDesc[MAX_VERTEX_DESC];
	IByteBufferPtr m_SrcData;
	
public: 
	RUVertexBufferFormat()
	{
	}

	~RUVertexBufferFormat()
	{
	}

public: 
};
