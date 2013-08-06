//==============================================================================
//
//		CDEFXDeferredFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CDEFXDeferredFX : public CRefCounted<IDEFXDeferredFX>
{
private:
	//IBFXBaseFX* m_pBaseFX;
	IRRenderer* m_pRenderer;

	IAppCallback* m_pAppCallback;

	CDEFXResourceMgr	m_ResMgr;

public:
	CDEFXDeferredFX()
		: m_pRenderer(NULL)
		, m_pAppCallback(NULL)
	{
	}

	virtual ~CDEFXDeferredFX()
	{
	}
    
public:
	boolean Initialize(IRRenderer& renderer, IAppCallback& callback);

	//IBFXBaseFX& GetBaseFX()		{ return *m_pBaseFX; }
	IRRenderer& GetRenderer()	{ return *m_pRenderer; }

	//IDEFXPipeline& GetPipeline() { return m_Pipeline; }
	//IDEFXParamPool& GetParamPool() { return m_ParamPool; }
	IDEFXResourceMgr& GetResourceMgr() { return m_ResMgr; }

	IAppCallback& GetAppCallback() { return *m_pAppCallback; }
};

_NAMESPACE_END
