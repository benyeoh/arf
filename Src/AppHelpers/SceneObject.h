//==============================================================================
//
//		SceneObject.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/10/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class SceneObject : public CRefCounted<IRefCounted>
{
private:
	// Aligned members 1st
	AABox				m_LocalAABox;
	AABox				m_WorldAABox;
	OOBox				m_WorldOOBox;
	gmtl::MatrixA44f	m_WorldMatrix;

	std::vector<RFXMesh> m_RenderMeshes;	
	float m_ZDistance;
	
public:
	SceneObject()
		: CRefCounted()
		, m_ZDistance(1.0f)
	{
	}	
	
	virtual ~SceneObject()
	{
	}
	
public:
	void AddRenderMesh(RFXMesh& mesh)
	{
		m_RenderMeshes.push_back(mesh);
	}

	uint GetNumOfRenderMeshes()
	{
		return (uint) m_RenderMeshes.size();
	}
	
	RFXMesh& GetRenderMesh(uint index)
	{
		return (m_RenderMeshes[index]);
	}
	
	const gmtl::MatrixA44f* GetWorldMatrix()
	{
		return &m_WorldMatrix;
	}
			
	void SetWorldMatrix(const gmtl::Matrix44f* pMat)
	{
		m_WorldMatrix = *pMat;
	}
	
	void SetZDistance(float val)
	{
		m_ZDistance = val;
	}
	
	float GetZDistance()
	{
		return m_ZDistance;
	}
	
	void SetLocalAABox(const AABox& box) { m_LocalAABox = box; }
	const AABox& GetLocalAABox() { return m_LocalAABox; }
	const OOBox& GetWorldOOBox() { return m_WorldOOBox; }
	const AABox& GetWorldAABox() { return m_WorldAABox; }
	
	void Render(IRFXRenderFX* pRenderFX);
	void UpdateWorldBounds();	
};

_DECLARE_SMARTPTR(SceneObject)

_NAMESPACE_END