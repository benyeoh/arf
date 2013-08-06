//==============================================================================
//
//		RefCounted.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, February 07, 2006
//
//==============================================================================

#pragma once


_NAMESPACE_BEGIN

struct IRefCounted
{
	virtual int AddRef() = 0;
	virtual int Release() = 0;
};

template <typename _INHERITED>
class CRefCounted : public _INHERITED
{
private:
	int	m_RefCount;
	
public:
	CRefCounted() 
		: m_RefCount(0)
	{
	}
	
	virtual ~CRefCounted()
	{
	}
	
public:
	virtual int AddRef()
	{ 
		++m_RefCount; 
		return m_RefCount; 
	}
	
	virtual int Release()	
	{ 
		--m_RefCount; 
		
		if(m_RefCount <= 0) 
		{ 
			delete this; 
			return 0; 
		} 
	
		return m_RefCount; 
	}
};


_NAMESPACE_END