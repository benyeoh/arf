//==============================================================================
//
//		CRVertexBufferGroupD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 23, 2006
//
//==============================================================================

#include "RendererD3D.h"


_NAMESPACE_BEGIN

uint
CRVertexBufferGroupD3D::SetVertexElement(D3DVERTEXELEMENT9& vertElem, const RVertexDesc& desc)
{
	uint typeOffset = 0;
	
	switch(desc.usage)
	{
		case VDU_POSITION:		
			vertElem.Type = D3DDECLTYPE_FLOAT3; typeOffset+=12; 
			vertElem.Usage = D3DDECLUSAGE_POSITION;	
			break;
		case VDU_NORMAL:		
			vertElem.Type = D3DDECLTYPE_FLOAT3; typeOffset+=12; 
			vertElem.Usage = D3DDECLUSAGE_NORMAL;	
			break;
		case VDU_TEXCOORDF2:		
			vertElem.Type = D3DDECLTYPE_FLOAT2; typeOffset+=8;
			vertElem.Usage = D3DDECLUSAGE_TEXCOORD;	
			break;
		case VDU_BINORMAL:		
			vertElem.Type = D3DDECLTYPE_FLOAT3; typeOffset+=12;
			vertElem.Usage = D3DDECLUSAGE_BINORMAL;	
			break;
		case VDU_TANGENT:		
			vertElem.Type = D3DDECLTYPE_FLOAT3; typeOffset+=12;
			vertElem.Usage = D3DDECLUSAGE_TANGENT;		
			break;
		case VDU_BLENDINDICES:	
			vertElem.Type = D3DDECLTYPE_UBYTE4; typeOffset+=4;
			vertElem.Usage = D3DDECLUSAGE_BLENDINDICES;	
			break;
		case VDU_BLENDWEIGHT:	
			vertElem.Type = D3DDECLTYPE_UBYTE4N; typeOffset+=4; 
			vertElem.Usage = D3DDECLUSAGE_BLENDWEIGHT;	
			break;
		case VDU_COLOR:
			vertElem.Type = D3DDECLTYPE_D3DCOLOR; typeOffset+=4; 
			vertElem.Usage = D3DDECLUSAGE_COLOR;	
			break;
		case VDU_TEXCOORDF1:
			vertElem.Type = D3DDECLTYPE_FLOAT1; typeOffset+=4;
			vertElem.Usage = D3DDECLUSAGE_TEXCOORD;
			break;
		case VDU_TEXCOORDF3:
			vertElem.Type = D3DDECLTYPE_FLOAT3; typeOffset+=12;
			vertElem.Usage = D3DDECLUSAGE_TEXCOORD;
			break;
		case VDU_TEXCOORDF4:
			vertElem.Type = D3DDECLTYPE_FLOAT4; typeOffset+=16;
			vertElem.Usage = D3DDECLUSAGE_TEXCOORD;		
			break;
									
		default: _DEBUG_ASSERT(FALSE); break;
	}
	
	return typeOffset;
}

