//==============================================================================
//
//		CSDBOctantOrderLooseOctTree.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 24 June, 2013
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSDBOctantOrderLooseOctTree : public CRefCounted<ISDBSparseLooseOctTree>
{
	// OctantOrderCell
	struct OctantOrderCell
	{
		uint children;
		size_t firstDataID;
	};

	struct OctantOrderCellKey
	{
		uint gridUniqueHash;

		inline bool operator==(const OctantOrderCellKey& other) const { return gridUniqueHash == other.gridUniqueHash; }
	};

	struct OctantOrderCellKeyHash
	{
		inline uint operator() (const OctantOrderCellKey& key) const { return key.gridUniqueHash; }
	};

	typedef DenseChainedHashMap<OctantOrderCellKey, OctantOrderCell, OctantOrderCellKeyHash> OctantOrderCellTable;

	// OctantOrderData
	struct OctantOrderData
	{
		size_t nextID;
		size_t prevID;
		uint filterType;

		//	To remove..
		const AABox* pAABB;
		const OOBox* pOOBB;

		void* pData;
	};

	struct OctantOrderDataKey
	{
		size_t dataID;
		OctantOrderCellKey cellKey;

		inline bool operator==(const OctantOrderDataKey& other) const { return dataID == other.dataID; }
	};
	
	struct OctantOrderDataKeyHash
	{
		inline uint operator() (const OctantOrderDataKey& key) const { return key.cellKey.gridUniqueHash * MAX_DATA_PER_CELL + (key.dataID % MAX_DATA_PER_CELL); }
	};

	typedef DenseChainedHashMap<OctantOrderDataKey, OctantOrderData, OctantOrderDataKeyHash> OctantOrderDataTable;

	// Octant
	struct OctantData
	{
		OctantOrderCellTable* pCellTable;
		OctantOrderDataTable* pDataTable;
	};

	struct OctantKey
	{
		uint octantPos;

		inline bool operator== (const OctantKey& other) const { return octantPos == other.octantPos; }
	};

	struct OctantKeyHash
	{
		inline uint operator() (const OctantKey& key) const { return key.octantPos; }
	};

	typedef DenseChainedHashMap<OctantKey, OctantData, OctantKeyHash> OctantTable;

	// AppHandle
	struct AppHandle
	{
		size_t	dataID;
		uint	octantPosHash;
		uint	gridInOctantHash;
	};

	struct QueryContext
	{
		union
		{
			const void* pQueryBV;
			const gmtl::MatrixA44f* pProj;
		};

		union
		{
			void** ppToStore;
			AABox* pCellInfo;
		};

		OctantData octant;

		uint filterType;
		uint bufferSize;

		// Additional params
		AABox* pScreenSpaceRes;
		//AABox* pFrustumAA;
		float nearZ;
		float farZ;
	};

	typedef uint (CSDBOctantOrderLooseOctTree::*QueryOctantFn)(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex);
	typedef uint (CSDBOctantOrderLooseOctTree::*QueryOctantContainedFn)(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex);

private:
	AABox		m_FrustumQueryLimits;
	
	std::vector<OctantData>	m_AllocatedOctantData;
	std::vector<OctantData>	m_FreeOctantData;
	OctantTable m_Octants;

	float		m_TopGridSize;
    float		m_MaxGridSizeForData;
	float		m_Size;
	uint		m_TopLevel;
    int         m_OctantMaxGridPerSide;
    float       m_OctantNumGridsPerSide;

	std::vector<OctantOrderData> m_TooLargeData;
	size_t		m_FreeTooLargeDataHeadID;
	size_t		m_TooLargeDataHeadID;

	std::vector<AppHandle>	m_AppHandles;
	size_t		m_FreeAppHandleHeadID;

    // Optimization for uniform grid sampling
    // We quickly skip Y plane levels if no octant exists there
    // (Typical scenario for a flat earth type worlds)
    // Remember that an octant can easily cover over a kilometer in the Y direction
    uint    m_NumOctantsYPlane[(1 << NUM_BITS_Y)];

public:
	CSDBOctantOrderLooseOctTree(uint numLevels, float size);
	~CSDBOctantOrderLooseOctTree();

private:
	template<QueryOctantFn QueryFn> inline uint SearchOctants(QueryContext& context, const AABox& searchAA, uint curIndex);
	template<QueryOctantFn QueryFn> inline uint QueryChildren(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex);
	template<QueryOctantContainedFn QueryFn> inline uint QueryChildrenContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex);

	inline void GetOctantPosFromWorld(const gmtl::VecA3f& pos, gmtl::VecA3i& outOctantPos);
	inline void GetOctantAndGridPosFromWorld(const gmtl::VecA3f& pos, gmtl::VecA3i& outOctantPos, gmtl::VecA3i& outGridPosInOctant);
	inline uint GetOctantHash(const gmtl::VecA3i& octantPos);
	inline uint GetOctantOrdering(const gmtl::VecA3i& gridInOctantPos);
	inline uint GetOctantOrderingHash(uint octantOrder, uint levelFromTop);
	
	inline uint FindLevelOffsetFromSize(float size);
	
	inline size_t AllocHandle();
	inline void FreeHandle(size_t handle);

	void InsertParentCells(OctantData* pOctant, uint octantOrderHash, uint octantOrdering, uint parentOffset, uint levelOffset);
	boolean InsertInOctant(OctantData* pOctant, uint octantOrderHash, uint octantOrdering, const AABox* pAABox, const OOBox* pOOBV, uint filterType, void* pData, uint levelOffset);
	boolean RemoveParentCells(OctantData* pOctant, uint octantOrderHash, uint parentOffset, uint levelOffset);

	// AABox queries
	uint	QueryOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex);
	uint	QueryOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex);

	// Sphere queries
	uint	QuerySphereOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex);

	// Orthogonal frustum queries
	uint	QueryInFrustumOrthoOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex);
	uint	QueryInFrustumOrthoOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex);

	// Perspective frustum Queries
	uint	QueryInFrustumPerspOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex);
	uint	QueryInFrustumPerspOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex);

	uint	QueryCellInfoOctantContained(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex);
	uint	QueryCellInfoOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex);

public:
	SDBDataHandle	Insert(const AABox* pAABox, const OOBox* pOOBV, uint filterType, void* pData);
	void			Remove(SDBDataHandle handle);
	boolean			UpdateMove(SDBDataHandle handle, const AABox* pNewBounds);

	uint	Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType);
	uint	QuerySphere(void** ppToStore, uint bufferSize, const AABox& sphereAA, const Sphere& sphereBV, uint filterType);
	uint	QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType);
	uint	QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType);
	uint	QueryAll(void** ppToStore, uint bufferSize, uint filterType);

	uint	QueryCellInfo(AABox* pToStore, uint bufferSize, const AABox& queryBV);

	float GetMaxGridSize() { return m_MaxGridSizeForData; }

	_IMPL_ALIGNED_ALLOCS(16)
};

_NAMESPACE_END