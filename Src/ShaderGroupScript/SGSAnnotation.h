//==============================================================================
//
//		SGSAnnotation.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Tuesday, 17 June, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SGSAnnotation
{
	const static int MAX_NUM_PARAMETERS = 16;

private:
	IByteBufferPtr	m_pName;
	SGSParameter	m_Parameters[MAX_NUM_PARAMETERS];
	uint			m_NumParameters;

public:
	SGSAnnotation()
		: m_NumParameters(0)
	{

	}

	//SGSAnnotation(const SGSAnnotation& other)
	//{
	//	*this = other;
	//}

	~SGSAnnotation()
	{

	}

public:
	const char* GetName();
	void SetName(IByteBuffer* pName);

	void AddParameter(const SGSParameter& param);
	uint GetNumParameters() { return m_NumParameters; }
	SGSParameter& GetParameter(uint index) { _DEBUG_ASSERT(index < MAX_NUM_PARAMETERS); return m_Parameters[index]; }

	//inline SGSAnnotation& operator =(const SGSAnnotation& rhs)
	//{
	//	m_pName = rhs.m_pName;
	//	_LOOPi(rhs.m_NumParameters)
	//	{
	//		m_Parameters[i] = rhs.m_Parameters[i];
	//	}

	//	return *this;	
	//}

};

_NAMESPACE_END