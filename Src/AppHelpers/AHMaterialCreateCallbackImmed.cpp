//================================================================================
//
//		AHMaterialCreateCallbackImmed.cpp
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

IREffect* AHMaterialCreateCallbackImmed::CreateEffect(IBFXMaterialGroup* pGroup, uint type, uint availability)
{
	m_MatMutex.Lock();
	IREffect* pEffect = pGroup->CreateEffect(type, availability);
	m_MatMutex.Unlock();

	_DEBUG_ASSERT(pEffect);

	return pEffect;
}

_NAMESPACE_END

#endif