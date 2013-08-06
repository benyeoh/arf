//================================================================================
//
//		PoissonBlurFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/5/2012
//
//================================================================================

#include "BaseFXUtils.h"

_NAMESPACE_BEGIN

struct CubeDir
{
	float x, y, z;
};

const static CubeDir s_XCubeAxis[CF_NUM_FACES] =
{
	{0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, -1.0f},

	{1.0f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f},

	{-1.0f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f},
};

const static CubeDir s_YCubeAxis[CF_NUM_FACES] =
{
	{0.0f, 1.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},

	{0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, -1.0f},

	{0.0f, 1.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
};

const static CubeDir s_ZCubeAxis[CF_NUM_FACES] =
{
	{-1.0f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f},

	{0.0f, -1.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},

	{0.0f, 0.0f, -1.0f},
	{0.0f, 0.0f, 1.0f},
};


IREffect* LoadPoissonEffect(CRCDataKey* pKey, IByteBuffer* pBuffer, IRRenderer* pRenderer, BFXUPoissonBlurParams* pParams, IRTexture2D* pSourceTexture)
{
	IREffect* pPoissonEffect;

	if(pParams->GetNoiseTex())
	{
		uint blurSemantics[4];
		blurSemantics[0] = BFXU_POISSON_DISK_SCALE;
		blurSemantics[1] = BFXU_NOISE_SCALE;
		blurSemantics[2] = BFXU_RT1;
		blurSemantics[3] = BFXU_RT2;

		REffectParam blurParams[4];
		blurParams[0].SetFloat(pParams->GetScale());
		blurParams[1].SetVector2(pParams->GetNoiseScale());
		blurParams[2].SetTexture2D(pSourceTexture);
		blurParams[3].SetTexture2D(pParams->GetNoiseTex());

		pPoissonEffect = pRenderer->GetRResourceMgr().LoadEffect(pKey, pBuffer, blurParams, blurSemantics, 4);
	}
	else
	{
		uint blurSemantics[2];
		blurSemantics[0] = BFXU_POISSON_DISK_SCALE;
		blurSemantics[1] = BFXU_RT1;
		
		REffectParam blurParams[2];
		blurParams[0].SetFloat(pParams->GetScale());
		blurParams[1].SetTexture2D(pSourceTexture);
	
		pPoissonEffect = pRenderer->GetRResourceMgr().LoadEffect(pKey, pBuffer, blurParams, blurSemantics, 2);
	}

	return pPoissonEffect;
}

IREffect* LoadPoissonCubeEffect(CRCDataKey* pKey, IByteBuffer* pBuffer, IRRenderer* pRenderer, BFXUPoissonBlurParams* pParams, IRTextureCube* pSourceTexture, eRCubeFace destFace)
{
	IREffect* pPoissonEffect;

	if(pParams->GetNoiseTex())
	{
		uint blurSemantics[7];
		blurSemantics[0] = BFXU_POISSON_DISK_SCALE;
		blurSemantics[1] = BFXU_NOISE_SCALE;
		blurSemantics[2] = BFXU_RT1;
		blurSemantics[3] = BFXU_RT2;
		blurSemantics[4] = BFXU_X_CUBE_AXIS;
		blurSemantics[5] = BFXU_Y_CUBE_AXIS;
		blurSemantics[6] = BFXU_Z_CUBE_AXIS;

		REffectParam blurParams[7];
		blurParams[0].SetFloat(pParams->GetScale());
		blurParams[1].SetVector2(pParams->GetNoiseScale());
		blurParams[2].SetTextureCube(pSourceTexture);
		blurParams[3].SetTexture2D(pParams->GetNoiseTex());
		blurParams[4].SetVector3( (gmtl::Vec3f*) &(s_XCubeAxis[destFace]) );
		blurParams[5].SetVector3( (gmtl::Vec3f*) &(s_YCubeAxis[destFace]) );
		blurParams[6].SetVector3( (gmtl::Vec3f*) &(s_ZCubeAxis[destFace]) );

		pPoissonEffect = pRenderer->GetRResourceMgr().LoadEffect(pKey, pBuffer, blurParams, blurSemantics, 7);
	}
	else
	{
		uint blurSemantics[5];
		blurSemantics[0] = BFXU_POISSON_DISK_SCALE;
		blurSemantics[1] = BFXU_RT1;
		blurSemantics[2] = BFXU_X_CUBE_AXIS;
		blurSemantics[3] = BFXU_Y_CUBE_AXIS;
		blurSemantics[4] = BFXU_Z_CUBE_AXIS;

		REffectParam blurParams[5];
		blurParams[0].SetFloat(pParams->GetScale());
		blurParams[1].SetTextureCube(pSourceTexture);
		blurParams[2].SetVector3( (gmtl::Vec3f*) &(s_XCubeAxis[destFace]) );
		blurParams[3].SetVector3( (gmtl::Vec3f*) &(s_YCubeAxis[destFace]) );
		blurParams[4].SetVector3( (gmtl::Vec3f*) &(s_ZCubeAxis[destFace]) );

		pPoissonEffect = pRenderer->GetRResourceMgr().LoadEffect(pKey, pBuffer, blurParams, blurSemantics, 5);
	}

	return pPoissonEffect;
}

uint GetPoissonBlurEffectTech(IREffect* pBlurEffect, BFXUPoissonBlurParams* pParams)
{
	if(pParams->GetNoiseTex())
		return (uint) pParams->GetPoissonSamples() + MAX_NUM_POISSON_SAMPLES;

	return (uint) pParams->GetPoissonSamples();
}

_NAMESPACE_END