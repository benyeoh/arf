//==============================================================================
//
//		CBFXMaterialGroupTemplate.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/22/2008
//
//==============================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN

CBFXMaterialGroupTemplate::CBFXMaterialGroupTemplate(CBFXBaseFX* pBase, const CRCDataKey* pResID, const wchar* pFileName)
	: m_pBase(pBase)
{
	if(pResID)
		m_ID = *pResID;
		
	if(pFileName)
		m_FileName = pFileName;
		
	memset(m_MatType, 0, sizeof(CBFXMaterial*) * BFX_MAX_MATERIAL_TYPES);	
}

CBFXMaterialGroupTemplate::~CBFXMaterialGroupTemplate()
{
	((CBFXResourceMgr*)&m_pBase->GetResourceMgr())->NotifyMaterialGroupTemplateDestroyed(m_ID, this);
}

void
CBFXMaterialGroupTemplate::AddMaterial(uint matType, CBFXMaterial* pMat)
{
	_DEBUG_ASSERT(matType >= 0 && matType < BFX_MAX_MATERIAL_TYPES);
	
	m_Mats.push_back(pMat);
	m_MatIndices.push_back(matType);
	
	m_MatType[matType] = pMat;
}

_NAMESPACE_END