//==============================================================================
//
//		CSDBOctantOrderSLOctTree.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Wednesday, 19 June, 2013
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSDBOctantOrderSLOctTree : public CRefCounted<ISDBSparseLooseOctTree>
{
	typedef DenseChainedHashMap<SDBDataKey, SDBData, SDBDataHash> DataMap;

	//struct TooLargeData
	//{
	//	uint			filterType;
	//	const AABox*	pBV;
	//	const OOBox*	pOOBV;
	//	void*			pData;
	//};

	//typedef std::vector<TooLargeData> TooLargeList;

private: 
	AABox			m_FrustumQueryLimits;

	DataMap			m_Data;
	SDBLayerPtr*	m_ppLayers;
	uint			m_NumLayers;

	float		m_Scale;	
	float		m_MaxGridSize;

	SDBDataKey		m_TooLargeDataStart;

	//TooLargeList	m_TooLargeList;

public: 
	CSDBOctantOrderSLOctTree(uint levels, float scale, uint initSize);
	virtual ~CSDBOctantOrderSLOctTree();

private:
	inline void	GetDataKey(void* pData, SDBDataKey* pDataKey);
	inline uint	FindLayerFromSize(float size);

public:	
	void	Insert(const AABox* pBV, const OOBox* pOOBV, uint filterType, void* pData);
	void	Remove(void* pData);

	uint	Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType);
	uint	QuerySphere(void** ppToStore, uint bufferSize, const AABox& sphereAA, const Sphere& sphereBV, uint filterType);
	uint	QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, 
		AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType);
	uint	QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, 
		AABox& frustumAA, const gmtl::MatrixA44f& viewProj, 
		float nearZ, float farZ, uint filterType);
	uint	QueryAll(void** ppToStore, uint bufferSize, uint filterType);

	uint	QueryCellInfo(AABox* pToStore, uint bufferSize, const AABox& queryBV);
	float	GetMaxGridSize();

	inline DataMap& GetDataMap() { return m_Data; }

	_IMPL_ALIGNED_ALLOCS(16)
};

_NAMESPACE_END
