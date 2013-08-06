//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/21/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

struct BFXMesh;
struct BFXParamContainer;
struct BFXRenderContainer;
struct BFXRenderContext;
struct IBFXMaterialGroup;

struct BFXHemiLight
{
	gmtl::Vec4f skyColor;
	gmtl::Vec4f groundColor;
};

struct BFXDirLight
{
	gmtl::Vec3f dir;
	gmtl::Vec4f color;	
	float invSize;
};

struct BFXPointLight
{
	gmtl::Vec3f pos;
	gmtl::Vec4f color;
	float		invRange;
	float		invSize;	// For modelling specular reflections
};

struct BFXSpotLight
{
	IRTexture2DPtr	pLightAttenTex;
	gmtl::Vec4f		color;
	gmtl::Matrix44f proj;
	float			range;
	float			invSize;
};

struct IBFXParamCallback
{
	_PURE( void GetParam(uint semantic, REffectParam* pToStore) )
	_PURE( boolean IsParamAvailable(uint semantic) )
	_PURE( boolean IsMorePassesRequired() )

	_PURE( void FinishedResolvingEffect() )
	_PURE( void ResetParams() )
};

struct IBFXMaterialCreateCallback
{
	_PURE( IREffect* CreateEffect(IBFXMaterialGroup* pGroup, uint type, uint availability) ) 
};

struct IBFXRenderCallback
{
	_PURE( void RenderEffect(BFXRenderContext& renderContext) )
};

struct IBFXMaterial : public IRefCounted
{
	_PURE( uint GetNumOfConstantVolatileParams() )
	_PURE( uint GetNumOfDynamicVolatileParams() )

	_PURE( uint GetConstantVolatileParamSemantic(uint index) )
	_PURE( uint GetDynamicVolatileParamSemantic(uint index) )

	_PURE( IREffectTemplate* GetEffectTemplate(uint paramAvailability) )
};

struct IBFXMaterialGroupTemplate : public IRefCounted
{
	_PURE( const wchar* GetDesc() )
	_PURE( const wchar* GetFileName() )
	
	_PURE( const CRCDataKey& GetID() )
	_PURE( uint GetNumOfMaterials() )
	_PURE( IBFXMaterial* GetMaterialByIndex(uint index) )
	_PURE( IBFXMaterial* GetMaterialByType(uint type) )
	_PURE( uint GetMaterialTypeByIndex(uint index) )
};

struct IBFXMaterialGroup : public IRefCounted
{
	_PURE( IBFXMaterialGroupTemplate* GetTemplate() )
	
	_PURE( uint GetNumOfParams() )
	_PURE( void GetParam(uint index, REffectParam& param) )
	_PURE( uint GetSemantic(uint index) )
	
	_PURE( IREffect* CreateEffect(uint type, uint availabilityFlag) )
	_PURE( IREffect* GetEffect(uint type, uint availabilityFlag) )
	_PURE( uint GetParamAvailability(uint type, BFXParamContainer& container) )
	
	_PURE( const CRCDataKey& GetID() )
};

struct IBFXSharedParamData : public IRefCounted
{
};

struct IBFXParamPool : public IBFXParamCallback, public IRefCounted
{
	// Shared params
	_PURE( void SetViewMatrix(const gmtl::Matrix44f& view) )
	_PURE( void SetProjMatrix(const gmtl::Matrix44f& proj) )
	_PURE( void SetCameraProperties(const gmtl::Vec3f& cameraPos, float nearPlane, float farPlane) )	

	// NOTE: Must be called before shared params are fetched from multiple threads
	_PURE( void FinalizeSharedParams() )	

	_PURE( void SetWorldMatrix(const gmtl::Matrix44f& world) )
	_PURE( void SetZDistanceFromCamera(float dist) )	
	_PURE( void SetHemiLight(const BFXHemiLight* pHemiLight) )	
	_PURE( void SetDirLight(const BFXDirLight* pDirLight) )
	_PURE( void SetSpotLights(const BFXSpotLight** ppSpotLight, uint numSpotLights) )
	_PURE( void SetPointLights(const BFXPointLight** ppPointLight, uint numPointLights) )
	
	_PURE( void SetCurrentContext(uint id) )
	_PURE( uint GetCurrentContext() )

	_PURE( float GetZDistanceFromCamera() )	
};

struct IBFXDebug
{
	_PURE( void SetWireFrame(boolean isEnabled) )
};

struct IBFXPipeline : public IBFXRenderCallback, public IRefCounted
{
	// For deferred rendering / z-pass
	//_PURE( void DisableMainGroupDepthClearVolatile() )

