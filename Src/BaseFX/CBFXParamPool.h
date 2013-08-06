//==============================================================================
//
//		CBFXParamPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXParamPool : public CRefCounted<IBFXParamPool>
{
private: 
	const gmtl::Matrix44f* m_pWorld;
	float m_ZDistance;

	//gmtl::Vec4f m_CameraPosWithViewFar;
	//gmtl::Vec4f m_CameraPosWithViewNear;
	
	ObjectPoolGrowable<gmtl::Matrix44f, 4>	m_WorldViewStack;
	ObjectPoolGrowable<gmtl::Matrix44f, 4>	m_WorldViewProjStack;

	boolean m_DirtyWorldViewProjMat;
	boolean m_DirtyWorldViewMat;

	CBFXBaseFX* m_pBase;	

	const BFXDirLight* m_pDirLight;
	const BFXSpotLight* m_pSpotLights[BFX_MAX_SPOT_LIGHTS];
	const BFXPointLight*  m_pPointLights[BFX_MAX_POINT_LIGHTS];
	const BFXHemiLight* m_pHemiLight;

	boolean m_IsHemiLightUsed;
	boolean m_IsDirLightUsed;

	int m_TotalNumPointLights;
	int m_TotalNumSpotLights;

	int m_CurPointLightStart;
	int m_CurSpotLightStart;

	uint m_CurContext;
	
	CBFXSharedParamDataPtr m_pSharedParam;

public: 
	CBFXParamPool(CBFXSharedParamData* pSharedParam)
		: m_pBase(NULL)
		, m_DirtyWorldViewProjMat(TRUE)
		, m_DirtyWorldViewMat(TRUE)
		, m_pDirLight(NULL)
		, m_pHemiLight(NULL)
		, m_TotalNumPointLights(0)
		, m_TotalNumSpotLights(0)
		, m_CurPointLightStart(0)
		, m_CurSpotLightStart(0)
		, m_IsHemiLightUsed(FALSE)
		, m_IsDirLightUsed(FALSE)
		, m_CurContext(0)
	{
		m_pSharedParam = pSharedParam;

		//m_CameraPosWithViewFar[3] = 500.0f;
		m_WorldViewProjStack.Initialize();
		m_WorldViewStack.Initialize();
	}

	virtual ~CBFXParamPool()
	{
	}

public: 
	void SetBaseFX(CBFXBaseFX* pBase) { m_pBase = pBase; }

	void SetCurrentContext(uint id);
	uint GetCurrentContext();

	void SetViewMatrix(const gmtl::Matrix44f& view);
	const gmtl::Matrix44f& GetViewMatrix();

	void SetProjMatrix(const gmtl::Matrix44f& proj);
	const gmtl::Matrix44f& GetProjMatrix();

	void SetWorldMatrix(const gmtl::Matrix44f& world);
	const gmtl::Matrix44f& GetWorldMatrix();

	const gmtl::Matrix44f& GetWorldViewProjMatrix();
	const gmtl::Matrix44f& GetViewProjMatrix();
	const gmtl::Matrix44f& GetWorldViewMatrix();

	void SetZDistanceFromCamera(float dist);
	float GetZDistanceFromCamera();
	
	void SetCameraProperties(const gmtl::Vec3f& cameraPos, float nearPlane, float farPlane);
	//void SetNearFarPlanes(float near, float far);

	const gmtl::Vec4f& GetCameraPosWithViewFar();
	const gmtl::Vec4f& GetCameraPosWithViewNear();

	void SetHemiLight(const BFXHemiLight* pHemiLight);
	const BFXHemiLight* UseHemiLight();

	void SetDirLight(const BFXDirLight* pDirLight);
	const BFXDirLight* UseDirLight();

	void SetSpotLights(const BFXSpotLight** ppSpotLight, uint numSpotLights);
	const BFXSpotLight* UseNextSpotLight();

	void SetPointLights(const BFXPointLight** ppPointLight, uint numPointLights);
	const BFXPointLight* UseNextPointLight();

	void ResetLightsCount();
	boolean CheckHemiLightAvailable();
	boolean CheckDirLightAvailable();
	boolean CheckPointLightsAvailable(uint numPointLights);
	boolean CheckSpotLightsAvailable(uint numSpotLights);
	
	void GetParam(uint semantic, REffectParam* pToStore);
	boolean IsParamAvailable(uint semantic);
	boolean IsMorePassesRequired();
	void FinishedResolvingEffect();
	void ResetParams();

	void FinalizeSharedParams();

	// Prevent cache line false sharing
	_IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

_NAMESPACE_END
