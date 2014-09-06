//==============================================================================
//
//		SGSParameter.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 19 June, 2014
//
//==============================================================================

#include "ShaderGroupScript.h"

_NAMESPACE_BEGIN

const char* SGSParameter::GetName()
{
	if(!m_pName)
		return NULL;

	return (const char*) m_pName->GetData();
}

void SGSParameter::SetName(IByteBuffer* pName)
{
	m_pName = pName;
}

void SGSParameter::AddProperty(const SGSProperty& prop)
{
	m_Properties.push_back(prop);
}

_NAMESPACE_END