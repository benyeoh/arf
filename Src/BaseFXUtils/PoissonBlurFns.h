//================================================================================
//
//		PoissonBlurFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/5/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

IREffect* LoadPoissonEffect(CRCDataKey* pKey, IByteBuffer* pBuffer, IRRenderer* pRenderer, BFXUPoissonBlurParams* pParams, IRTexture2D* pSourceTexture);									
IREffect* LoadPoissonCubeEffect(CRCDataKey* pKey, IByteBuffer* pBuffer, IRRenderer* pRenderer, BFXUPoissonBlurParams* pParams, IRTextureCube* pSourceTexture, eRCubeFace destFace);									

uint GetPoissonBlurEffectTech(IREffect* pBlurEffect, BFXUPoissonBlurParams* pParams);

_NAMESPACE_END
