//================================================================================
//
//		AHMaterialCreateCallbackDeferred.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/23/2012
//
//================================================================================

#include "AppHelpers.h"

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

_NAMESPACE_BEGIN

void AHMaterialCreateCallbackDeferred::Reset()
{
	// Reset
	if(m_UncachedMatMap.GetNumOfElements() > 0)
	{
		m_UncachedMatMap.Reset();
		m_UncachedMatList.clear();
	}
}

IREffect* AHMaterialCreateCallbackDeferred::CreateEffect(IBFXMaterialGroup* pGroup, uint type, uint availability)
{
	UncachedMat mat;
	mat.pMatGroup = pGroup;
	mat.matType = type;
	mat.uncachedFlags = availability;

	mat.SetHash();

	MutexScopeLock lock(&m_UncachedMatMutex);
	{
		if(m_UncachedMatMap.Get(mat) == NULL)
		{
			m_UncachedMatMap.Add(mat, TRUE);
			m_UncachedMatList.push_back(mat);
		}
	}

	// We can return a placeholder effect here instead
	return NULL;
}

_NAMESPACE_END

#endif