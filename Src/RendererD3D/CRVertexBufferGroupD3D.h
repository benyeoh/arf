//==============================================================================
//
//		CRVertexBufferGroupD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 23, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRVertexBufferGroupD3D : public CRVertexBufferGroup
{
	struct MurmurUintHash
	{
		inline uint operator() (uint key) { return MurmurHash(&key, 1); }
	};
	
	struct VBDescKey
	{
		// This is basically the VB desc packed into 4 ints (16 bytes)
		// This is a bit faster and more importantly, the order in which
		// a description exists in an array does not matter
		uint descPacked[4];
		uint descHash;
		
		boolean operator==(const VBDescKey& other)
		{				
			return (descPacked[0] == other.descPacked[0]) &&
				(descPacked[1] == other.descPacked[1]) &&
				(descPacked[2] == other.descPacked[2]) &&
				(descPacked[3] == other.descPacked[3]);
		}
	};

	struct VBDescHash
	{
		inline uint operator() (const VBDescKey& key) { return key.descHash; }
	};
	
	typedef DenseChainedHashMap<uint, uint, MurmurUintHash> InstanceVBMap;	
	typedef DenseChainedHashMap<VBDescKey, uint, VBDescHash> InstanceDeclMap;
	
	struct VBDeclData
	{
		RVertexDesc desc[MAX_VERTEX_DESC];
		IDirect3DVertexDeclaration9Ptr pDecl;
	};
	
private:
	IDirect3DVertexDeclaration9Ptr m_pDecl;
	
	CRVertexBufferD3D*				m_pInstancedVB;
	IDirect3DVertexDeclaration9*	m_pInstancedDecl;
	
	InstanceVBMap		m_CachedInstancedVBIndices;
	InstanceDeclMap		m_CachedInstancedDeclIndices;
	
	std::vector<CRVertexBufferPtr>	m_CachedInstancedVB;
	std::vector<VBDeclData>			m_CachedInstancedDecl;
	
public:
	CRVertexBufferGroupD3D(CRRenderer* pRenderer)
		: CRVertexBufferGroup(pRenderer)
		, m_pInstancedVB(NULL)
		, m_pInstancedDecl(NULL)
	{
		m_CachedInstancedVBIndices.Initialize(32, 0.7f);
		m_CachedInstancedDeclIndices.Initialize(32, 0.7f);
	}
	
	virtual ~CRVertexBufferGroupD3D()
	{
	}
	
private:
	uint SetVertexElement(D3DVERTEXELEMENT9& vertElem, const RVertexDesc& desc);
	
protected:
	IDirect3DVertexDeclaration9* CacheOneInstancedVertexDeclaration(const RVertexDesc* pInstDesc);

	void CacheVertexDeclaration();
	void DoNotifyAddedVertexBuffer(CRVertexBuffer* pVB);
	void DoNotifyRemovedVertexBuffer(CRVertexBuffer* pVB);	
	
public:
	IDirect3DVertexDeclaration9* GetDeclD3D();
	IDirect3DVertexDeclaration9* GetInstancedDeclD3D();
	
	void SetInstancedVB(uint instParamsSize, RVertexDesc* pDesc, uint* pDescPacked, uint vbDescHash);
	
	//void SetInstancedVB(CRVertexBufferD3D* pInstancedVB);
	CRVertexBufferD3D* GetInstancedVB() { return m_pInstancedVB; }
};

_NAMESPACE_END
