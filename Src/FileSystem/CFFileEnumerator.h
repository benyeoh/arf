//==============================================================================
//
//		CFFileEnumerator.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/6/2007
//
//==============================================================================


#pragma once

_NAMESPACE_BEGIN

class CFFileEnumerator : public CRefCounted<IFFileEnumerator>
{
private: 
	
public: 
	CFFileEnumerator()
		: CRefCounted<IFFileEnumerator>()
	{
	}

	virtual ~CFFileEnumerator()
	{
	}

protected:
	_PURE( IFFile* DoGetNextFile() )
	
public: 
	IFFile* GetNextFile();
	
};

_NAMESPACE_END
 