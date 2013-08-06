//==============================================================================
//
//		RefCountSmartPointer.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Wednesday, February 08, 2006
//
//==============================================================================

#pragma once


_NAMESPACE_BEGIN

template <typename _RefCounted> 
class RefCountSmartPointer
{
private:
	_RefCounted* pRC;

public:
	RefCountSmartPointer() : pRC(0) {}
	RefCountSmartPointer(int null) : pRC(0) { _DEBUG_ASSERT(null == 0); }
	RefCountSmartPointer(_RefCounted* _pRC) 
		: pRC(_pRC)
	 {
		if(pRC)
			pRC->AddRef();
	}
	
	RefCountSmartPointer(const RefCountSmartPointer& rc)
	{
		pRC = rc.pRC;
		if(pRC)
			pRC->AddRef();
	}
	
	~RefCountSmartPointer()
	{
		_RELEASE(pRC);
	}
	
public:
	_RefCounted* GetPtr()			const { return pRC; }
	
	//------------------------------------------------------------------------------
	//	Casting operators
	//		
	//------------------------------------------------------------------------------
	operator _RefCounted*()			const { return pRC; }
	operator const _RefCounted*()	const { return pRC; }
	operator bool()					const { return pRC != 0; }
	
	//------------------------------------------------------------------------------
	//	Dereference operators()
	//		
	//------------------------------------------------------------------------------	
	_RefCounted& operator*()		const { return *pRC; }
	_RefCounted* operator->()		const { return pRC; }
	
	//------------------------------------------------------------------------------
	//	Assignment operators()
	//		
	//------------------------------------------------------------------------------	
	RefCountSmartPointer&  operator=(_RefCounted* newpRC)
	{
		if(newpRC)
		{
			uint count = newpRC->AddRef();
			//_DEBUG_TRACE(_W("ADDED\n"));
		}
		if(pRC)
		{
			uint count = pRC->Release();
			//_DEBUG_TRACE(_W("REMOVED\n"));
		}
		pRC = newpRC;
		return *this;
	}
	
	RefCountSmartPointer&  operator=(const RefCountSmartPointer& newRCSmartPtr)
	{
		if (newRCSmartPtr.pRC)
			newRCSmartPtr.pRC->AddRef();
		if (pRC)
			pRC->Release();
		pRC = newRCSmartPtr.pRC;
		return *this;
	}
	
	//------------------------------------------------------------------------------
	//	Boolean operators
	//
	//------------------------------------------------------------------------------
	bool operator !()											const { return pRC == 0; }
	bool operator ==(const _RefCounted* pRC2)					const { return pRC == pRC2; }
	bool operator ==(_RefCounted* pRC2)							const { return pRC == pRC2; }
	bool operator ==(const RefCountSmartPointer& rcSmartPtr)	const { return pRC == rcSmartPtr.pRC; }
	bool operator !=(const _RefCounted* pRC2)					const { return pRC != pRC2; }
	bool operator !=(_RefCounted* pRC2)							const { return pRC != pRC2; }
	bool operator !=(const RefCountSmartPointer& rcSmartPtr)	const { return pRC != rcSmartPtr.pRC; }
	bool operator <(const _RefCounted* pRC2)					const { return pRC < pRC2; }
	bool operator >(const _RefCounted* pRC2)					const { return pRC > pRC2; }
};

//------------------------------------------------------------------------------
//	Boolean operators for comparisons with 0 integer value
//		
//------------------------------------------------------------------------------
template <class _RefCounted>
inline bool operator ==(const RefCountSmartPointer<_RefCounted>& rcSmartPtr, int null)	
{
	_DEBUG_ASSERT(null == 0);
	return !(bool)rcSmartPtr;	
}

template <class _RefCounted>
inline bool operator !=(const RefCountSmartPointer<_RefCounted>& rcSmartPtr, int null)
{
	_DEBUG_ASSERT(null == 0);
	return (bool)rcSmartPtr;	
}

template <class _RefCounted>
inline bool operator ==(int null, const RefCountSmartPointer<_RefCounted>& rcSmartPtr)
{
	_DEBUG_ASSERT(null == 0);
	return !(bool)rcSmartPtr;	
}

template <class _RefCounted>
inline bool operator !=(int null, const RefCountSmartPointer<_RefCounted>& rcSmartPtr)
{
	_DEBUG_ASSERT(null == 0);
	return (bool)rcSmartPtr;	
}

//------------------------------------------------------------------------------
//	Utility macro for declaring a smart pointer for a ref counted class
//		
//------------------------------------------------------------------------------
#define _DECLARE_SMARTPTR(i) typedef _NAMESPACE RefCountSmartPointer<i> i##Ptr;


_NAMESPACE_END