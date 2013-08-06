//================================================================================
//
//		CDEFXResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/5/2012
//
//================================================================================

#include "DeferredFXFramework.h"

_NAMESPACE_BEGIN

IDEFXPipeline* CDEFXResourceMgr::CreatePipeline()
{
	CDEFXPipeline* pPipeline = _NEW CDEFXPipeline();
	pPipeline->SetDeferredFX(m_pDeferredFX);
	pPipeline->Initialize();

	return pPipeline;
}

IDEFXParamPool* CDEFXResourceMgr::CreateParamPool()
{
	CDEFXParamPool* pPool = _NEW CDEFXParamPool();
	pPool->SetDeferredFX(m_pDeferredFX);
	return pPool;
}

IDEFXLightGroup* CDEFXResourceMgr::CreateLightGroup()
{
	CDEFXLightGroup* pLightGroup = _NEW CDEFXLightGroup();
	pLightGroup->Initialize(m_pDeferredFX);
	return pLightGroup;
}

IDEFXBRDFTableAtlas* CDEFXResourceMgr::CreateBRDFAtlas(IRTexture2D* pBRDFAtlas, IRTexture2D* pBRDFParams)
{
	IDEFXBRDFTableAtlas* pAtlas = _NEW CDEFXBRDFTableAtlas(m_pDeferredFX);
	pAtlas->SetData(pBRDFAtlas, pBRDFParams);
	return pAtlas;
}

IDEFXBRDFTable* CDEFXResourceMgr::CreateBRDFTable(const DEFXLaFortuneBRDFParam& param, IRTexture2D* pData)
{
	IDEFXBRDFTable* pTable = _NEW CDEFXBRDFTable(m_pDeferredFX); 
	pTable->SetParams(param);
	pTable->SetData(pData);
	return pTable;
}

_NAMESPACE_END