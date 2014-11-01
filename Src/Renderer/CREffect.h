//==============================================================================
//
//		CREffect.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, March 25, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CREffect : public CRResource< IREffect >
{
private:
	typedef std::vector<REffectParam> ParamList;
	typedef std::vector<REffectParamStore> StoreList;
	
private:
	CREffectTemplatePtr m_pTemplate;
	ParamList m_ConstantParams;
	StoreList m_ConstantStore;
	wstring m_EffectFileName;
			
public:	
	CREffect();
	CREffect(const CREffect& effect);
	
	virtual ~CREffect();
	
public:
	uint GetNumOfTechs() const;
	//int GetEffectType();	
		
	void SetTemplate(CREffectTemplate* pTemplate);	
	IREffectTemplate* GetTemplate();	
	
	void ApplyRenderState(uint techIndex, REffectParam* pDynamicParams);
	void ResetRenderState();

	void SetParam(const REffectParam& param, uint index);
	uint GetNumOfParams() const;
	const REffectParam* GetParam(uint index) const;
	const REffectParamStore* GetParamStore(uint index) const;
	
	const REffectParam* GetParams() const
	{
		if(GetNumOfParams() > 0)
			return &(m_ConstantParams[0]);
		
		return NULL;
	}

	void SetTemplateFileName(const wchar* pFileName)
	{
		m_EffectFileName = pFileName;
	}
	
	const wchar* GetTemplateFileName() const
	{
		return m_EffectFileName.c_str();		
	}
	

	boolean operator ==(const CREffect& toCompare) const ;
	
	CREffect& operator =(const CREffect& toAssign);
};

_NAMESPACE_END