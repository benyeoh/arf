//==============================================================================
//
//		CBLFXParamPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/21/2008
//
//==============================================================================

#include "BloomFXFramework.h"

_NAMESPACE_BEGIN

void 
CBLFXParamPool::SetExposure(float val)
{
	m_Exposure = val;
}

float 
CBLFXParamPool::GetExposure()	
{
	return m_Exposure;
}

void 
CBLFXParamPool::SetBloomThreshold(float val)
{
	m_BloomThreshold = val;
}

float 
CBLFXParamPool::GetBloomThreshold()	
{
	return m_BloomThreshold;
}

//void 
//CBLFXParamPool::SetBloomOffset(float val)
//{
//	m_BloomOffset = val;
//}

//float 
//CBLFXParamPool::GetBloomOffset()	
//{
//	return m_BloomOffset;
//}


uint 
CBLFXParamPool::InitializeEffect(IREffect& effect, float detailLevel, REffectParam* params)
{
	uint tech = (uint)((effect.GetNumOfTechs()-1) * detailLevel + 0.5f);
	uint numDynamicParams = effect.GetTemplate()->GetNumOfDynamicParams(tech);
	
	_LOOPi(numDynamicParams)
	{
		uint semantic = effect.GetTemplate()->GetDynamicParamSemantic(tech, i);
		GetParameter(params[i], semantic);
	}
	
	return tech;
}

void 
CBLFXParamPool::GetParameter(REffectParam& param, uint semantic)
{
	switch(semantic)
	{
		case BLFX_BLOOM_THRESHOLD:
		{	
			param.SetFloat(GetBloomThreshold());
			break;
		}

		case BLFX_EXPOSURE:
		{
			param.SetFloat(GetExposure());
			break;
		}

		//case BLFX_BLOOM_OFFSET:
		//{
		//	param.SetFloat(GetBloomOffset());
		//	break;
		//}
		
		default:
		{
			_DEBUG_ASSERT(FALSE);
			break;
		}
	}	
}


//void 
//CRFXCommon::SetExternalParam(const REffectParam& param, uint semantic)
//{
//	_DEBUG_ASSERT(semantic > RFX_EXTERNAL_DYNAMIC_SEMANTIC_BEGIN);
//	_DEBUG_ASSERT(semantic < RFX_EXTERNAL_DYNAMIC_SEMANTIC_END);
//	m_ExternalParams[semantic - RFX_EXTERNAL_DYNAMIC_SEMANTIC_BEGIN - 1] = param;	
//}
//
//const REffectParam& 
//CRFXCommon::GetExternalParam(uint semantic)
//{
//	_DEBUG_ASSERT(semantic > RFX_EXTERNAL_DYNAMIC_SEMANTIC_BEGIN);
//	_DEBUG_ASSERT(semantic < RFX_EXTERNAL_DYNAMIC_SEMANTIC_END);
//	return m_ExternalParams[semantic - RFX_EXTERNAL_DYNAMIC_SEMANTIC_BEGIN - 1];	
//}

_NAMESPACE_END