IDirect3DVertexDeclaration9*
CRVertexBufferGroupD3D::CacheOneInstancedVertexDeclaration(const RVertexDesc* pInstDesc)
{
	// Allocate the new vertex elements
	D3DVERTEXELEMENT9 vertElems[MAX_VERTEX_DESC];
	
	uint curElemIndex = 0;
	
	uint size = (uint)(m_VBs.size());
	
	// For each vertex buffer
	_LOOPi( size )
	{
		CRVertexBufferD3D* pCurVB = (CRVertexBufferD3D*)(m_VBs[i].GetPtr());
		const RVertexDesc* pCurDesc = pCurVB->GetDescription();

		_DEBUG_ASSERT(pCurDesc);
		
		uint j = 0;
		uint typeOffset = 0;
		
		// Convert the vertex description into D3DVERTEXELEMENT9
		while( pCurDesc[j].usage != VDU_END )
		{
			_DEBUG_ASSERT(curElemIndex < MAX_VERTEX_DESC);
			
			vertElems[curElemIndex].Method = D3DDECLMETHOD_DEFAULT;
			vertElems[curElemIndex].Stream = i;
			vertElems[curElemIndex].Offset = typeOffset;
			vertElems[curElemIndex].UsageIndex = pCurDesc[j].usageIndex;
			
			typeOffset += SetVertexElement(vertElems[curElemIndex], pCurDesc[j]);
			
			++j;
			++curElemIndex;
		}
	}
	
	// Cache for one instance
	const RVertexDesc* pCurDesc = pInstDesc;
	
	uint j				= 0;
	uint typeOffset		= 0;
	uint origElemIndex	= curElemIndex;
	
	// Convert the vertex description into D3DVERTEXELEMENT9
	while( pCurDesc[j].usage != VDU_END )
	{
		_DEBUG_ASSERT(curElemIndex < MAX_VERTEX_DESC);
		
		vertElems[curElemIndex].Method = D3DDECLMETHOD_DEFAULT;
		vertElems[curElemIndex].Stream = size;			// Set to the last stream index
		vertElems[curElemIndex].Offset = typeOffset;
		vertElems[curElemIndex].UsageIndex = pCurDesc[j].usageIndex;
		
		typeOffset += SetVertexElement(vertElems[curElemIndex], pCurDesc[j]);
		
		++j;
		++curElemIndex;
	}
	
	if(curElemIndex > 0)
	{	
		vertElems[curElemIndex].Stream		= 0xFF;
		vertElems[curElemIndex].Offset		= 0;
		vertElems[curElemIndex].Type		= D3DDECLTYPE_UNUSED;
		vertElems[curElemIndex].Method		= 0;
		vertElems[curElemIndex].Usage		= 0;
		vertElems[curElemIndex].UsageIndex	= 0;
		
		// Create the vertex declaration
		IDirect3DVertexDeclaration9* pDecl = NULL;
		_DEBUG_ASSERT(m_pRenderer);
		
		IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);
		
		HRESULT hr = pDevice->CreateVertexDeclaration( vertElems, &pDecl ); _DEBUG_ASSERT( SUCCEEDED(hr) );
		return pDecl;
	}
	
	return NULL;
}

