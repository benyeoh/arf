//==============================================================================
//
//		SGSAnnotation.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 19 June, 2014
//
//==============================================================================

#include "ShaderGroupScript.h"

_NAMESPACE_BEGIN

const char* SGSAnnotation::GetName()
{
	if(!m_pName)
		return NULL;

	return (const char*) m_pName->GetData();
}

void SGSAnnotation::SetName(IByteBuffer* pName)
{
	m_pName = pName;
}

void SGSAnnotation::AddParameter(const SGSParameter& param)
{
	_DEBUG_ASSERT(m_NumParameters < MAX_NUM_PARAMETERS);
	m_Parameters[m_NumParameters] = param;
	m_NumParameters++;
}


_NAMESPACE_END