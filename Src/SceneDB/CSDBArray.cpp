//==============================================================================
//
//		CSDBArray.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/3/2009
//
//==============================================================================

#include "SceneDBFramework.h"

_NAMESPACE_BEGIN

CSDBArray::CSDBArray()
{
	m_FrustumQueryLimits.min = gmtl::VecA3f(-1.0f, -1.0f, 0.0f);
	m_FrustumQueryLimits.max = gmtl::VecA3f(1.0f, 1.0f, 1.0f);
}

CSDBArray::~CSDBArray()
{

}

SDBDataHandle	CSDBArray::Insert(const AABox* pBV, const OOBox* pOOBV, uint filterType, void* pData)
{
	Data toInsert;
	toInsert.filterType = filterType;
	toInsert.pBV = pBV;
	toInsert.pOOBV = pOOBV;
	toInsert.pData = pData;

	m_Data.push_back(toInsert);

	return SDBDataHandle((size_t) pData);
}

void	CSDBArray::Remove(SDBDataHandle handle)
{
    _DEBUG_ASSERT(handle.IsValid());

	void* pData = (void*)(size_t) handle;

	uint i = 0;
	while( i < (uint) m_Data.size() )
	{
		if(m_Data[i].pData == pData)
			break;

		i++;
	}
	
	if(i < (uint) m_Data.size())
	{
		if(m_Data.size() > 1)
		{
			Data& data = m_Data.back();
			m_Data[i] = data;
		}

		m_Data.pop_back();
	}
}

uint	CSDBArray::Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType)
{
	uint stored = 0;

	_LOOPi(m_Data.size())
	{
		if(m_Data[i].filterType & filterType)
		{
			if(AABoxInAABox(m_Data[i].pBV, &queryBV))
			{
				_DEBUG_ASSERT(stored < bufferSize);
				ppToStore[stored] = m_Data[i].pData;
				stored++;
			}
		}
	}

	return stored;
}

uint	CSDBArray::QuerySphere(void** ppToStore, uint bufferSize, const AABox& sphereAA, const Sphere& sphereBV, uint filterType)
{
	uint stored = 0;

	_LOOPi(m_Data.size())
	{
		if(m_Data[i].filterType & filterType)
		{
			if(AABoxInSphere(m_Data[i].pBV, &sphereBV))
			{
				_DEBUG_ASSERT(stored < bufferSize);
				ppToStore[stored] = m_Data[i].pData;
				stored++;
			}
		}
	}

	return stored;
}

uint	CSDBArray::QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType)
{
	uint stored = 0;

	_LOOPi(m_Data.size())
	{
		if(m_Data[i].filterType & filterType)
		{
			AABox screenSpace;
			if(pScreenSpaceRes)
				*(pScreenSpaceRes+stored) = screenSpace;
	
			TransformOOBoxToAABox(&viewProj, m_Data[i].pOOBV, &screenSpace);
			if(AABoxInAABox(&m_FrustumQueryLimits, &screenSpace))
			{
				_DEBUG_ASSERT(stored < bufferSize);
				ppToStore[stored] = m_Data[i].pData;
				stored++;
			}
		}
	}

	return stored;
}

uint	CSDBArray::QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType)
{
	uint stored = 0;

	_LOOPi(m_Data.size())
	{
		if(m_Data[i].filterType & filterType)
		{
			AABox screenSpace;
			TransformAndProjectOOBoxToAABox(&viewProj, nearZ, farZ, m_Data[i].pOOBV, &screenSpace);
			if(pScreenSpaceRes)
				*(pScreenSpaceRes+stored) = screenSpace;

			if(AABoxInAABox(&m_FrustumQueryLimits, &screenSpace))
			{
				_DEBUG_ASSERT(stored < bufferSize);
				ppToStore[stored] = m_Data[i].pData;
				stored++;
			}
		}
	}

	return stored;
}

uint	CSDBArray::QueryAll(void** ppToStore, uint bufferSize, uint filterType)
{
	uint stored = 0;

	_LOOPi(m_Data.size())
	{
		if(m_Data[i].filterType & filterType)
		{
			_DEBUG_ASSERT(stored < bufferSize);
			ppToStore[stored] = m_Data[i].pData;
			stored++;
		}
	}

	return stored;
}

boolean	CSDBArray::UpdateMove(SDBDataHandle handle, const AABox* pNewBounds)
{
    return FALSE;
}

_NAMESPACE_END