void
CRVertexBufferGroupD3D::CacheVertexDeclaration()
{
	// Allocate the new vertex elements
	D3DVERTEXELEMENT9 vertElems[MAX_VERTEX_DESC];
	
	uint curElemIndex = 0;
	
	uint size = (uint)(m_VBs.size());
	
	// For each vertex buffer
	_LOOPi( size )
	{
		CRVertexBufferD3D* pCurVB = (CRVertexBufferD3D*)(m_VBs[i].GetPtr());
		const RVertexDesc* pCurDesc = pCurVB->GetDescription();

		_DEBUG_ASSERT(pCurDesc);
		
		uint j = 0;
		uint typeOffset = 0;
		
		// Convert the vertex description into D3DVERTEXELEMENT9
		while( pCurDesc[j].usage != VDU_END )
		{
			_DEBUG_ASSERT(curElemIndex < MAX_VERTEX_DESC);
			
			vertElems[curElemIndex].Method = D3DDECLMETHOD_DEFAULT;
			vertElems[curElemIndex].Stream = i;
			vertElems[curElemIndex].Offset = typeOffset;
			vertElems[curElemIndex].UsageIndex = pCurDesc[j].usageIndex;
			
			typeOffset += SetVertexElement(vertElems[curElemIndex], pCurDesc[j]);
			
			++j;
			++curElemIndex;
		}
	}

	// Clear the current declaration
	m_pDecl = NULL;
	
	// Get declaration if valid
	if(curElemIndex > 0)
	{	
		vertElems[curElemIndex].Stream		= 0xFF;
		vertElems[curElemIndex].Offset		= 0;
		vertElems[curElemIndex].Type		= D3DDECLTYPE_UNUSED;
		vertElems[curElemIndex].Method		= 0;
		vertElems[curElemIndex].Usage		= 0;
		vertElems[curElemIndex].UsageIndex	= 0;
		
		// Create the vertex declaration
		IDirect3DVertexDeclaration9* pDecl = NULL;
		_DEBUG_ASSERT(m_pRenderer);
		
		IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);
		
		// TODO: Cache this declaration in the resource mgr
		HRESULT hr = pDevice->CreateVertexDeclaration( vertElems, &pDecl ); _DEBUG_ASSERT( SUCCEEDED(hr) );
		m_pDecl = pDecl;
		_RELEASE(pDecl);
	}
	
	// Cache for all instances
	_LOOPi((uint) m_CachedInstancedDecl.size())
	{
		VBDeclData& data = m_CachedInstancedDecl[i];
		
		const RVertexDesc* pCurDesc = data.desc;
		
		uint j				= 0;
		uint typeOffset		= 0;
		uint origElemIndex	= curElemIndex;
		
		// Convert the vertex description into D3DVERTEXELEMENT9
		while( pCurDesc[j].usage != VDU_END )
		{
			_DEBUG_ASSERT(curElemIndex < MAX_VERTEX_DESC);
			
			vertElems[curElemIndex].Method = D3DDECLMETHOD_DEFAULT;
			vertElems[curElemIndex].Stream = size;			// Set to the last stream index
			vertElems[curElemIndex].Offset = typeOffset;
			vertElems[curElemIndex].UsageIndex = pCurDesc[j].usageIndex;
			
			typeOffset += SetVertexElement(vertElems[curElemIndex], pCurDesc[j]);
			
			++j;
			++curElemIndex;
		}
		
		if(curElemIndex > 0)
		{	
			vertElems[curElemIndex].Stream		= 0xFF;
			vertElems[curElemIndex].Offset		= 0;
			vertElems[curElemIndex].Type		= D3DDECLTYPE_UNUSED;
			vertElems[curElemIndex].Method		= 0;
			vertElems[curElemIndex].Usage		= 0;
			vertElems[curElemIndex].UsageIndex	= 0;
			
			// Create the vertex declaration
			IDirect3DVertexDeclaration9* pDecl = NULL;
			_DEBUG_ASSERT(m_pRenderer);
			
			IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);
			
			HRESULT hr = pDevice->CreateVertexDeclaration( vertElems, &pDecl ); _DEBUG_ASSERT( SUCCEEDED(hr) );
			data.pDecl = pDecl;
			_RELEASE(pDecl);
		}
		
		curElemIndex = origElemIndex;
	}
}

void 
CRVertexBufferGroupD3D::DoNotifyAddedVertexBuffer(CRVertexBuffer* pVB)
{
	CacheVertexDeclaration();
}

void 
CRVertexBufferGroupD3D::DoNotifyRemovedVertexBuffer(CRVertexBuffer* pVB)
{
	CacheVertexDeclaration();	
}

