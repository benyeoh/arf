//================================================================================
//
//		CBFXSharedParamData.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/17/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXSharedParamData : public CRefCounted<IBFXSharedParamData>
{
private:
	const gmtl::Matrix44f* m_pView;
	const gmtl::Matrix44f* m_pProj;

	boolean m_DirtyViewProjMat;
	boolean m_DirtyCamPosViewFar;
	boolean m_DirtyCamPosViewNear;

	ObjectPoolGrowable<gmtl::Vec4f, 4>	m_CameraPosWithViewFarStack;
	ObjectPoolGrowable<gmtl::Vec4f, 4>	m_CameraPosWithViewNearStack;
	ObjectPoolGrowable<gmtl::Matrix44f, 4>	m_ViewProjStack;

	gmtl::Vec3f m_CameraPos;
	float m_Near;
	float m_Far;

public:
	CBFXSharedParamData()
		: m_DirtyViewProjMat(TRUE)
		, m_DirtyCamPosViewFar(TRUE)
		, m_DirtyCamPosViewNear(TRUE)
		, m_pView(NULL)
		, m_pProj(NULL)
	{

		m_ViewProjStack.Initialize();
		m_CameraPosWithViewFarStack.Initialize();
		m_CameraPosWithViewNearStack.Initialize();
	}

	~CBFXSharedParamData()
	{
	}

public:
	void SetViewMatrix(const gmtl::Matrix44f& view);
	const gmtl::Matrix44f& GetViewMatrix();

	void SetProjMatrix(const gmtl::Matrix44f& proj);
	const gmtl::Matrix44f& GetProjMatrix();

	const gmtl::Matrix44f& GetViewProjMatrix();
	
	void SetCameraProperties(const gmtl::Vec3f& cameraPos, float nearPlane, float farPlane);

	const gmtl::Vec4f& GetCameraPosWithViewFar();
	const gmtl::Vec4f& GetCameraPosWithViewNear();

	void Finalize();

	void ResetParams();

	// Prevent cache line false sharing
	_IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

_DECLARE_SMARTPTR(CBFXSharedParamData)

_NAMESPACE_END