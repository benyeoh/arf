//==============================================================================
//
//		CBLFXParamPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/20/2008
//
//==============================================================================

#pragma once  

_NAMESPACE_BEGIN

class CBLFXParamPool : public IBLFXParamPool
{
private:
	float m_Exposure;
	float m_BloomThreshold;
	float m_BloomOffset;
	
	CBLFXBloomFX* m_pBloomFX;	
	
public:
	CBLFXParamPool()
		: m_pBloomFX(NULL)
		, m_Exposure(1.0f)
		, m_BloomThreshold(0.48f)
		//, m_BloomOffset(0.0f)
	{

	}
	
	~CBLFXParamPool()
	{
	}
	
public:
	void SetBloomFX(CBLFXBloomFX* pBloomFX) { m_pBloomFX = pBloomFX; }
	uint InitializeEffect(IREffect& effect, float detailLevel, REffectParam* params);

	void SetExposure(float val);
	float GetExposure();

	//void SetBloomOffset(float val);
	//float GetBloomOffset();

	void SetBloomThreshold(float val);
	float GetBloomThreshold();

	void GetParameter(REffectParam& param, uint semantic);
};

_NAMESPACE_END