void 
CRVertexBufferGroupD3D::SetInstancedVB(uint instParamsSize, RVertexDesc* pDesc, uint* pDescPacked, uint vbDescHash)
{
	// TODO: Reset the size if the vb is too small
	// Set the instanced VB
	uint* pVBIndex = m_CachedInstancedVBIndices.Get(instParamsSize);
	if(!pVBIndex)
	{
		// Create a VB with the same size (if it does not already exist)
		uint crcData[2];
		crcData[0] = _MAKE_ID('i','n','s','t');
		crcData[1] = instParamsSize;
		
		CRCDataKey instVBKey;
		instVBKey.Set(crcData, sizeof(uint) * 2);
		CRVertexBufferD3D* pInstVB = (CRVertexBufferD3D*)
			m_pRenderer->GetCRResourceMgr().CreateVertexBuffer(&instVBKey, pDesc, MAX_SIZE_INSTANCED_VB, BUF_DYNAMIC);
		
		m_CachedInstancedVB.push_back(pInstVB);
		
		// We use the size as the key since we want to share
		// buffers that are the same size
		m_CachedInstancedVBIndices.Add(instParamsSize, (uint) (m_CachedInstancedVB.size())-1);
		
		m_pInstancedVB = pInstVB;
	}
	else
	{
		m_pInstancedVB = (CRVertexBufferD3D*) ((CRVertexBuffer*)m_CachedInstancedVB[*pVBIndex]);
	}
	
	// Set the declaration
	VBDescKey descKey;
	descKey.descHash = vbDescHash;
	descKey.descPacked[0] = pDescPacked[0];
	descKey.descPacked[1] = pDescPacked[1];
	descKey.descPacked[2] = pDescPacked[2];
	descKey.descPacked[3] = pDescPacked[3];
	
	uint* pDeclIndex = m_CachedInstancedDeclIndices.Get(descKey);
	if(!pDeclIndex)
	{
		// Create a new declaration
		IDirect3DVertexDeclaration9* pD3DDecl = CacheOneInstancedVertexDeclaration(pDesc);
		
		// Set the declaration data
		VBDeclData declData;
	
		uint i = 0;
		while(pDesc[i].usage != VDU_END)
		{
			declData.desc[i] = pDesc[i];
			i++;
		}
		
		declData.desc[i].usage		= VDU_END;
		declData.desc[i].usageIndex = 0xFF;
		
		declData.pDecl = pD3DDecl;					
		
		m_CachedInstancedDecl.push_back(declData);	
		
		// Then add the index mapping
		m_CachedInstancedDeclIndices.Add(descKey, ((uint)m_CachedInstancedDecl.size())-1);
					
		m_pInstancedDecl = pD3DDecl;
		
		_RELEASE(pD3DDecl);
	}
	else
	{
		m_pInstancedDecl = m_CachedInstancedDecl[*pDeclIndex].pDecl;
	}
}	


//void 
//CRVertexBufferGroupD3D::SetInstancedVB(CRVertexBufferD3D* pInstancedVB)
//{
//	_DEBUG_ASSERT(pInstancedVB);
//	
//	uint* pIndex = m_DeclIndices.Get((uint) pInstancedVB);
//	if(!pIndex)
//	{
//		// We add the new instanced data VB into the
//		// list
//		DeclData data;
//		data.pDecl	= NULL;
//		data.pVB	= pInstancedVB;
//		m_DeclList.push_back(data);	
//		
//		// Then add the index mapping
//		m_DeclIndices.Add((uint) pInstancedVB, ((uint)m_DeclList.size())-1);
//				
//		// Then cache the declaration with the new data
//		CacheVertexDeclaration();
//		
//		m_pInstancedVB		= pInstancedVB;
//		m_pInstancedDecl	= m_DeclList.back().pDecl;
//		
//		_DEBUG_ASSERT(pInstancedVB == m_DeclList.back().pVB);
//	}
//	else
//	{
//		m_pInstancedVB		= pInstancedVB;
//		m_pInstancedDecl	= m_DeclList[*pIndex].pDecl;
//		
//		_DEBUG_ASSERT(pInstancedVB == m_DeclList[*pIndex].pVB);
//	}
//}

IDirect3DVertexDeclaration9* 
CRVertexBufferGroupD3D::GetDeclD3D()
{
	//if(!m_pDecl)
	//	CacheVertexDeclaration();
	
	return m_pDecl;
}

IDirect3DVertexDeclaration9* 
CRVertexBufferGroupD3D::GetInstancedDeclD3D()
{
	//if(!m_pDecl)
	//	CacheVertexDeclaration();
	
	return m_pInstancedDecl;
}

_NAMESPACE_END