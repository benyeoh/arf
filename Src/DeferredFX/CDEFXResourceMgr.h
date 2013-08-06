//==============================================================================
//
//		CDEFXResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CDEFXResourceMgr : public IDEFXResourceMgr
{
private:
	CDEFXDeferredFX* m_pDeferredFX;

public:
	CDEFXResourceMgr()
		: m_pDeferredFX(NULL)
	{
	}

	virtual ~CDEFXResourceMgr()
	{
	}

public:
	void SetDeferredFX(CDEFXDeferredFX* pDeferredFX) { m_pDeferredFX = pDeferredFX; }

	IDEFXPipeline* CreatePipeline();
	IDEFXParamPool* CreateParamPool();

	IDEFXLightGroup* CreateLightGroup();
	IDEFXBRDFTableAtlas* CreateBRDFAtlas(IRTexture2D* pBRDFAtlas, IRTexture2D* pBRDFParams);
	IDEFXBRDFTable* CreateBRDFTable(const DEFXLaFortuneBRDFParam& param, IRTexture2D* pData);
};

_NAMESPACE_END
