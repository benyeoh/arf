//==============================================================================
//
//		AHRenderComponent.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/18/2009
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

class AHRenderComponent : public AHComponent
{
private:	
	AABox	m_LocalAABox;
	AABox	m_WorldAABoxGather;
	OOBox	m_WorldOOBoxGather;

	AABox	m_WorldAABox;
	OOBox	m_WorldOOBox;
	
	wstring			m_RenderMeshFileName;
	BFXUMeshListPtr	m_pRenderMeshList;
		
	DenseChainedHashMap<uint, boolean, NullUintHash> m_ParamSemanticMap; 
	boolean m_MatTypeMap[BFX_MAX_MATERIAL_TYPES]; 
	
	boolean m_IsAddedToUpdate;

	ISDBSceneContainer* m_pSceneContainer;
    SDBDataHandle   m_SceneHandle;

public:
	AHRenderComponent()
		: AHComponent(COMP_RENDER)
		, m_IsAddedToUpdate(FALSE)
		, m_pSceneContainer(NULL)
	{
		m_LocalAABox.min.set(0.0f, 0.0f, 0.0f);
		m_LocalAABox.max.set(0.0f, 0.0f, 0.0f);
		//m_RenderParams.Initialize(16, 0.667f);
	}	
	
	virtual ~AHRenderComponent()
	{
		SetScene(NULL);
	}

private:
	void AddToTempUpdate();
	void UpdateWorldBoundsFromSpatial(AHSpatialComponent* pSrc);
	void UpdateWorldBounds(const gmtl::MatrixA44f& world);	

public:
	void SetScene(ISDBSceneContainer* pContainer);

	void SetMeshList(BFXUMeshList* pMeshList);
	BFXUMeshList* GetMeshList()						{ return m_pRenderMeshList; }
	
	void SetRenderMeshFileName(const wchar* pName)	{ m_RenderMeshFileName = pName; }
	const wchar* GetRenderMeshFileName()			{ return m_RenderMeshFileName.size() > 0 ? m_RenderMeshFileName.c_str() : NULL; }
	
	void SetLocalAABox(const AABox& box);
	const AABox& GetLocalAABox() { return m_LocalAABox; }
	const OOBox& GetWorldOOBox() { return m_WorldOOBox; }
	const AABox& GetWorldAABox() { return m_WorldAABox; }
	
	void Render(uint matType, uint libIndex, IBFXBaseFX* pBaseFX, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IBFXMaterialCreateCallback* pCreateCallback);
		
	boolean IsParamSemanticUsed(uint semantic);
	boolean IsMatTypeUsed(uint semantic);

	void OnComponentEvent(int eventType, AHComponent* pSrc);
	void Update(uint phaseNum);

	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(AHRenderComponent)

#endif

_NAMESPACE_END