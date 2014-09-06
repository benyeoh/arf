//==============================================================================
//
//		CDFXParams.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/27/2008
//
//==============================================================================

#pragma once
_NAMESPACE_BEGIN

struct BatchedQuadParams
{
	std::vector<gmtl::Vec3f> quadPos;
	std::vector<gmtl::Vec2f> quadSize;
	std::vector<gmtl::Vec2f> quadTexPos;
	std::vector<gmtl::Vec2f> quadTexSize;
};

class CDFXParams : public IDFXParams
{
	typedef dense_hash_map<size_t, BatchedQuadParams*> BatchedQuadMap;

private: 
	const gmtl::Matrix44f* m_pView;
	const gmtl::Matrix44f* m_pProj;
	const gmtl::Matrix44f* m_pWorldBatch;
	const gmtl::Vec4f* m_pColorBatch;
	const gmtl::Vec3f* m_pPosBatch;
	const gmtl::Vec2f* m_pQuadSizeBatch;
	const gmtl::Vec2f* m_pQuadTexPosBatch;
	const gmtl::Vec2f* m_pQuadTexSizeBatch;
	REffectParam* m_pQuadTex;
		
	gmtl::Matrix44f m_ViewProj;
	boolean m_IsViewProjDirty;
	
	std::vector<gmtl::Matrix44f> m_BatchedWorldWireBox;
	std::vector<gmtl::Vec4f> m_BatchedColorsWireBox;
	std::vector<gmtl::Matrix44f> m_BatchedWorldWireSphere;
	std::vector<gmtl::Vec4f> m_BatchedColorsWireSphere;
	std::vector<gmtl::Vec3f> m_BatchedPosPoint;
	std::vector<gmtl::Vec4f> m_BatchedColorsPoint;
	
	ObjectPool<BatchedQuadParams, 256> m_BatchedQuadPool;
	BatchedQuadMap m_BatchedQuads;  
	std::vector<REffectParam> m_BatchedQuadTextures;
	
	uint m_NumOfBatch;
	
	CDFXDebugFX* m_pDebugFX;
		
public: 
	CDFXParams()
		: m_pDebugFX(NULL)
		, m_pProj(NULL)
		, m_pView(NULL)
		, m_pWorldBatch(NULL)
		, m_pColorBatch(NULL)
		, m_NumOfBatch(0)
		, m_IsViewProjDirty(FALSE)
		, m_pQuadSizeBatch(NULL)
		, m_pQuadTexPosBatch(NULL)
		, m_pQuadTexSizeBatch(NULL)
		, m_pQuadTex(NULL)
	{
		m_BatchedQuads.set_empty_key(0xFFFFFFFE);
		m_BatchedQuads.set_deleted_key(0xFFFFFFFF);
	}

	~CDFXParams()
	{
	}

public: 
	void SetDebugFX(CDFXDebugFX* pDebugFX) { m_pDebugFX = pDebugFX; }

	void ResetParams();

	uint InitializeEffect(IREffect& effect, float detailLevel, REffectParam* params);

	const gmtl::Matrix44f& GetViewProjMatrix();
	
	void SetViewMatrix(const gmtl::Matrix44f& view);
	const gmtl::Matrix44f& GetViewMatrix();

	void SetProjMatrix(const gmtl::Matrix44f& proj);
	const gmtl::Matrix44f& GetProjMatrix();

	void SetWorldBatchMatrix(const gmtl::Matrix44f* pWorld);
	const gmtl::Matrix44f* GetWorldBatchMatrix();

	void SetColorBatch(const gmtl::Vec4f* pColor);
	const gmtl::Vec4f* GetColorBatch();
	
	void SetPosBatch(const gmtl::Vec3f* pPos);
	const gmtl::Vec3f* GetPosBatch();
	
	void SetQuadSizeBatch(const gmtl::Vec2f* pSize);
	const gmtl::Vec2f* GetQuadSizeBatch();
	
	void SetQuadTexPosBatch(const gmtl::Vec2f* pPos);
	const gmtl::Vec2f* GetQuadTexPosBatch();
	
	void SetQuadTexSizeBatch(const gmtl::Vec2f* pSize);
	const gmtl::Vec2f* GetQuadTexSizeBatch();
	
	void SetQuadTexture(REffectParam* pTex);
	REffectParam* GetQuadTexture();
	
	void SetNumOfBatch(uint numOfBatch);
	uint GetNumOfBatch();
	
	void AddBatchedWireBoxParams(const gmtl::Matrix44f& world, const gmtl::Vec4f& color);
	const gmtl::Matrix44f* GetBatchedWorldMatricesWireBox();
	const gmtl::Vec4f* GetBatchedColorsWireBox();
	uint GetNumOfBatchedWireBox();
	
	void AddBatchedWireSphereParams(const gmtl::Matrix44f& world, const gmtl::Vec4f& color);
	const gmtl::Matrix44f* GetBatchedWorldMatricesWireSphere();
	const gmtl::Vec4f* GetBatchedColorsWireSphere();
	uint GetNumOfBatchedWireSphere();

	void AddBatchedPointParams(const gmtl::Vec3f& pos, const gmtl::Vec4f& color);
	const gmtl::Vec3f* GetBatchedPosPoint();
	const gmtl::Vec4f* GetBatchedColorsPoint();
	uint GetNumOfBatchedPoint();

	void AddBatchedQuadParams(const gmtl::Vec3f& pos, const gmtl::Vec2f& size,
							const gmtl::Vec2f& texPos, const gmtl::Vec2f& texSize,
							IRTexture2D* pTex);						
	//void AddBatchedQuadParams(const gmtl::Vec3f& pos, const gmtl::Vec2f& size,
	//						const gmtl::Vec2f& texPos, const gmtl::Vec2f& texSize,
	//						IRTexture2D* pTex);
	void AddBatchedQuadVertexParams(size_t index, const gmtl::Vec3f& pos, const gmtl::Vec2f& size,
								const gmtl::Vec2f& texPos, const gmtl::Vec2f& texSize);
								
	BatchedQuadParams* GetBatchedQuadParams(size_t index);
	REffectParam* GetQuadTexture(uint index);
	uint GetNumOfBatchedQuadParams();
			
	void GetParameter(REffectParam& param, uint semantic);
};

_NAMESPACE_END