	_PURE( void SetViewport(uint left, uint top, uint width, uint height) )
	_PURE( void AddRenderTarget(IRTextureRT* pTexRT) )
	_PURE( void ClearRenderTargets() )

	_PURE( void SetDepthBuffer(IRDepthBuffer* pDepth) )

	_PURE( void FlushOpaqueGroup() )
	_PURE( void FlushAlphaTestedGroup() )
	_PURE( void FlushAlphaBlendedGroup() )

	_PURE( void ResetGroup() )

	_PURE( uint AddContext() )
	_PURE( uint GetNumOfContexts() )
	_PURE( void ResetContext(uint contextID) )
};

struct IBFXResourceMgr
{
	_PURE( IBFXMaterialGroupTemplate* CreateMaterialGroupTemplate(const CRCDataKey* pKey, IByteBuffer* pBuffer, const wchar* pFileName) )	
	_PURE( IBFXMaterialGroup* CreateMaterialGroup(const CRCDataKey* pKey, 
												IBFXMaterialGroupTemplate* pTemplate,
												const REffectParam* pConstantParams,
												const uint* pSemantics,
												uint numParams) )

	_PURE( IBFXSharedParamData* CreateSharedParamData() )
	_PURE( IBFXPipeline* CreatePipeline() )
	_PURE( IBFXParamPool* CreateParamPool(IBFXSharedParamData* pSharedParam) )
};

struct IBFXBaseFX : public IRefCounted
{
	//_PURE( void SetParamCallback(IBFXParamCallback* pModule, uint offsetStart) )
	//_PURE( void SetRenderCallback(IBFXRenderCallback* pModule, uint materialType) )
	
	_PURE( IBFXResourceMgr& GetResourceMgr() )
	//_PURE( IBFXParamPool& GetParamPool() )
	_PURE( IAppCallback& GetAppCallback() )
	//_PURE( IBFXDebug& GetDebug() )
	//_PURE( IBFXPipeline& GetPipeline() )
	
	_PURE( boolean Initialize(IRRenderer& renderer, IMGMaterialGen& matGen, IAppCallback& callback) )
	
	_PURE( void Render(BFXMesh& mesh, float detailLevel, uint matType, uint pipelineMatType, BFXParamContainer& paramContainer, BFXRenderContainer& renderContainer, IBFXMaterialCreateCallback* pCreateCallback=NULL) )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IBFXBaseFX)
_DECLARE_SMARTPTR(IBFXMaterialGroup)
_DECLARE_SMARTPTR(IBFXMaterialGroupTemplate)
_DECLARE_SMARTPTR(IBFXMaterial)
_DECLARE_SMARTPTR(IBFXSharedParamData)
_DECLARE_SMARTPTR(IBFXParamPool)
_DECLARE_SMARTPTR(IBFXPipeline)

//////////////////////////////////////////////////////////////////////////
//	Helper Structs
//
//////////////////////////////////////////////////////////////////////////
struct BFXMesh
{
	IRVertexBufferGroupPtr pVBGroup;
	IRIndexBufferPtr pIB;
	IBFXMaterialGroupPtr pMatGroup;
};

struct BFXRenderContainer
{
	IBFXRenderCallback* renderCallbacks[BFX_MAX_NUM_LIBS];

	IBFXRenderCallback* GetRenderCallback(uint libIndex)
	{
		return renderCallbacks[libIndex];
	}

	void SetRenderCallback(IBFXRenderCallback* pCallback, uint libIndex)
	{
		//_DEBUG_ASSERT(renderCallbacks[libIndex] == NULL);
		renderCallbacks[libIndex] = pCallback;		
	}
};

struct BFXParamContainer
{
	IBFXParamCallback* paramCallbacks[BFX_MAX_NUM_LIBS];
	
	IBFXParamCallback* GetParamCallback(uint libIndex)
	{
		return paramCallbacks[libIndex];
	}

	void SetParamCallback(IBFXParamCallback* pCallback, uint libIndex)
	{
		//_DEBUG_ASSERT(paramCallbacks[libIndex] == NULL);
		paramCallbacks[libIndex] = pCallback;		
	}
};

struct BFXRenderContext
{
	uint			matType;
	BFXMesh*		pMesh;
	IREffect*		pEffect;
	uint			tech;
	REffectParam*	pDynamicParams;
	uint			numParams;
	
	BFXParamContainer* pContainer;

	uint			numIters;
};

_NAMESPACE_END


//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IBFXBaseFX* (*CreateBaseFXFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE IBFXBaseFX* CreateBaseFX() )
}
