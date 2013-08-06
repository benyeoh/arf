//==============================================================================
//
//		CSHFXShadowFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/9/2008
//
//==============================================================================

#pragma once 

#pragma once

_NAMESPACE_BEGIN

class CSHFXShadowFX : public CRefCounted<ISHFXShadowFX>
{
private: 
	IRRendererPtr m_pRenderer;	
	IAppCallback* m_pAppCallback;

	CSHFXResourceMgr	m_ResMgr;
	CSHFXDebug			m_Debug;
	
public: 
	CSHFXShadowFX()
	{
	}

	virtual ~CSHFXShadowFX()
	{
	}

public: 
	boolean Initialize(IRRenderer& renderer, IAppCallback& callback);
	
	ISHFXResourceMgr&	GetResourceMgr()	{ return m_ResMgr; }
	ISHFXDebug&			GetDebug()			{ return m_Debug; }
		
	IRRenderer&		GetRenderer()		{ return *m_pRenderer; }
	IAppCallback&	GetAppCallback()	{ return *m_pAppCallback; }
};

_NAMESPACE_END
