//==============================================================================
//
//		CSEFXSHExpLightingFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSEFXSHExpLightingFX : public CRefCounted<ISEFXSHExpLightingFX>
{
private:
	IDEFXDeferredFX*	m_pDeferredFX;
	IBFXBaseFX*			m_pBaseFX;
	IRRenderer*			m_pRenderer;
	IAppCallback*		m_pAppCallback;

	CSEFXPipeline		m_Pipeline;
	CSEFXResourceMgr	m_ResMgr;
	CSEFXParamPool		m_ParamPool;

public:
	CSEFXSHExpLightingFX()
		: m_pBaseFX(NULL)
		, m_pRenderer(NULL)
		, m_pAppCallback(NULL)
	{
	}

	virtual ~CSEFXSHExpLightingFX()
	{
	}

public:
	boolean Initialize(IDEFXDeferredFX& deferredFX, IBFXBaseFX& baseFX, IRRenderer& renderer, IAppCallback& callback);

	IBFXBaseFX& GetBaseFX()				{ return *m_pBaseFX; }
	IDEFXDeferredFX& GetDeferredFX()	{ return *m_pDeferredFX; }
	IRRenderer& GetRenderer()			{ return *m_pRenderer; }

	ISEFXResourceMgr& GetResourceMgr()	{ return m_ResMgr; }
	ISEFXParamPool& GetParamPool()		{ return m_ParamPool; }
	ISEFXPipeline& GetPipeline()		{ return m_Pipeline; }

	CSEFXPipeline& GetCPipeline()	{ return m_Pipeline; }
	CSEFXParamPool& GetCParamPool()	{ return m_ParamPool; }
	IAppCallback& GetAppCallback()	{ return *m_pAppCallback; }
};

_NAMESPACE_END
