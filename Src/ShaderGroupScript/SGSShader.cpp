//==============================================================================
//
//		SGSShader.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 9 June, 2014
//
//==============================================================================

#include "ShaderGroupScript.h"

_NAMESPACE_BEGIN

const char* SGSShader::GetName()
{
	return m_pName ? (const char*) m_pName->GetData() : NULL;
}

const char* SGSShader::GetShaderCode()
{
	return m_pShaderCode ? (const char*) m_pShaderCode->GetData() : NULL;
}

void SGSShader::SetName(IByteBuffer* pName)
{
	m_pName = pName;
}

void SGSShader::SetShaderCode(IByteBuffer* pCode)
{
	m_pShaderCode = pCode;
}
_NAMESPACE_END