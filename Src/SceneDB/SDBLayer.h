//==============================================================================
//
//		SDBSparseGrid.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/23/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct SDBDataKey
{
	size_t data;
	
	bool operator==(const SDBDataKey& other) const
	{
		return data == other.data;
	}
};

struct SDBDataHash
{
	inline uint operator() (const SDBDataKey& key) const { return key.data; }
};

struct SDBGridPos
{
	int		pos[3];
	uint	posHash;
	
	bool operator==(const SDBGridPos& other) const
	{
		return pos[0] == other.pos[0] && 
			pos[1] == other.pos[1] &&
			pos[2] == other.pos[2];
	}
};

struct SDBGridPosHash
{
	inline uint operator() (const SDBGridPos& pos) const { return pos.posHash; }
};

struct SDBData
{
	SDBDataKey		next;
	SDBDataKey		prev;
	
	SDBGridPos		gridPos;
	
	uint			filterType;
	const AABox*	pBV;
	const OOBox*	pOOBV;
	void*			pData;
};

struct SDBOctCell
{
	uint		childFlag;
	SDBGridPos	children[8];
	
	SDBDataKey	firstData;
};

class SDBLayer : public CRefCounted<IRefCounted>
{
protected:
	typedef DenseChainedHashMap<SDBGridPos, SDBOctCell, SDBGridPosHash> SparseGrid;
	
protected:
	CSDBSparseLooseOctTree*	m_pMain;
	
	SparseGrid		m_Grid;
	uint			m_Level;
	gmtl::VecA3f	m_GridSizeVec;
	gmtl::VecA3f	m_HalfGridSizeVec;
	gmtl::VecA3f	m_CullGridSizeVec;
	AABox			m_FrustumQueryLimits;
	
	float		m_GridSize;
	float		m_HalfGridSize;
	float		m_InvScale;
	
	SDBLayer*	m_pChild;
	SDBLayer*	m_pParent;
		
public: 
	SDBLayer()
		: m_pChild(NULL)
		, m_pParent(NULL)
		, m_GridSize(1.0f)
		, m_InvScale(1.0f)
		, m_HalfGridSize(0.5f)
		, m_pMain(NULL)
	{
		m_FrustumQueryLimits.min = gmtl::VecA3f(-1.0f, -1.0f, 0.0f);
		m_FrustumQueryLimits.max = gmtl::VecA3f(1.0f, 1.0f, 1.0f);
	}

	~SDBLayer()
	{
	}

protected:
	//////////////////////////////////////////////////////////////////////////
	// Query Contained
	//
	//////////////////////////////////////////////////////////////////////////
	
	inline uint	QueryContained(void** ppToStore, uint bufferSize, uint curIndex, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell);
	inline uint	QueryFrustumPerspContained(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
									const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType, 
									SDBGridPos& cellPos, SDBOctCell* pCell);
	inline uint	QueryFrustumOrthoContained(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
									const gmtl::MatrixA44f& viewProj, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell);
	inline uint	QueryCellInfoContained(AABox* pToStore, uint bufferSize, uint curIndex, SDBGridPos& cellPos, SDBOctCell* pCell);

	inline uint	QueryContained(void** ppToStore, uint bufferSize, uint curIndex, uint filterType, SDBGridPos& cellPos);
	inline uint	QueryFrustumPerspContained(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
									const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType, 
									SDBGridPos& cellPos);
	inline uint	QueryFrustumOrthoContained(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
									const gmtl::MatrixA44f& viewProj, uint filterType, SDBGridPos& cellPos);
	inline uint	QueryCellInfoContained(AABox* pToStore, uint bufferSize, uint curIndex, SDBGridPos& cellPos);

