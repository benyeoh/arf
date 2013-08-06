//==============================================================================
//
//		GaussianBlurFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/5/2008
//
//==============================================================================

#include "BaseFXUtils.h"

_NAMESPACE_BEGIN

IREffect* 
LoadGaussianEffect(CRCDataKey* pKey,
				IByteBuffer* pBuffer, 
				IRRenderer* pRenderer,
				BFXUGaussianBlurParams* pParams,
				IRTexture2D* pSourceTexture)
{
	uint blurSemantics[3];
	blurSemantics[0] = BFXU_FILTER_TEXTURE_OFFSETS;
	blurSemantics[1] = BFXU_FILTER_WEIGHTS;
	blurSemantics[2] = BFXU_RT1;
	
	REffectParam blurParams[3];
	_DEBUG_ASSERT(sizeof(gmtl::Vec2f) == (sizeof(float) * 2));
	blurParams[0].SetValue((void*) pParams->GetGaussianOffsets(), BFXU_MAX_FILTER_KERNEL_SIZE * sizeof(gmtl::Vec2f));
	blurParams[1].SetValue((void*) pParams->GetGaussianWeights(), BFXU_MAX_FILTER_KERNEL_SIZE * sizeof(float));
	blurParams[2].SetTexture2D(pSourceTexture);
	IREffect* pGaussianEffect = pRenderer->GetRResourceMgr().LoadEffect(pKey, pBuffer, blurParams, blurSemantics, 3);
	
	return pGaussianEffect;
}

IREffect* 
LoadGaussianEffect(IREffectTemplate* pBlurTemplate, 
                   IRRenderer* pRenderer,
                   BFXUGaussianBlurParams* pParams,
                   IRTexture2D* pSourceTexture)
{
    uint blurSemantics[3];
    blurSemantics[0] = BFXU_FILTER_TEXTURE_OFFSETS;
    blurSemantics[1] = BFXU_FILTER_WEIGHTS;
    blurSemantics[2] = BFXU_RT1;

    REffectParam blurParams[3];
    _DEBUG_ASSERT(sizeof(gmtl::Vec2f) == (sizeof(float) * 2));
    blurParams[0].SetValue((void*) pParams->GetGaussianOffsets(), BFXU_MAX_FILTER_KERNEL_SIZE * sizeof(gmtl::Vec2f));
    blurParams[1].SetValue((void*) pParams->GetGaussianWeights(), BFXU_MAX_FILTER_KERNEL_SIZE * sizeof(float));
    blurParams[2].SetTexture2D(pSourceTexture);
    IREffect* pGaussianEffect = pRenderer->GetRResourceMgr().LoadEffect(pBlurTemplate, blurParams, blurSemantics, 3);

    return pGaussianEffect;
}

uint 
GetGaussianBlurEffectTech(IREffect* pBlurEffect, uint kernelSize)
{
	//float detailLevel = ((float)(kernelSize - BFXU_MIN_FILTER_KERNEL_SIZE)) / 
	//	(BFXU_MAX_FILTER_KERNEL_SIZE - BFXU_MIN_FILTER_KERNEL_SIZE);	

	//uint tech = (uint)(pBlurEffect->GetTemplate()->GetNumOfTechs() * detailLevel + 0.5f);	
	//return tech;

	return kernelSize - BFXU_MIN_FILTER_KERNEL_SIZE;
}


_NAMESPACE_END