//==============================================================================
//
//		SGSPass.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Tuesday, 1 July, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SGSPass
{
private:
	IByteBufferPtr m_pName;
	std::vector<SGSProperty> m_Properties;

public:
	SGSPass()
	{

	}

	SGSPass(const SGSPass& other)
	{
		*this = other;
	}

	~SGSPass()
	{

	}

public:
	const char* GetName();
	void SetName(IByteBuffer* pName);

	void AddProperty(const SGSProperty& prop);
	uint GetNumProperties() { return (uint) m_Properties.size(); }
	const SGSProperty& GetProperty(uint index) { return m_Properties[index]; }

	inline SGSPass& operator =(const SGSPass& rhs)
	{
		m_pName = rhs.m_pName;
		m_Properties.clear();
		_LOOPi(rhs.m_Properties.size())
		{
			m_Properties.push_back(rhs.m_Properties[i]);
		}

		return *this;	
	}
};

_NAMESPACE_END