	//////////////////////////////////////////////////////////////////////////
	//	Query Impl
	//
	//////////////////////////////////////////////////////////////////////////
	inline uint	Query(void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell);
	inline uint QuerySphere(void** ppToStore, uint bufferSize, uint curIndex, const Sphere& sphere, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell);
	inline uint	QueryFrustumOrtho(void** ppToStore, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj,
								 uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell);
	inline uint	QueryFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj,
								 uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell);
	inline uint	QueryFrustumPersp(void** ppToStore, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
								float nearZ, float farZ, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell);
	inline uint	QueryFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
								float nearZ, float farZ, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell);
	
	inline uint	QueryCellInfo(AABox* pToStore, uint bufferSize, uint curIndex, const AABox& queryBV, SDBGridPos& cellPos, SDBOctCell* pCell);

	//////////////////////////////////////////////////////////////////////////
	//	Top Level Query
	//
	//////////////////////////////////////////////////////////////////////////
	//inline uint	QueryNeighbourPairSphere(SDBGridPos& gridPos, void** ppToStore, uint bufferSize, uint curIndex, 
	//								const Sphere& sphere, uint filterType, int zInc);
	//inline uint QueryNeighbourPair(SDBGridPos& gridPos, void** ppToStore, uint bufferSize, uint curIndex, 
	//								const AABox& queryBV, uint filterType, int zInc);
	//inline uint	QueryNeighbourPairFrustumOrtho(SDBGridPos& gridPos, void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
	//								const gmtl::MatrixA44f& viewProj, uint filterType, int zInc);
	//inline uint	QueryNeighbourPairFrustumOrtho(SDBGridPos& gridPos, void** ppToStore, uint bufferSize, uint curIndex, 
	//								const gmtl::MatrixA44f& viewProj, uint filterType, int zInc);
	//inline uint	QueryNeighbourPairFrustumPersp(SDBGridPos& gridPos, void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
	//								const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType, int zInc);
	//inline uint	QueryNeighbourPairFrustumPersp(SDBGridPos& gridPos, void** ppToStore, uint bufferSize, uint curIndex, 
	//								const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType, int zInc);
	//							
	//inline uint QueryNeighbourPairCellInfo(SDBGridPos& gridPos, AABox* pToStore, uint bufferSize, uint curIndex, const AABox& queryBV, int zInc);
	
	inline void	GetGridPos(const gmtl::Vec3f& pos, SDBGridPos* pGridPos);
	inline uint	GetOctCellPos(const SDBGridPos& gridPos);
	inline void GetGridPosNoHash(const gmtl::VecA3f& pos, SDBGridPos* pGridPos, uint& xHash, uint& yHash, uint& zHash);

public:
	void	Initialize(CSDBSparseLooseOctTree* pMain, 
					float gridSize,
					uint initSize, 
					SDBLayer* pParent, 
					SDBLayer* pChild);
	
	void	Insert(const gmtl::Vec3f& pos, SDBDataKey& dataKey, SDBData& data);
	void	Remove(SDBDataKey& dataKey, SDBData& data);
	
	void	NotifyCreated(const SDBGridPos& childPos);
	void	NotifyDestroyed(const SDBGridPos& childPos);
	
	//////////////////////////////////////////////////////////////////////////
	//	Query
	//
	//////////////////////////////////////////////////////////////////////////
	uint	Query(void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, uint filterType, SDBGridPos& cellPos);
	uint	QuerySphere(void** ppToStore, uint bufferSize, uint curIndex, const Sphere& sphere, uint filterType, SDBGridPos& cellPos);
	uint	QueryFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, uint filterType, SDBGridPos& cellPos);
	uint	QueryFrustumOrtho(void** ppToStore, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, uint filterType, SDBGridPos& cellPos);
	uint	QueryFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
							float nearZ, float farZ, uint filterType, SDBGridPos& cellPos);
	uint	QueryFrustumPersp(void** ppToStore, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
							float nearZ, float farZ, uint filterType, SDBGridPos& cellPos);
	uint	QueryCellInfo(AABox* pToStore, uint bufferSize, uint curIndex, const AABox& queryBV, SDBGridPos& cellPos);

	//////////////////////////////////////////////////////////////////////////
	//	Top Level Query
	//
	//////////////////////////////////////////////////////////////////////////
	uint	QueryIncludeAdjacent(void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, uint filterType, float looseOffset);	
	uint	QueryIncludeAdjacentSphere(void** ppToStore, uint bufferSize, uint curIndex, const AABox& sphereAA, const Sphere& sphere, uint filterType, float looseOffset);
	uint	QueryIncludeAdjacentFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const AABox& queryBV, 
											const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType, float looseOffset);
	uint	QueryIncludeAdjacentFrustumPersp(void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, 
											const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType, float looseOffset);
	uint	QueryIncludeAdjacentFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const AABox& queryBV, 
											const gmtl::MatrixA44f& viewProj, uint filterType, float looseOffset);
	uint	QueryIncludeAdjacentFrustumOrtho(void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, 
											const gmtl::MatrixA44f& viewProj, uint filterType, float looseOffset);
	
	uint	QueryIncludeAdjacentCellInfo(AABox* pToStore, uint bufferSize, uint curIndex, const AABox& queryBV, float looseOffset);
	
	SDBLayer*	GetChild()	{ return m_pChild; }
	
	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(SDBLayer)

_NAMESPACE_END
