//==============================================================================
//
//		SGSParameter.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 19 June, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SGSParameter
{
private:
	IByteBufferPtr	m_pName;
	std::vector< SGSProperty > m_Properties;

public:
	SGSParameter()
	{

	}

	SGSParameter(const SGSParameter& other)
	{
		*this = other;
	}

	~SGSParameter()
	{

	}

public:
	const char* GetName();
	void SetName(IByteBuffer* pName);

	void AddProperty(const SGSProperty& prop);
	
	uint GetNumProperties() { return m_Properties.size(); }
	SGSProperty& GetProperty(uint index) { return m_Properties[index]; }
	
	inline SGSParameter& operator =(const SGSParameter& rhs)
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