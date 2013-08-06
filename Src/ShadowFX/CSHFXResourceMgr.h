//==============================================================================
//
//		CSHFXResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/15/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSHFXResourceMgr : public ISHFXResourceMgr
{
private: 
	CSHFXShadowFX* m_pShadowFX;
	
public: 
	CSHFXResourceMgr()
		: m_pShadowFX(NULL)
	{
	}

	virtual ~CSHFXResourceMgr()
	{
	}

public:
	void SetShadowFX(CSHFXShadowFX* pShadowFX) { m_pShadowFX = pShadowFX; }

	ISHFXCascadedDirLight*	CreateCascadedDirLight();
	ISHFXPointLight*		CreatePointLight();
	ISHFXCubePointLight*	CreateCubePointLight();

	//IRTextureRT* CreateCascadedDirLightExpShadowMap(uint size);
	//IRTextureRT* CreatePointLightExpShadowMap(uint size);
	//IRDepthBuffer* CreateShadowMapDepthBuffer(uint size);

	eRTexFormat GetSMCompatibleFormat();
	eRDepthFormat GetSMCompatibleDepthFormat();

	ISHFXPipeline* CreatePipeline();
	ISHFXParamPool* CreateParamPool();
};

_NAMESPACE_END
