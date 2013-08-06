//==============================================================================
//
//		CSAOFXScreenAmbOcclFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		08/12/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSAOFXScreenAmbOcclFX : public CRefCounted<ISAOFXScreenAmbOcclFX>
{
private:
    IDEFXDeferredFX*    m_pDeferredFX;
    IBFXBaseFX*         m_pBaseFX;
    IRRenderer*         m_pRenderer;
    IAppCallback*       m_pAppCallback;

    CSAOFXPipeline      m_Pipeline;
    //CSAOFXParamPool     m_ParamPool;

public:
	CSAOFXScreenAmbOcclFX()
	{
	}

	virtual ~CSAOFXScreenAmbOcclFX()
	{
	}

public:
    boolean Initialize(IDEFXDeferredFX& deferredFX, IBFXBaseFX& baseFX, IRRenderer& renderer, IAppCallback& callback);

    IBFXBaseFX& GetBaseFX()				{ return *m_pBaseFX; }
    IDEFXDeferredFX& GetDeferredFX()	{ return *m_pDeferredFX; }
    IRRenderer& GetRenderer()			{ return *m_pRenderer; }

//    ISEFXResourceMgr& GetResourceMgr()	{ return m_ResMgr; }
  //  ISAOFXParamPool& GetParamPool()		{ return m_ParamPool; }
    ISAOFXPipeline& GetPipeline()		{ return m_Pipeline; }

    CSAOFXPipeline& GetCPipeline()	    { return m_Pipeline; }
  //  CSAOFXParamPool& GetCParamPool()	{ return m_ParamPool; }
    IAppCallback& GetAppCallback()	{ return *m_pAppCallback; }
};

_NAMESPACE_END
