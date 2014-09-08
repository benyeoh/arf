//==============================================================================
//
//		SGSShader.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 9 June, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SGSShader
{
private:
	IByteBufferPtr m_pName;
	IByteBufferPtr m_pShaderCode;

public:
	SGSShader()
	{

	}

	~SGSShader()
	{

	}

public:
	const char* GetShaderCode() const;
	uint GetShaderCodeLength() const;

	const char* GetName() const;

	void SetName(IByteBuffer* pName);
	void SetShaderCode(IByteBuffer* pCode);
};

_NAMESPACE_END