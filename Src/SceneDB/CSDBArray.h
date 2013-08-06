//==============================================================================
//
//		CSDBArray.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/23/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSDBArray : public CRefCounted<ISDBArray>
{
	struct Data
	{
		uint			filterType;
		const AABox*	pBV;
		const OOBox*	pOOBV;
		void*			pData;
	};

	typedef std::vector<Data> DataList;

private: 
	AABox			m_FrustumQueryLimits;
	DataList		m_Data;

public: 
	CSDBArray();
	virtual ~CSDBArray();

public:	
	SDBDataHandle	Insert(const AABox* pBV, const OOBox* pOOBV, uint filterType, void* pData);
	void	Remove(SDBDataHandle handle);
	boolean	UpdateMove(SDBDataHandle handle, const AABox* pNewBounds);

	uint	Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType);
	uint	QuerySphere(void** ppToStore, uint bufferSize, const AABox& sphereAA, const Sphere& sphereBV, uint filterType);
	uint	QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType);
	uint	QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType);
	uint	QueryAll(void** ppToStore, uint bufferSize, uint filterType);

	_IMPL_ALIGNED_ALLOCS(16)
};

_NAMESPACE_END
