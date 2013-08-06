//==============================================================================
//
//		CRResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, March 21, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRResourceMgr : public IRResourceMgr
{
	typedef dense_hash_map< CRCDataKey, CREffectTemplate*, HashCRCDataKey >		EffectTemplateMap;
	typedef dense_hash_map< CRCDataKey, CRTexture2D*, HashCRCDataKey >				Texture2DMap;	
	typedef dense_hash_map< CRCDataKey, CRTexture3D*, HashCRCDataKey >				Texture3DMap;	
	typedef dense_hash_map< CRCDataKey, CRTextureCube*, HashCRCDataKey >			TextureCubeMap;
	typedef dense_hash_map< CRCDataKey, std::vector<CREffect*>, HashCRCDataKey >	EffectMultiMap;
	//typedef std::multimap< MD5Digest, CREffect* >	EffectMultiMap;
	typedef dense_hash_map< CRCDataKey, CRIndexBuffer*, HashCRCDataKey >			IndexBufferMap;
	typedef dense_hash_map< CRCDataKey, CRVertexBuffer*, HashCRCDataKey >			VertexBufferMap;
	typedef dense_hash_map< CRCDataKey, CRVertexBufferGroup*, HashCRCDataKey >		VertexBufferGroupMap;
	
	typedef std::vector< CRIndexBuffer* >		IndexBufferList;
	typedef std::vector< CRVertexBuffer* >		VertexBufferList;
	typedef std::vector< CREffect* >			EffectList;
	typedef std::vector< CREffectTemplate* >	EffectTemplateList;
	typedef std::vector< CRTexture2D* >			Texture2DList;	
	typedef std::vector< CRTextureCube* >		TextureCubeList;	
	typedef std::vector< CRTexture3D* >			Texture3DList;	

	typedef std::vector< CRDepthBuffer* >		DepthBufferList;
	typedef std::vector< CRTextureRT* >			TextureRTList;
	typedef std::vector< CRVertexBufferGroup* >	VertexBufferGroupList;
	
private:
	EffectTemplateMap	m_EffectTemplateStash;	// All loaded effect template go here
	Texture2DMap		m_Texture2DStash;		// All loaded textures go here
	EffectMultiMap		m_EffectStash;			//
	Texture3DMap		m_Texture3DStash;
	IndexBufferMap		m_IBStash;		
	VertexBufferMap		m_VBStash;
	TextureCubeMap		m_CubeStash;
	VertexBufferGroupMap	m_VBGroupStash;
	
	DepthBufferList		m_DepthBufferStash;
	TextureRTList		m_TextureRTStash;

	IndexBufferList		m_IBNoIDStash;
	VertexBufferList	m_VBNoIDStash;
	EffectList			m_EffectNoIDStash;
	EffectTemplateList	m_EffectTemplateNoIDStash;
	Texture2DList		m_Texture2DNoIDStash;
	Texture3DList		m_Texture3DNoIDStash;
	TextureCubeList		m_CubeNoIDStash;
	VertexBufferGroupList	m_VBGroupNoIDStash;
	
protected:
	CRRenderer*				m_pRenderer;
	
public:
	CRResourceMgr(CRRenderer* pRenderer)
		: m_pRenderer(pRenderer)
	{
		CRCDataKey emptyKey;
		emptyKey.hashVal = 0x00000000;
		emptyKey.pHashData = _NEW CByteBuffer(4);
		AppendData(emptyKey.pHashData, (uint) 0xDEADBEEF);
		
		CRCDataKey deleteKey;
		deleteKey.hashVal = 0xFFFFFFFF;
		deleteKey.pHashData = emptyKey.pHashData;
	
		m_EffectTemplateStash.set_deleted_key(deleteKey);
		m_EffectTemplateStash.set_empty_key(emptyKey);
		m_Texture2DStash.set_deleted_key(deleteKey);
		m_Texture2DStash.set_empty_key(emptyKey);
		m_Texture3DStash.set_deleted_key(deleteKey);
		m_Texture3DStash.set_empty_key(emptyKey);
		m_EffectStash.set_deleted_key(deleteKey);
		m_EffectStash.set_empty_key(emptyKey);
		m_IBStash.set_deleted_key(deleteKey);
		m_IBStash.set_empty_key(emptyKey);
		m_VBStash.set_deleted_key(deleteKey);
		m_VBStash.set_empty_key(emptyKey);
		m_CubeStash.set_deleted_key(deleteKey);
		m_CubeStash.set_empty_key(emptyKey);
		
		m_VBGroupStash.set_deleted_key(deleteKey);
		m_VBGroupStash.set_empty_key(emptyKey);	
	}
	
	virtual ~CRResourceMgr()
	{
	}

private:
	//MD5Digest GenerateID(const wchar* pName);

protected:
	_PURE( IRRenderGroup* DoCreateRenderGroup(RenderOpSort sortFn, uint maxRenderOps) )

	_PURE( CREffectTemplate* DoLoadEffectTemplate(IByteBuffer* pData) )
	_PURE( CRTexture2D* DoLoadTexture2D(IByteBuffer* pData) )
	_PURE( void DoSaveTexture2D(IRTexture2D* pTex, IByteBuffer* pData) )
	_PURE( CRTextureCube* DoLoadTextureCube(IByteBuffer* pData) )
	_PURE( CRTexture3D* DoLoadTexture3D(IByteBuffer* pData) )

	_PURE( CRTextureCube* DoCreateTextureCube(uint size, uint mipMapLevel, eRTexFormat format, eRTexUsage usage) )
	_PURE( CRTextureCube* DoCreateTextureCubeRT(uint size, eRTexFormat format) )

	_PURE( CRTexture2D* DoCreateTexture2D(uint width, uint height, uint mipMapLevel, eRTexFormat format, eRTexUsage usage) )
	_PURE( CRTexture3D* DoCreateTexture3D(uint width, uint height, uint depth, uint mipMapLevel, eRTexFormat format, eRTexUsage usage) )

	_PURE( CRTexture2D* DoCreateTexture2DRT(uint width, uint height, uint mipLevels, eRTexFormat format) )
	_PURE( CRDepthBuffer* DoCreateDepthBuffer(uint width, uint height, eRDepthFormat format, uint multiSampleQuality) )
											
	_PURE( CRVertexBuffer* DoCreateVertexBuffer(uint bufferLen, eRBufferUsage usage) )
	_PURE( CRIndexBuffer* DoCreateIndexBuffer(uint bufferLen, eRBufferUsage usage) )
	_PURE( CRVertexBufferGroup* DoCreateVertexBufferGroup() )
	
	_PURE( CRVertexBuffer* DoCreateVertexBufferShared(uint bufferLen, CRVertexBuffer* pSource) )
	_PURE( CRIndexBuffer* DoCreateIndexBufferShared(uint bufferLen, CRIndexBuffer* pSource) )

	_PURE( CRTextureRT* DoCreateBackBufferColor() )
	_PURE( CRDepthBuffer* DoCreateBackBufferDepth() )

	_PURE( void DoOnLostDevice() )
	_PURE( void DoOnRestoreDevice() )
	_PURE( void DoOnResetDevice() )
		
public:
	void OnLostDevice();	
	void OnResetDevice();
	void OnRestoreDevice();
	
	void OnDestroyResource(const CRCDataKey& id, eRResourceType type, IRResource* pRes);

	IRRenderGroup* CreateRenderGroup(RenderOpSort sortFn, uint maxOps);

	IREffectTemplate* LoadEffectTemplate(const CRCDataKey* pKey, 
										IByteBuffer* pData);
	IREffect* LoadEffect(const CRCDataKey* pTemplateKey, 
						IByteBuffer* pTemplateData,
						const REffectParam* pConstantParams,
						const uint* pSemantics,
						uint numParams);
						
	IREffect* LoadEffect(IREffectTemplate* pTemplate,
		const REffectParam* pConstantParams,
		const uint* pSemantics,
		uint numParams);
		
	IRTexture2D* LoadTexture2D(const CRCDataKey* pKey, IByteBuffer* pData);
	IRTexture3D* LoadTexture3D(const CRCDataKey* pKey, IByteBuffer* pData);
	IRTextureCube* LoadTextureCube(const CRCDataKey* pKey, IByteBuffer* pData);
	void SaveTexture2D(IRTexture2D* pTex, IByteBuffer* pData);
	
	IRTextureCube* CreateTextureCube(const CRCDataKey* pKey, 
								uint size, 
								uint mipMapLevel, 
								eRTexFormat format,
								eRTexUsage usage);

	IRTextureCube* CreateTextureCubeRT(uint size, eRTexFormat format);

	IRTexture3D* CreateTexture3D(const CRCDataKey* pKey, 
								uint width, 
								uint height,
								uint depth,
								uint mipMapLevel, 
								eRTexFormat format,
								eRTexUsage usage);

	IRTexture2D* CreateTexture2D(const CRCDataKey* pKey, 
								uint width, 
								uint height, 
								uint mipMapLevel, 
								eRTexFormat format,
								eRTexUsage usage);
	
	IRTexture2D* CreateTexture2DRT(uint width, 
								uint height,
								uint mipLevels,
								eRTexFormat format);
	
	IRDepthBuffer* CreateDepthBuffer(uint width, 
									uint height, 
									eRDepthFormat format,
									uint multiSampleQuality);
								
	IRVertexBuffer* CreateVertexBuffer(const CRCDataKey* pKey, const RVertexDesc* pDesc, uint numVertices, eRBufferUsage usage);
	IRVertexBuffer* CreateVertexBufferShared(const RVertexDesc* pDesc, uint numVertices, IRVertexBuffer* pSource);

	IRIndexBuffer* CreateIndexBuffer(const CRCDataKey* pKey, uint numIndices, eRBufferUsage usage);
	IRIndexBuffer* CreateIndexBufferShared(uint numIndices, IRIndexBuffer* pSource);

	IRVertexBufferGroup* CreateVertexBufferGroup(const CRCDataKey* pKey);

	IRTextureRT* CreateBackBufferColor();
	IRDepthBuffer* CreateBackBufferDepth();
};

_NAMESPACE_END