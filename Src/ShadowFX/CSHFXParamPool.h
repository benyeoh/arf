//==============================================================================
//
//		CSHFXParamPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/9/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSHFXParamPool : public CRefCounted<ISHFXParamPool>
{
private: 
	CSHFXShadowFX*				m_pShadowFX;
	CSHFXCascadedDirLight*		m_pCascadedDirLight;
	boolean						m_IsCascadedDirLightUsed;

	const gmtl::Vec4f*			m_pOffset;
	const IRTextureRT*			m_pDebugSM;
	float						m_ZDistance;

	CSHFXPointLight*			m_pPointLights[BFX_MAX_POINT_LIGHTS];
	uint						m_PLCasterFlag[BFX_MAX_POINT_LIGHTS];
	uint						m_NumPointLights;
	uint						m_CurCasterPLIndex;
	uint						m_CurRecPLIndex;

	uint m_CascadedCasterFlag;
	uint m_CurrentCasterFlag;
	uint m_CurrentCasterIndex;

	CSHFXCubePointLight*		m_pCasterCubePL;
	eRCubeFace					m_CasterCubeFace;

	CSHFXCubePointLight*		m_pCubePointLights[BFX_MAX_POINT_LIGHTS];
	uint						m_NumCubePointLights;
	uint						m_CurRecCubePLIndex;

public: 
	CSHFXParamPool()
		: m_pShadowFX(NULL)
		, m_pCascadedDirLight(NULL)
		//, m_CascadedCasterIndex(0)
		, m_CascadedCasterFlag(0)
		, m_CurrentCasterFlag(m_CascadedCasterFlag)
		, m_IsCascadedDirLightUsed(FALSE)
		, m_pDebugSM(NULL)
		, m_pOffset(NULL)
		, m_CurrentCasterIndex(0)
		, m_CurCasterPLIndex(0)
		, m_CurRecPLIndex(0)
		, m_NumPointLights(0)
		, m_CurRecCubePLIndex(0)
		, m_NumCubePointLights(0)
		, m_CasterCubeFace(CF_NEG_Z)
		, m_pCasterCubePL(NULL)
	{
	}

	virtual ~CSHFXParamPool()
	{
	}

public: 
	void	SetShadowFX(CSHFXShadowFX* pShadowFX) { m_pShadowFX = pShadowFX; }

	void	SetZDistanceFromCamera(float zDist)		{ m_ZDistance = zDist; }
	float	GetZDistanceFromCamera()				{ return m_ZDistance; }
	
	void	SetCascadedCasterFlag(uint cascadedFlag);
	uint	GetCurrentCascadedCasterIndex();
	
	void	SetCascadedDirLight(ISHFXCascadedDirLight* pDirLight);
	ISHFXCascadedDirLight* GetCascadedDirLight();

	void	SetPointLights(ISHFXPointLight** ppPointLight, uint* pCasterFlag, uint numPointLights);
	SHFXShadowPLParams*	UseNextCasterPointLightParam();
	CSHFXPointLight* UseNextRecPointLight();
	CSHFXPointLight* GetCurrentPLCaster();
	uint	GetCurrentPLCasterFace();
	
	void	SetCasterCubePointLight(ISHFXCubePointLight* pCaster, eRCubeFace face);
	CSHFXCubePointLight* GetCubePLCaster();
	eRCubeFace GetCubePLCasterFace();

	void	SetCubePointLights(ISHFXCubePointLight** ppPointLight, uint numPointLights);
	CSHFXCubePointLight* UseNextRecCubePointLight();

	const SHFXCascadedProj* UseCascadedProjWithTexOffset();
	boolean CheckCascadedDirLightAvail();
	
	void GetParam(uint semantic, REffectParam* pToStore);
	boolean IsParamAvailable(uint semantic);
	boolean IsMorePassesRequired();
	
	void FinishedResolvingEffect();
	void ResetParams();

	// Prevent cache line false sharing
	_IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)

};

_NAMESPACE_END
