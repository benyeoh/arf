//==============================================================================
//
//		CMGMaterialGenD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGMaterialGenD3D : public CMGMaterialGen
{
private: 
	
public: 
	CMGMaterialGenD3D()
	{
	}

	virtual ~CMGMaterialGenD3D()
	{
	}

protected:
	void DoInitialize(IAppCallback& callback);
	
public: 

};

_NAMESPACE_END
