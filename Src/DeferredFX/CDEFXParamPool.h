//==============================================================================
//
//		CDEFXParamPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CDEFXParamPool : public CRefCounted<IDEFXParamPool>
{
	struct FarPlaneOffsets
	{
		gmtl::Vec4f offset[3];
	};

	struct ViewFarPlaneOffsets
	{
		gmtl::Vec4f topLeft;
		gmtl::Vec2f xy;
	};

private:
	CDEFXDeferredFX* m_pDeferredFX;

	IRTexture2DPtr m_pZPos;
	IRTexture2DPtr m_pNormals;
	IRTexture2DPtr m_pAlbedo;
	IDEFXLightGroup* m_pCurLightGroup;
	DEFXPointLight* m_pPointLight;
	DEFXDirLight* m_pDirLight;

	//IDEFXBRDFTableAtlas* m_pBRDFAtlas;

    //IRTextureRT* m_pDownsample2XZPos;
    //IRTextureRT* m_pDownsample2XNormals;

	gmtl::VecA3f*	m_pFarPlaneCorners;
	ObjectPoolGrowable<FarPlaneOffsets, 4> m_FarPlaneOffsetStack;
	boolean m_IsFarPlaneOffsetDirty;

	//gmtl::VecA3f	m_FarPlaneOffsets[3];	// TODO: This needs the array too

	ObjectPoolGrowable<gmtl::Vec2f, 4>	m_ScreenUVOffsetPool;
	boolean m_IsScreenUVOffsetDirty;

	ObjectPoolGrowable<ViewFarPlaneOffsets, 4> m_ViewFarPlaneOffsetStack;

public:
	CDEFXParamPool()
		: m_pDeferredFX(NULL)
		, m_IsScreenUVOffsetDirty(TRUE)
		, m_IsFarPlaneOffsetDirty(TRUE)
	{
		m_ScreenUVOffsetPool.Initialize();
		m_FarPlaneOffsetStack.Initialize();
		m_ViewFarPlaneOffsetStack.Initialize();
	}

	virtual ~CDEFXParamPool()
	{
	}

private:
	gmtl::Vec2f* GetScreenUVOffset();
	FarPlaneOffsets* GetFarPlaneOffset();

public:
	void SetDeferredFX(IDEFXDeferredFX* pDeferredFX) { m_pDeferredFX = (CDEFXDeferredFX*) pDeferredFX; }

	void SetAccumulationBuffers(IRTexture2D* pZPos, IRTexture2D* pNormals, IRTexture2D* pAlbedo);

	//void SetZPosAccum(IRTextureRT* pZPos) { m_pZPos = pZPos; }
	//void SetNormalsAccum(IRTextureRT* pNormals) { m_pNormals = pNormals; }
	//void SetAlbedoAccum(IRTextureRT* pAlbedo) { m_pAlbedo = pAlbedo; }
	void SetCurrentLightGroup(IDEFXLightGroup* pLightGroup)	{ m_pCurLightGroup = pLightGroup; }
	void SetPointLight(DEFXPointLight* pPointLight)			{ m_pPointLight = pPointLight; }
	void SetDirLight(DEFXDirLight* pDirLight)				{ m_pDirLight = pDirLight; }

	//void SetBRDFAtlas(IDEFXBRDFTableAtlas* pAtlas)		{ m_pBRDFAtlas = pAtlas; }
    //void SetDownsample2XZPos(IRTextureRT* pDownsample2XZPos) { m_pDownsample2XZPos = pDownsample2XZPos; }
    //void SetDownsample2XNormals(IRTextureRT* pDownsample2XNormals) { m_pDownsample2XNormals = pDownsample2XNormals; }
	IDEFXLightGroup* GetCurrentLight() { return m_pCurLightGroup; }

	void SetCameraFarPlaneCornerVecs(gmtl::VecA3f* pCorners);
	void SetViewFarPlaneOffsets(gmtl::Vec3f* pTopLeftCorner, float xRange, float yRange);

	void GetParam(uint semantic, REffectParam* pToStore);
	boolean IsParamAvailable(uint semantic);
	boolean IsMorePassesRequired();

	void FinishedResolvingEffect();
	void ResetParams();

	// Prevent cache line false sharing
	_IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

_NAMESPACE_END
