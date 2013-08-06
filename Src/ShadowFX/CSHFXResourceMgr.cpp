//==============================================================================
//
//		CSHFXResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/15/2008
//
//==============================================================================

#include "ShadowFXFramework.h"

_NAMESPACE_BEGIN

ISHFXCascadedDirLight* 
CSHFXResourceMgr::CreateCascadedDirLight()
{
	CSHFXCascadedDirLight* pToReturn = 	_NEW CSHFXCascadedDirLight(m_pShadowFX);//, 1024, 4);
	
	_DEBUG_ASSERT(((uint) (size_t) pToReturn->GetProj().cascadedViewProj) % 16 == 0);
		
	return pToReturn;
}

ISHFXPointLight*		
CSHFXResourceMgr::CreatePointLight()
{
	CSHFXPointLight* pToReturn = _NEW CSHFXPointLight(m_pShadowFX);//, 256, 5);
	return pToReturn;
}

ISHFXCubePointLight* CSHFXResourceMgr::CreateCubePointLight()
{
	CSHFXCubePointLight* pToReturn = _NEW CSHFXCubePointLight(m_pShadowFX);
	return pToReturn;
}

eRTexFormat CSHFXResourceMgr::GetSMCompatibleFormat()
{
	// NOTE: G16R16 is slow on some GPUs (like ATI 5800)
	return TEXF_G16R16;
}

eRDepthFormat CSHFXResourceMgr::GetSMCompatibleDepthFormat()
{
	return DF_D24S8;
}

//IRTextureRT* CSHFXResourceMgr::CreateCascadedDirLightExpShadowMap(uint size)
//{
//	return m_pShadowFX->GetRenderer().GetRResourceMgr().CreateTextureRT(size, size, TEXF_R32F);
//}
//
//IRTextureRT* CSHFXResourceMgr::CreatePointLightExpShadowMap(uint size)
//{
//	return m_pShadowFX->GetRenderer().GetRResourceMgr().CreateTextureRT(size, size, TEXF_G32R32F);
//}
//
//IRDepthBuffer* CSHFXResourceMgr::CreateShadowMapDepthBuffer(uint size)
//{
//	return m_pShadowFX->GetRenderer().GetRResourceMgr().CreateDepthBuffer(size, size, DF_D24S8, 0);
//}

ISHFXPipeline* CSHFXResourceMgr::CreatePipeline()
{
	CSHFXPipeline* pPipeline = _NEW CSHFXPipeline();
	pPipeline->SetShadowFX(m_pShadowFX);
	pPipeline->Initialize();

	return pPipeline;
}

ISHFXParamPool* CSHFXResourceMgr::CreateParamPool()
{
	CSHFXParamPool* pPool = _NEW CSHFXParamPool();
	pPool->SetShadowFX(m_pShadowFX);
	
	return pPool;
}

_NAMESPACE_END