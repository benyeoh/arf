//==============================================================================
//
//		CBFXBaseFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/21/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXBaseFX : public CRefCounted<IBFXBaseFX>
{	
private: 
	IAppCallback* m_pCallback;
	IRRendererPtr m_pRenderer;
	IMGMaterialGenPtr m_pMatGen;
	
	CBFXResourceMgr m_ResMgr;
	//CBFXParamPool	m_ParamPool;
	//CBFXPipeline	m_Pipeline;
	CBFXDebug		m_Debug;
	
	//IBFXRenderCallback*	m_pMaterialRenderCallbacks[BFX_MAX_MATERIAL_TYPES];	

public: 
	CBFXBaseFX()
	{
		//memset(m_pMaterialRenderCallbacks, 0, sizeof(m_pMaterialRenderCallbacks));
	}

	virtual ~CBFXBaseFX()
	{
	}

public: 
	boolean Initialize(IRRenderer& renderer, IMGMaterialGen& matGen, IAppCallback& callback);

	IMGMaterialGen& GetMaterialGen() { return *m_pMatGen; }
	IRRenderer& GetRenderer() { return *m_pRenderer; }
	IAppCallback& GetAppCallback() { return *m_pCallback; }
	
	IBFXResourceMgr& GetResourceMgr() { return m_ResMgr; }
	//IBFXParamPool& GetParamPool() { return m_ParamPool; }
	//IBFXDebug& GetDebug() { return m_Debug; }
	
	//IBFXPipeline& GetPipeline() { return  m_Pipeline; }
	//CBFXPipeline& GetCPipeline() { return  m_Pipeline; }
	
    //void GetDynamicParams(IREffect* pEffect, uint tech, REffectParam* pStore);

	void Render(BFXMesh& mesh, float detailLevel, uint matType, uint pipelineMatType, BFXParamContainer& paramContainer, BFXRenderContainer& renderContainer, IBFXMaterialCreateCallback* pCreateCallback);
	//void Render(BFXMesh& mesh, float detailLevel, uint matType, uint libIndex, BFXParamContainer& paramContainer, BFXRenderContainer& renderContainer, uint* pUncachedAvailFlags, uint& numUncachedFlags);
	
	//void SetParamCallback(IBFXParamCallback* pModule, uint offsetStart);
	//void SetRenderCallback(IBFXRenderCallback* pModule, uint materialType);

	//IBFXParamCallback* GetParamCallback(uint semantic);
};

_NAMESPACE_END
