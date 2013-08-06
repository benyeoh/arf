//==============================================================================
//
//		GaussianBlurFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/5/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

IREffect* 
LoadGaussianEffect(CRCDataKey* pKey,
			    IByteBuffer* pBuffer, 
			    IRRenderer* pRenderer,
			    BFXUGaussianBlurParams* pParams,
			    IRTexture2D* pSourceTexture);									

IREffect* 
LoadGaussianEffect(IREffectTemplate* pBlurTemplate, 
                   IRRenderer* pRenderer,
                   BFXUGaussianBlurParams* pParams,
                   IRTexture2D* pSourceTexture);

uint GetGaussianBlurEffectTech(IREffect* pBlurEffect, uint kernelSize);
												
_NAMESPACE_END
