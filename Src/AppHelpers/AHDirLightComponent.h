//================================================================================
//
//		AHDirLightComponent.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/6/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

class AHDirLightComponent : public AHComponent
{
	const static uint MAX_NUM_CASCADE_BOUNDS = 8;


	struct MurmurHashSizeTObj
	{
		inline uint operator() (size_t val) const { return MurmurHashSizeT(val); }
	};

private:
	AABox				m_LocalAABox;
	AABox				m_WorldAABoxGather;
	OOBox				m_WorldOOBoxGather;

	AABox				m_WorldAABox;
	OOBox				m_WorldOOBox;
	
	BFXDirLight					m_BFXDirLight;
	ISHFXCascadedDirLightPtr	m_pShadowDL;
	boolean						m_IsShadowMapShared;
	float						m_CascadeSplitExp;
	float						m_PosOffset;

	DenseChainedHashMap<size_t, SHFXCascadedFrustumBounds*, MurmurHashSizeTObj> m_CascadeBoundsMap;
//	SHFXCascadedFrustumBounds*	m_pCascadeBounds[MAX_NUM_CASCADE_BOUNDS];

	DEFXDirLight			m_DEFXDirLight;
	IDEFXLightGroupPtr		m_pLightGroup;
	AHRenderComponentPtr	m_pRenderComp;
	gmtl::MatrixA44f		m_RenderWorldMat;

	ISDBSceneContainer* m_pSceneContainer;
    SDBDataHandle   m_SceneHandle;
	boolean m_IsAddedToUpdate;

public: 
	AHDirLightComponent();
	virtual ~AHDirLightComponent();

private:
	void AddToTempUpdate();
	void UpdateWorldBoundsFromSpatial(AHSpatialComponent* pSpatial);
	void UpdateWorldBounds(const gmtl::MatrixA44f& world);
	void ClearCascadeBounds();

public:
	void	SetScene(ISDBSceneContainer* pContainer);

	void	SetColor(const gmtl::Vec3f& color);
	void	SetLocalAABox(const AABox& bounds);
	void	SetShadowDirLight(ISHFXCascadedDirLight* pShadowDL, float cascadeExp, float posOffset, boolean isShadowMapShared);

	float					GetShadowCascadeSplitExp()	{ return m_CascadeSplitExp; }
	float					GetShadowPosOffset()	{ return m_PosOffset; } 
	ISHFXCascadedDirLight*	GetShadowDirLight()		{ return m_pShadowDL; }
	boolean					IsShadowMapShared()		{ return m_IsShadowMapShared; }

	BFXDirLight&			GetBFXDirLight()		{ return m_BFXDirLight; }
	const AABox&			GetWorldAABox()			{ return m_WorldAABox; }
	const OOBox&			GetWorldOOBox()			{ return m_WorldOOBox; }

	//void	AddToRenderComp(AHRenderComponent* pRenderComp);
	void					SetDeferredDirLight(IDEFXLightGroup* pLightGroup, AHRenderComponent* pRenderComp);
	IDEFXLightGroup*		GetLightGroup()			{ return m_pLightGroup; }
	AHRenderComponent*		GetRenderComponent()	{ return m_pRenderComp; }
	DEFXDirLight&			GetDEFXDirLight()		{ return m_DEFXDirLight; }
	const gmtl::MatrixA44f&		GetRenderWorldMat()		{ return m_RenderWorldMat; }
	
	void UpdateCascadeBounds(const gmtl::MatrixA44f& proj, size_t boundsID);
	void UpdateCascadeShadow(const gmtl::VecA3f& camPos, const gmtl::MatrixA44f& view, gmtl::MatrixA44f& proj, size_t boundsID);
	void UpdateCascadeShadow(const gmtl::VecA3f& camPos, const gmtl::MatrixA44f&view, SHFXCascadedFrustumBounds& bounds);

	void OnComponentEvent(int eventType, AHComponent* pSrc);
	void Update(uint phaseNum);

	_IMPL_ALIGNED_ALLOCS(16)
};

#endif

_NAMESPACE_END
