//==============================================================================
//
//		SGSPass.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Tuesday, 1 July, 2014
//
//==============================================================================

#include "ShaderGroupScript.h"

_NAMESPACE_BEGIN

const char* SGSPass::GetName()
{
	if(!m_pName)
		return NULL;

	return (const char*) m_pName->GetData();
}

void SGSPass::SetName(IByteBuffer* pName)
{
	m_pName = pName;
}

void SGSPass::AddProperty(const SGSProperty& prop)
{
	m_Properties.push_back(prop);
}

_NAMESPACE_END