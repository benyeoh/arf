//==============================================================================
//
//		AHPointLightComponent.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/27/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

class AHPointLightComponent : public AHComponent
{
private:
	AABox	m_WorldAABoxGather;
	OOBox	m_WorldOOBoxGather;
	Sphere	m_WorldSphereBV;

	AABox	m_WorldAABoxBV;
	OOBox	m_WorldOOBoxBV;

	BFXPointLight			m_BFXPointLight;
	ISHFXCubePointLightPtr	m_pShadowPL;
	boolean					m_IsShadowMapShared;

	DEFXPointLight			m_DEFXPointLight;
	IDEFXLightGroupPtr		m_pLightGroup;
	AHRenderComponentPtr	m_pRenderComp;
	gmtl::MatrixA44f		m_RenderWorldMat;

	ISDBSceneContainer* m_pSceneContainer;
	SDBDataHandle   m_SceneHandle;
    boolean m_IsAddedToUpdate;

public: 
	AHPointLightComponent();
	virtual ~AHPointLightComponent();

private:
	void AddToTempUpdate();
	void UpdateWorldBoundsFromSpatial(AHSpatialComponent* pSpatial);
	void UpdateWorldBounds(const gmtl::MatrixA44f& world);

public:
	void	SetScene(ISDBSceneContainer* pContainer);

	void	SetColor(const gmtl::Vec3f& color);
	void	SetRadius(float lightRadius);
	void	SetShadowPointLight(ISHFXCubePointLight* pShadowPL, boolean isShadowMapShared);
	ISHFXCubePointLight*	GetShadowPointLight()	{ return m_pShadowPL; }
	boolean					IsShadowMapShared()		{ return m_IsShadowMapShared; }
	BFXPointLight&			GetBFXPointLight()		{ return m_BFXPointLight; }
	const Sphere&			GetWorldSphere()		{ return m_WorldSphereBV; }
	const AABox&			GetWorldAABox()			{ return m_WorldAABoxBV; }
	float					GetRadius()				{ return m_WorldSphereBV.center[3]; }

	//void	AddToRenderComp(AHRenderComponent* pRenderComp);
	void					SetDeferredPointLight(IDEFXLightGroup* pLightGroup, AHRenderComponent* pRenderComp);
	IDEFXLightGroup*		GetLightGroup()			{ return m_pLightGroup; }
	AHRenderComponent*		GetRenderComponent()	{ return m_pRenderComp; }
	DEFXPointLight&			GetDEFXPointLight()		{ return m_DEFXPointLight; }
	const gmtl::MatrixA44f&		GetRenderWorldMat()		{ return m_RenderWorldMat; }

	void OnComponentEvent(int eventType, AHComponent* pSrc);
	void Update(uint phaseNum);

	_IMPL_ALIGNED_ALLOCS(16)
};

#endif
_NAMESPACE_END
