//==============================================================================
//
//		CREffectTemplate.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Wednesday, July 19, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

struct REffectState
{
	int tech;
	int pass;
};

class CREffectTemplate : public CRResource< IREffectTemplate >
{
protected:
	CRRenderer* m_pRenderer;
	DenseChainedHashMap<uint, int, NullUintHash> m_SemanticToIndexTable;
	
public:
	CREffectTemplate(CRRenderer* pRenderer)
		: m_pRenderer(pRenderer)
	{
		m_SemanticToIndexTable.Initialize(64, 0.7f);
	}

	virtual ~CREffectTemplate()
	{
	}

protected:
	_PURE( uint DoGetNumOfInstancedParams(uint techIndex) )
	_PURE( uint DoGetInstancedParamSemantic(uint techIndex, uint paramIndex) )
	_PURE( uint DoGetInstancedParamNumDesc(uint techIndex, uint paramIndex) )
	_PURE( REffectParam DoGetInstancedParamDesc(uint techIndex, uint paramIndex, uint descIndex) )
	_PURE( const wchar* DoGetInstancedParamDescName(uint techIndex, uint paramIndex, uint descIndex) )

	_PURE( uint DoGetNumOfDynamicParams(uint techIndex) )
	_PURE( uint DoGetDynamicParamSemantic(uint techIndex, uint paramIndex) )
	_PURE( eREffectParamType DoGetDynamicParamType(uint techIndex, uint paramIndex) )
		
	_PURE( uint DoGetNumOfConstantParams() )
	_PURE( int DoGetConstantParamIndex(uint semantic) )
	_PURE( eREffectParamType DoGetConstantParamType(uint paramIndex) )
	_PURE( uint DoGetConstantParamSemantic(uint paramIndex) )

	_PURE( void DoApplyDynamicParams(const REffectState& state, const REffectParam* pEffectParams, const REffectParam* pToCompare) )
	_PURE( void DoApplyDynamicParams(const REffectState& state, const REffectParam* pEffectParams) )
	_PURE( void DoApplyConstantParams(const REffectState& state, const REffectParam* pEffectParams) )
	_PURE( void DoApplyInstancedParams(const REffectState& state, const REffectParam* pEffectParams) )

	_PURE( void DoBeginTechnique(const REffectState& state) )
	_PURE( void DoBeginPass(const REffectState& state) )
	_PURE( void DoEndTechnique() )
	_PURE( void DoEndPass() )
	_PURE( uint DoGetNumOfPasses(uint techIndex) )
	_PURE( uint DoGetNumOfTechs() )

	_PURE( uint DoGetConstantParamNumDesc(uint paramIndex) )
	_PURE( REffectParam DoGetConstantParamDesc(uint paramIndex, uint descIndex) )
	_PURE( const wchar* DoGetConstantParamDescName(uint paramIndex, uint descIndex) )
	_PURE( REffectParam DoGetConstantParamDefaultValue(uint paramIndex) )
	
public:
	uint GetNumOfInstancedParams(uint techIndex);
	uint GetInstancedParamSemantic(uint techIndex, uint paramIndex);
	uint GetInstancedParamNumDesc(uint techIndex, uint paramIndex);
	REffectParam GetInstancedParamDesc(uint techIndex, uint paramIndex, uint descIndex);
	const wchar* GetInstancedParamDescName(uint techIndex, uint paramIndex, uint descIndex);
	
	uint GetNumOfDynamicParams(uint techIndex);
	uint GetDynamicParamSemantic(uint techIndex, uint paramIndex);
	eREffectParamType GetDynamicParamType(uint techIndex, uint paramIndex);
	
	uint GetNumOfConstantParams();
	int GetConstantParamIndex(uint semantic);
	uint GetConstantParamSemantic(uint paramIndex);
	eREffectParamType GetConstantParamType(uint paramIndex);
	uint GetConstantParamNumDesc(uint paramIndex);
	REffectParam GetConstantParamDesc(uint paramIndex, uint descIndex);
	const wchar* GetConstantParamDescName(uint paramIndex, uint descIndex);
	REffectParam GetConstantParamDefaultValue(uint paramIndex);

	void ApplyConstantParams(const REffectState& state, const REffectParam* pEffectParams);
	void ApplyDynamicParams(const REffectState& state, const REffectParam* pEffectParams, const REffectParam* pToCompare);
	void ApplyDynamicParams(const REffectState& state, const REffectParam* pEffectParams);
	void ApplyInstancedParams(const REffectState& state, const REffectParam* pEffectParams);

	void BeginTechnique(const REffectState& state);
	void BeginPass(const REffectState& state);
	
	void EndTechnique();
	void EndPass();
	
	uint GetNumOfPasses(uint techIndex);
	uint GetNumOfTechs();
	
	////int GetEffectType();
	//REffectParam GetEffectDesc(uint index);
	//const wchar* GetEffectDescName(uint index);
	//uint GetNumEffectDesc();
	
	virtual void OnLostDevice()
	{
	}

	//------------------------------------------------------------------------------
	//	OnResetDevice()
	//		Handles the case when the render device is (re)initialized	
	//------------------------------------------------------------------------------
	virtual void OnResetDevice()
	{
	}

	//------------------------------------------------------------------------------
	//	OnRestoreDevice()
	//		Handles the case when the render device context is regained after
	//		losing it
	//------------------------------------------------------------------------------
	virtual void OnRestoreDevice()
	{
	}
};

_DECLARE_SMARTPTR(CREffectTemplate)

_NAMESPACE_END