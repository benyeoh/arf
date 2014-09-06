//==============================================================================
//
//		SGSTechnique.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Tuesday, 1 July, 2014
//
//==============================================================================

#include "ShaderGroupScript.h"

_NAMESPACE_BEGIN

const char* SGSTechnique::GetName()
{
	if(!m_pName)
		return NULL;

	return (const char*) m_pName->GetData();
}

void SGSTechnique::SetName(IByteBuffer* pName)
{
	m_pName = pName;
}

void SGSTechnique::AddPass(const SGSPass& param)
{
	_DEBUG_ASSERT(m_NumPasses < MAX_NUM_PASSES);
	m_Passes[m_NumPasses] = param;
	m_NumPasses++;
}


_NAMESPACE_END