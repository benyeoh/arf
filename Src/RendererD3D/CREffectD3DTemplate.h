//==============================================================================
//
//		CREffectD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct REffectDesc
{
	wstring name;
	REffectParamStore param;
};

struct REffectParamD3D
{
	eREffectParamType type;
	uint semantic;
	wstring paramName;
	std::vector< REffectDesc > desc;
	REffectParamStore defaultValue;
	// wstring semanticName;
	D3DXHANDLE hParam;
};

typedef std::vector< uint > ParamIndexList;
typedef std::vector< REffectParamD3D > ParamList;

struct REffectPassD3D
{
	D3DXHANDLE hPass;
};

typedef std::vector< REffectPassD3D > PassList;

struct REffectTechD3D
{
	D3DXHANDLE hTech;
//	uint numParamsBatcheable;	
	PassList passes;
	ParamIndexList varyingParams;
	ParamIndexList constantParams;
	ParamIndexList instancedParams;
};

typedef std::vector< REffectTechD3D > TechList;
typedef std::vector< REffectDesc > EffectDescList;

class CREffectD3DTemplate : public CREffectTemplate
{
private:
	TechList m_Techniques;
	ID3DXEffectPtr m_pD3DEffect;
	
	//uint m_NumOfParamsD3D;
	ParamList m_D3DDynamicParams;
	ParamList m_D3DConstantParams;
	ParamList m_D3DInstancedParams;
	//EffectDescList m_EffectDesc;
		
	RD3DEffectStateManagerPtr m_pStateManager;

	//------------------------------------------------------------------------------
	// D3D Instancing specific stuff
	//------------------------------------------------------------------------------
	const static uint MAX_INSTANCE_DESC = 32;
	
	struct InstancingSetup
	{
		uint instancedParamSize;
		uint instancedParamDescPacked[4];
		uint instancedParamHash;
		RVertexDesc	instancedDesc[MAX_INSTANCE_DESC];
	};
	
	std::vector<InstancingSetup> m_InstancingSetups;
		
public:
	CREffectD3DTemplate(CRRenderer* pRenderer)
		: CREffectTemplate(pRenderer)
		, m_pStateManager(NULL)
	{
	}
	
	virtual ~CREffectD3DTemplate()
	{
	}

private:
	void ProcessInstancedParams();

	void ParseEffect();
	void ParseAllParams(const D3DXEFFECT_DESC& desc);
	void ParseTechniques(const D3DXEFFECT_DESC& desc);
	void ParsePasses(REffectTechD3D& tech);
	//void ParseEffectDesc(REffectParamD3D& param);
	REffectDesc ParseParamEffectDesc(D3DXHANDLE hAnnotation);
	
protected:
	uint DoGetNumOfInstancedParams(uint techIndex);
	uint DoGetInstancedParamSemantic(uint techIndex, uint paramIndex);
	uint DoGetInstancedParamNumDesc(uint techIndex, uint paramIndex);
	REffectParam DoGetInstancedParamDesc(uint techIndex, uint paramIndex, uint descIndex);
	const wchar* DoGetInstancedParamDescName(uint techIndex, uint paramIndex, uint descIndex);

	uint DoGetNumOfDynamicParams(uint techIndex);
	uint DoGetDynamicParamSemantic(uint techIndex, uint paramIndex);
	eREffectParamType DoGetDynamicParamType(uint techIndex, uint paramIndex);
	
	uint DoGetNumOfConstantParams();
	int DoGetConstantParamIndex(uint semantic);
	uint DoGetConstantParamSemantic(uint paramIndex);
	REffectParam DoGetConstantParamDefaultValue(uint paramIndex);
	
	eREffectParamType DoGetConstantParamType(uint paramIndex);
	uint DoGetConstantParamNumDesc(uint paramIndex);
	REffectParam DoGetConstantParamDesc(uint paramIndex, uint descIndex);
	const wchar* DoGetConstantParamDescName(uint paramIndex, uint descIndex);

	void DoApplyConstantParams(const REffectState& state, const REffectParam* pEffectParams);
	void DoApplyDynamicParams(const REffectState& state, const REffectParam* pEffectParams, const REffectParam* pToCompare);
	void DoApplyDynamicParams(const REffectState& state, const REffectParam* pEffectParams);
	void DoApplyInstancedParams(const REffectState& state, const REffectParam* pEffectParams);

	void DoBeginTechnique(const REffectState& state);
	void DoBeginPass(const REffectState& state);
	
	void DoEndTechnique();
	void DoEndPass();
	uint DoGetNumOfPasses(uint techIndex);
	uint DoGetNumOfTechs();

	//REffectParam DoGetEffectDesc(uint index);
	//const wchar* DoGetEffectDescName(uint index);
	//uint DoGetNumEffectDesc();
	
public:
	void SetupInstancing(CRVertexBufferGroupD3D* pVBGroup, uint techIndex);
	void SetD3DEffect(ID3DXEffect* pD3DEffect);	
	
	void OnLostDevice();
	void OnResetDevice();
	void OnRestoreDevice();
};

_NAMESPACE_END
