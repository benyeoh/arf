//==============================================================================
//
//		SGSTechnique.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 26 June, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SGSTechnique
{
	const static int MAX_NUM_PASSES = 16;

private:
	IByteBufferPtr m_pName;
	
	SGSPass m_Passes[MAX_NUM_PASSES];
	uint m_NumPasses;

public:
	SGSTechnique()
		: m_NumPasses(0)
	{

	}

	~SGSTechnique()
	{

	}

public:
	const char* GetName();
	void SetName(IByteBuffer* pName);

	void AddPass(const SGSPass& pass);
	uint GetNumPasses() { return m_NumPasses; }
	const SGSPass& GetPass(uint index) { _DEBUG_ASSERT(index < MAX_NUM_PASSES); return m_Passes[index]; }

	//inline SGSTechnique& operator =(const SGSTechnique& rhs)
	//{
	//	m_pName = rhs.m_pName;
	//	_LOOPi(rhs.m_NumPasses)
	//	{
	//		m_Passes[i] = rhs.m_Passes[i];
	//	}

	//	return *this;	
	//}
};

_NAMESPACE_END