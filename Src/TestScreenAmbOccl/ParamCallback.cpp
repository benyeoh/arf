//==============================================================================
//
//		ParamCallback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/11/2009
//
//==============================================================================

#include "stdafx.h"

void InitializeEffect(IREffect& effect, uint tech, REffectParam* params)
{
	uint numDynamicParams = effect.GetTemplate()->GetNumOfDynamicParams(tech);

	_LOOPi(numDynamicParams)
	{
		uint semantic = effect.GetTemplate()->GetDynamicParamSemantic(tech, i);
		GetParameter(params[i], semantic);
	}
}

void GetParameter(REffectParam& param, uint semantic)
{
	switch(semantic)
	{
	case NUM_SAMPLES:
		param.SetInt(g_NumSamples);
		break;
	case ALCHEMY_AO_PARAMS:
		param.SetVector4(&g_AlchemyAOParams);
		break;
	case DISTRIBUTED_SAMPLE_PARAMS:
		param.SetVector4(&g_DistributedSampleParams);
		break;
	case CAMERA_POS_WITH_VIEW_FAR:
		param.SetVector4(&g_CameraPosWithViewFar);
		break;
	case FAR_PLANE_OFFSETS:
		param.SetValue(&g_FarPlaneOffsets, sizeof(FarPlaneOffsets));
		break;

	default: _DEBUG_ASSERT(FALSE);
	}
}

