//==============================================================================
//
//		AHSpatialComponent.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/21/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

class AHSpatialComponent : public AHComponent
{
private:
	gmtl::MatrixA44f	m_WorldMatrix;
	AABox				m_LocalAABox;
	AABox				m_WorldAABox;
	
	std::vector<AHComponent*> m_pListeners;

public: 
	AHSpatialComponent()
		: AHComponent(COMP_SPATIAL)
	{
		gmtl::identity(_CAST_MAT44(m_WorldMatrix));
		m_LocalAABox.min.set(0.0f, 0.0f, 0.0f);
		m_LocalAABox.max.set(0.0f, 0.0f, 0.0f);
		m_WorldAABox.min.set(0.0f, 0.0f, 0.0f);
		m_WorldAABox.max.set(0.0f, 0.0f, 0.0f);
	}

	virtual ~AHSpatialComponent()
	{
	}

private:
	void UpdateWorldBoundsTranslate();

public: 
	void SetWorldMatrix(const gmtl::MatrixA44f& mat);
	void SetWorldPos(const gmtl::VecA3f& vec);
	void SetWorldFacing(const gmtl::VecA3f& dir, const gmtl::VecA3f& up);
	void SetWorldPosAndFacing(const gmtl::VecA3f& pos, const gmtl::VecA3f& dir, const gmtl::VecA3f& up);

	void GetWorldPos(gmtl::VecA3f& pos);
	const gmtl::MatrixA44f& GetWorldMatrix()		{ return m_WorldMatrix; }		

	void SetLocalAABox(const AABox& box) { m_LocalAABox = box; }
	const AABox& GetLocalAABox() { return m_LocalAABox; }
	const AABox& GetWorldAABox() { return m_WorldAABox; }
	
	void AddEventListener(AHComponent* pComp);
	void RemoveEventListener(AHComponent* pComp);

	void OnComponentEvent(int eventType, AHComponent* pSrc); 

	_IMPL_ALIGNED_ALLOCS(16)
};

#endif

_NAMESPACE_END
