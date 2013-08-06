//==============================================================================
//
//		CMGMaterialGen.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/3/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGMaterialGen : public CRefCounted<IMGMaterialGen>
{
protected: 
	CMGResourceMgr* m_pResMgr;
	IAppCallback* m_pCallback;
	
public: 
	CMGMaterialGen()
		: m_pResMgr(NULL)
		, m_pCallback(NULL)
	{
	}

	virtual ~CMGMaterialGen()
	{
		_DELETE(m_pResMgr);
	}

protected:
	_PURE( void DoInitialize(IAppCallback& callback) )
	
public: 
	void Initialize(IAppCallback& callback);
	IMGResourceMgr& GetResourceMgr() { return *m_pResMgr; }	
	IAppCallback& GetAppCallback() { return *m_pCallback; }
};

_NAMESPACE_END
