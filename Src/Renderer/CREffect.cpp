//==============================================================================
//
//		CREffect.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Friday, April 07, 2006
//
//==============================================================================

#include "RendererFramework.h"


_NAMESPACE_BEGIN

CREffect::CREffect()
{
	m_EffectFileName = _W("");
}

CREffect::CREffect(const CREffect& effect)
{	
	*this = effect;
}

CREffect::~CREffect()
{
}

void 
CREffect::SetTemplate(CREffectTemplate* pTemplate)
{
	m_pTemplate = pTemplate;
	m_ConstantStore.clear();
	m_ConstantParams.clear();

	uint numConstantParams = m_pTemplate->GetNumOfConstantParams();	

	_LOOPi(numConstantParams)
	{		
		m_ConstantStore.push_back(REffectParamStore());
		REffectParam param;
		param.type = EPT_END;
		m_ConstantParams.push_back(param);
	}
}

IREffectTemplate* 
CREffect::GetTemplate() 
{ 
	return m_pTemplate;
}

uint 
CREffect::GetNumOfTechs() const
{
	_DEBUG_ASSERT(m_pTemplate);
	return m_pTemplate->GetNumOfTechs();
}

void 
CREffect::SetParam(const REffectParam& param, uint index)
{
	_DEBUG_ASSERT(index >= 0);
	_DEBUG_ASSERT(index < (uint)(m_ConstantParams.size()));
	_DEBUG_ASSERT(index < (uint)(m_ConstantStore.size()));

	REffectParamStore& store = m_ConstantStore[index];
	store = param;
	store.AssignREffectParam( m_ConstantParams[index] );
}

uint 
CREffect::GetNumOfParams() const
{
	return (uint)(m_ConstantParams.size());
}

const REffectParam* 
CREffect::GetParam(uint index) const 
{
	return &(m_ConstantParams[index]);		
}

const REffectParamStore* 
CREffect::GetParamStore(uint index) const
{
	return &(m_ConstantStore[index]);
}
	
boolean 
CREffect::operator ==(const CREffect& toCompare) const 
{ 
	// Check if they use the same effect template
	if(m_pTemplate != toCompare.m_pTemplate)
		return FALSE;

	// Check that each constant param store is the same
	uint size = (uint)(m_ConstantStore.size());
	if(size != (uint)(toCompare.m_ConstantStore.size()))
		return FALSE;

	_LOOPi(size)
	{
		if(m_ConstantStore[i] != toCompare.m_ConstantStore[i])
			return FALSE;
	}			

	return TRUE;
}

CREffect& 
CREffect::operator =(const CREffect& toAssign)
{
	SetTemplate(toAssign.m_pTemplate);
	SetTemplateFileName(toAssign.m_EffectFileName.c_str());
	
	_LOOPi(toAssign.GetNumOfParams())
	{
		const REffectParam* pParam = toAssign.GetParam(i);
		SetParam(*pParam, i);
	}	
	
	return *this;
}

_NAMESPACE_END