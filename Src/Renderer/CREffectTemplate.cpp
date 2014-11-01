//==============================================================================
//
//		CREffectTemplate.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Wednesday, July 19, 2006
//
//==============================================================================

#include "RendererFramework.h"


_NAMESPACE_BEGIN

uint 
CREffectTemplate::GetNumOfInstancedParams(uint techIndex)
{
	return DoGetNumOfInstancedParams(techIndex);
}
	
uint 
CREffectTemplate::GetInstancedParamSemantic(uint techIndex, uint paramIndex)
{
	return DoGetInstancedParamSemantic(techIndex, paramIndex);
}
	
void 
CREffectTemplate::ApplyInstancedParams(const REffectState& state, const REffectParam* pEffectParams)
{
	DoApplyInstancedParams(state, pEffectParams);
}
	
uint 
CREffectTemplate::GetNumOfDynamicParams(uint techIndex)
{
	return DoGetNumOfDynamicParams(techIndex);
}

uint 
CREffectTemplate::GetDynamicParamSemantic(uint techIndex, uint paramIndex)
{
	return DoGetDynamicParamSemantic(techIndex, paramIndex);
}

eREffectParamType 
CREffectTemplate::GetDynamicParamType(uint techIndex, uint paramIndex)
{
	return DoGetDynamicParamType(techIndex, paramIndex);
}

void 
CREffectTemplate::ApplyDynamicParams(const REffectState& state, 
									 const REffectParam* pEffectParams, 
									 const REffectParam* pToCompare)
{
	DoApplyDynamicParams(state, pEffectParams, pToCompare);
	_INC_METRIC(m_pRenderer, numApplyDynamicParam);
}

void 
CREffectTemplate::ApplyDynamicParams(const REffectState& state, const REffectParam* pEffectParams)
{
	DoApplyDynamicParams(state, pEffectParams);
	_INC_METRIC(m_pRenderer, numApplyDynamicParam);
}
	
uint 
CREffectTemplate::GetNumOfConstantParams()
{
	return DoGetNumOfConstantParams();
}

uint 
CREffectTemplate::GetConstantParamSemantic(uint paramIndex)
{
	return DoGetConstantParamSemantic(paramIndex);
}

void 
CREffectTemplate::ApplyConstantParams(const REffectState& state, const REffectParam* pEffectParams)
{
	DoApplyConstantParams(state, pEffectParams);
	_INC_METRIC(m_pRenderer, numApplyConstantParam);
}

eREffectParamType 
CREffectTemplate::GetConstantParamType(uint paramIndex)
{
	return DoGetConstantParamType(paramIndex);
}

int 
CREffectTemplate::GetConstantParamIndex(uint semantic)
{
	int* pParamIndex = m_SemanticToIndexTable.Get(semantic);
	if(pParamIndex)
		return *pParamIndex;	

	int paramIndex = DoGetConstantParamIndex(semantic);
	m_SemanticToIndexTable.Add(semantic, paramIndex);
	return paramIndex;
			
	//dense_hash_map<uint, int>::iterator iterVal = m_SemanticToIndexTable.find(semantic);
	//int paramIndex = 0;
	//if(iterVal != m_SemanticToIndexTable.end())
	//{
	//	paramIndex = (iterVal->second);
	//	_DEBUG_ASSERT(m_SemanticToIndexTable[semantic] == paramIndex);
	//}	
	//else
	//{
	//	paramIndex = DoGetConstantParamIndex(semantic);
	//	m_SemanticToIndexTable[semantic] = paramIndex;
	//	_DEBUG_ASSERT(m_SemanticToIndexTable[semantic] == paramIndex);
	//}
	//
	//return paramIndex;
}

//uint 
//CREffectTemplate::GetNumOfParams(uint techIndex)
//{
//	return DoGetNumOfParams(techIndex);
//}
//
//uint 
//CREffectTemplate::GetParamSemantic(uint techIndex, uint paramIndex)
//{
//	return DoGetParamSemantic(techIndex, paramIndex);
//}
//
//void 
//CREffectTemplate::Apply(const REffectParam* pEffectParams, uint length, const REffectParam* pToCompare)
//{
//	DoApply(pEffectParams, length, pToCompare);
//}

void 
CREffectTemplate::BeginTechnique(const REffectState& state)
{
	DoBeginTechnique(state);
	_INC_METRIC(m_pRenderer, numBeginTechnique);
}

void 
CREffectTemplate::BeginPass(const REffectState& state)
{
	DoBeginPass(state);
	_INC_METRIC(m_pRenderer, numBeginPass);
}

void 
CREffectTemplate::EndTechnique()
{
	DoEndTechnique();
}

void 
CREffectTemplate::EndPass()
{
	DoEndPass();
}

uint 
CREffectTemplate::GetNumOfPasses(uint techIndex)
{
	return DoGetNumOfPasses(techIndex);
}

uint 
CREffectTemplate::GetNumOfTechs()
{
	return DoGetNumOfTechs();
}

REffectParam 
CREffectTemplate::GetConstantParamDefaultValue(uint paramIndex)
{
	return DoGetConstantParamDefaultValue(paramIndex);
}

//int 
//CREffectTemplate::GetEffectType()
//{
//	return DoGetEffectType();
//}

//REffectParam 
//CREffectTemplate::GetEffectDesc(uint index)
//{
//	return DoGetEffectDesc(index);
//}
//
//const wchar*
//CREffectTemplate::GetEffectDescName(uint index)
//{
//	return DoGetEffectDescName(index);
//}
//
//uint 
//CREffectTemplate::GetNumEffectDesc()
//{
//	return DoGetNumEffectDesc();
//}

uint 
CREffectTemplate::GetConstantParamNumDesc(uint paramIndex)
{
	return DoGetConstantParamNumDesc(paramIndex);
}

REffectParam 
CREffectTemplate::GetConstantParamDesc(uint paramIndex, uint descIndex)
{
	return DoGetConstantParamDesc(paramIndex, descIndex);
}

const wchar* 
CREffectTemplate::GetConstantParamDescName(uint paramIndex, uint descIndex)
{
	return DoGetConstantParamDescName(paramIndex, descIndex);
}

uint 
CREffectTemplate::GetInstancedParamNumDesc(uint techIndex, uint paramIndex)
{
	return DoGetInstancedParamNumDesc(techIndex, paramIndex);
}
	
REffectParam 
CREffectTemplate::GetInstancedParamDesc(uint techIndex, uint paramIndex, uint descIndex)
{
	return DoGetInstancedParamDesc(techIndex, paramIndex, descIndex);
}
	
const wchar* 
CREffectTemplate::GetInstancedParamDescName(uint techIndex, uint paramIndex, uint descIndex)
{
	return DoGetInstancedParamDescName(techIndex, paramIndex, descIndex);
}


_NAMESPACE_END