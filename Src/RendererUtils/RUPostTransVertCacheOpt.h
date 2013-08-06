//==============================================================================
//
//		RUPostTransVertCacheOpt.h
//
//			Based on Tom Forsyth's implementation:
//			http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
//
//		Author:		Ben Yeoh
//		Date:		2/2/2009
//
//==============================================================================


#pragma once

_NAMESPACE_BEGIN

class RUPostTransVertCacheOpt
{
	const static uint	CACHE_SIZE			= 32;
	const static int	INVALID_CACHE_POS	= -1;
	
	struct CacheNode;
	
	struct TriangleData
	{
		boolean isAdded;
		float	score;
		int		vertIndices[3];
	};

	struct VertexData
	{
		const static uint MAX_TRIANGLE_INDICES = 128;
		
		CacheNode*	pNode;
		float		score;
		int			numTotalTriangles;
		int			numTrianglesNotAdded;
		int			triangleIndices[MAX_TRIANGLE_INDICES];
	};

	struct CacheNode
	{
		uint pos;
		VertexData* pVert;
		CacheNode* pNext;
		CacheNode* pPrev;
	};
	
private:
	VertexData*		m_pVertices;
	uint	m_NumVertices;
	
	TriangleData*	m_pTriangles;
	TriangleData**	m_ppDrawnTriangles;
	uint	m_NumDrawnTriangles;
	uint	m_NumTriangles;
		
	CacheNode					m_CacheNodes[CACHE_SIZE];
	CacheNode*					m_pHead;
	CacheNode*					m_pTail;
	
	TriangleData*	m_pHighestTriangle;
	
public: 
	RUPostTransVertCacheOpt()
		: m_pVertices(NULL)
		, m_NumVertices(0)
		, m_pTriangles(NULL)
		, m_NumTriangles(0)
		, m_NumDrawnTriangles(0)
		, m_pHighestTriangle(NULL)
		, m_pHead(NULL)
		, m_pTail(NULL)
	{
	
	}

	~RUPostTransVertCacheOpt()
	{
	}

private:
	inline void		MoveToHead(CacheNode* pNode);
	inline float	GetVertexScore(VertexData* pVert);
	inline float	GetTriangleScore(TriangleData* pTri);

	VertexData* AddOneVertToCache(VertexData& vert);
	void	AddToVertCache(VertexData& v1, 
						VertexData& v2,
						VertexData& v3);
										
	void	InitializeTriangles(IRIndexBuffer* pIB);
	void	InitializeVertices();

	void	DrawTriangle(TriangleData& tri);
	void	Simulate();
	
	void	RemoveTriangleFromVertex(TriangleData& tri, VertexData& data);

public: 
	void	Optimize(IRIndexBuffer* pToOptimize, uint numVertices);
};

_NAMESPACE_END
