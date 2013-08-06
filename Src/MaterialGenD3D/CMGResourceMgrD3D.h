//==============================================================================
//
//		CMGResourceMgrD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGResourceMgrD3D : public CMGResourceMgr
{
private: 
	
public: 
	CMGResourceMgrD3D(CMGMaterialGen* pGen)
		 : CMGResourceMgr(pGen)
	{
	}

	virtual ~CMGResourceMgrD3D()
	{
	}

protected:
	IMGMaterialTemplate* DoCreateMaterialTemplate();
};

_NAMESPACE_END
