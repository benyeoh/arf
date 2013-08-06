//==============================================================================
//
//		CSHFXPipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/11/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSHFXPipeline : public CRefCounted<ISHFXPipeline>
{
private: 	
	CSHFXShadowFX* m_pShadowFX;

public: 
	CSHFXPipeline()
		: m_pShadowFX(NULL)
	{
	}

	virtual ~CSHFXPipeline()
	{
	}

public: 
	void SetShadowFX(CSHFXShadowFX* pShadowFX) { m_pShadowFX = pShadowFX; }
	void Initialize();
	
	void RenderEffect(BFXRenderContext& renderContext);
	
	void RenderCascadeDirLightCaster(BFXRenderContext& context);																	
	void RenderPointLightCaster(BFXRenderContext& context);
	void RenderCubePointLightCaster(BFXRenderContext& context);
};

_NAMESPACE_END
