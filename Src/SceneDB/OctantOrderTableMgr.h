//==============================================================================
//
//		OctantOrderTableMgr.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 24 June, 2013
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class OctantOrderTableManager
{
	const static uint MAX_DATA_PER_CELL = 7; // Must be prime number

	struct OctantOrderCell
	{
		uint children;
		size_t firstDataID;
	};

	struct OctantOrderCellKey
	{
		uint gridUniqueHash;

		bool operator==(const OctantOrderCellKey& other) const
		{
			return gridUniqueHash == other.gridUniqueHash;
		}
	};

	struct OctantOrderCellKeyHash
	{
		inline uint operator() (const OctantOrderCellKey& key) const { return key.gridUniqueHash; }
	};

	typedef DenseChainedHashMap<OctantOrderCellKey, OctantOrderCell, OctantOrderCellKeyHash> OctantCellTable;

	struct OctantOrderData
	{
		size_t nextID;
		size_t prevID;
		uint flag;

		//	To remove..
		AABox* pAABB;
		OOBox* pOOBB;
	};

	struct OctantOrderDataKey
	{
		size_t dataID;
		OctantOrderCellKey cellKey;

		bool operator==(const OctantOrderDataKey& other) const
		{
			return dataID == other.dataID;
		}
	};
	
	struct OctantOrderDataKeyHash
	{
		inline uint operator() (const OctantOrderDataKey& key) const { return key.cellKey.gridUniqueHash * MAX_DATA_PER_CELL + (key.dataID % MAX_DATA_PER_CELL); }
	};

	typedef DenseChainedHashMap<OctantOrderDataKey, OctantOrderData, OctantOrderDataKeyHash> OctantDataTable;

	struct OctantData
	{
		OctantCellTable* pCellTable;
		OctantDataTable* pDataTable;
	};

	struct OctantKey
	{
		uint octantPos;
	};

	struct OctantKeyHash
	{
		inline uint operator() (const OctantKey& key) const { return key.octantPos; }
	};

	typedef DenseChainedHashMap<OctantKey, OctantData, OctantKeyHash> OctantTable;

	struct OctantHandle
	{
		OctantOrderCellKey key;		// This will give access to cell and data
		uint octantUniqueHash;		// To access octant table
	};

private:
	AABox		m_FrustumQueryLimits;
	
	std::vector<OctantData>	m_AllocatedOctantData;
	std::vector<OctantData>	m_FreeOctantData;

	OctantTable m_Octants;
	float		m_TopGridSize;
	float		m_Size;
	uint		m_TopLevel;

public:
	OctantOrderTableManager();
	~OctantOrderTableManager();

private:
	void GetOctantAndOrderPosFromWorld(const gmtl::VecA3f& pos, uint& octantPos, uint& octantOrder);
	OctantTable* GetOctantTable(uint pos);
	uint GetOctantOrdering(uint x, uint y, uint z);

public:
	void	SetGridSize(uint numLevels, uint sizePow2);

	void	Insert(const AABox* pAABox, const OOBox* pOOBV, uint filterType, void* pData, SDBDataHandle* pOutHandle);
	void	Remove(SDBDataHandle* pHandle);

	uint	Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType);
	uint	QuerySphere(void** ppToStore, uint bufferSize, const Sphere& sphereBV, uint filterType);
	uint	QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType);
	uint	QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType);
	uint	QueryAll(void** ppToStore, uint bufferSize, uint filterType);

	uint	QueryCellInfo(AABox* pToStore, uint bufferSize, const AABox& queryBV);

	_IMPL_ALIGNED_ALLOCS(16)
};

_NAMESPACE_END