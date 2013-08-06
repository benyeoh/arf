//==============================================================================
//
//		CBLFXPipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/20/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBLFXPipeline : public IBLFXPipeline, public IRRenderCallback
{
	//typedef std::vector< BFXPipelineOp > PipelineOpVec;
	//	typedef std::vector< RFXLightingTranslucent* > TranslucentVec;

private:	
	//PipelineOpVec m_Opaque;
	//int m_OpaqueGroupIndex;
	//PipelineOpVec m_Trans;
	//int m_TransGroupIndex;
	//PipelineOpVec m_OpaqueAdditive;
	//int m_OpaqueAdditiveGroupIndex;
	////PipelineOpVec m_AlphaTested;
	////int m_AlphaTestedGroupIndex;

	//IREffectPtr m_pTransSetupEffect;
	//IREffectPtr m_pOpaqueSetupEffect;
	//IREffectPtr m_pOpaqueAdditiveSetupEffect;
	//IREffectPtr m_pAlphaTestedSetupEffect;

	IREffectPtr m_p2DPipeSetupEffect;
	IREffectPtr m_pAdditivePipeSetupEffect;
	
	CBLFXBloomFX* m_pBloomFX;
	//uint m_NumEffectsSoFar;

public: 
	CBLFXPipeline()
		: m_pBloomFX(NULL)
	{
	}

	virtual ~CBLFXPipeline()
	{
	}

public: 
	void SetBloomFX(CBLFXBloomFX* pBloom) { m_pBloomFX = pBloom; }
	void Initialize();
			
	void BeginRenderGroup(uint groupNum);
	void EndRenderGroup(uint groupNum);

	void BeginRenderOp(const RRenderOp* pSortData);
	void EndRenderOp(const RRenderOp* pSortData);

};

_NAMESPACE_END
