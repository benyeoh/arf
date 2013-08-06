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

struct DEFXLaFortuneLobe
{
	float Cx;
	float Cy;
	float Cz;
	float n;
};

struct DEFXLaFortuneBRDFParam
{
	float nDotLScale;
	float metalScale;
	float lobeScale;
	float diffuseScale;
};

struct DEFXPointLight
{
	gmtl::Vec3f pos;	// viewPos
	gmtl::Vec4f color;
	float		invRange;
};

struct DEFXDirLight
{
	gmtl::Vec3f dir;
	gmtl::Vec4f color;
};

struct IDEFXBRDFTable : public IRefCounted
{
	_PURE( const DEFXLaFortuneBRDFParam& GetParams() )
	_PURE( IRTexture2D* GetData() )

	_PURE( void GenerateData(const DEFXLaFortuneLobe* pLobes, uint numLobes, const wchar* pDataFileName, float maxOverbrightNormalize=0.0f) )

	_PURE( void SetData(IRTexture2D* pData) )
	_PURE( void SetParams(const DEFXLaFortuneBRDFParam& params) )
};

struct IDEFXBRDFTableAtlas : public IRefCounted
{
	_PURE( void GetData(IRTexture2D** ppBRDFAtlas, IRTexture2D** ppBRDFParams) )

	_PURE( void GenerateData(IDEFXBRDFTable** ppTables, uint numTables, const wchar* pAtlasFileName, const wchar* pParamFileName) )
	_PURE( void SetData(IRTexture2D* pBRDFAtlas, IRTexture2D* pBRDFParams) )
};

struct IDEFXLightGroup : public IRefCounted
{
	_PURE( void SetViewport(uint left, uint top, uint width, uint height) )
	_PURE( void AddRenderTarget(IRTextureRT* pRT) )
	_PURE( void ClearRenderTargets() )

	//_PURE( uint GetNumOfRenderTargets() )
	//_PURE( IRTextureRT* GetRenderTarget(uint rtIndex) )

	_PURE( void SetDepthBuffer(IRDepthBuffer* pDepth) )

	_PURE( void Flush() )
	_PURE( uint AddContext() )
	_PURE( uint GetNumOfContexts() )
	_PURE( void ResetContext(uint contextID) )
};

struct IDEFXParamPool : public IBFXParamCallback, public IRefCounted
{
	// TODO: Some of these should be shared params
	_PURE( void SetAccumulationBuffers(IRTexture2D* pZPos, IRTexture2D* pNormals, IRTexture2D* pAlbedo) )
	_PURE( void SetCameraFarPlaneCornerVecs(gmtl::VecA3f* pCorners) )
	_PURE( void SetViewFarPlaneOffsets(gmtl::Vec3f* pTopLeftCorner, float xRange, float yRange) )

	_PURE( void SetCurrentLightGroup(IDEFXLightGroup* pLightGroup) )
	_PURE( void SetPointLight(DEFXPointLight* pPointLight) )
	_PURE( void SetDirLight(DEFXDirLight* pDirLight) )

	//_PURE( void SetBRDFAtlas(IDEFXBRDFTableAtlas* pAtlas) )
};

struct IDEFXPipeline : public IBFXRenderCallback, public IRefCounted
{
	_PURE( void SetViewport(uint left, uint top, uint width, uint height) )
	_PURE( void AddAccumulationTargets(IRTextureRT* pZPos, IRTextureRT* pNormal, IRTextureRT* pMaterial) )
	_PURE( void AddRenderTarget(IRTextureRT* pRT) )
	_PURE( void ClearRenderTargets() )

	_PURE( void SetDepthBuffer(IRDepthBuffer* pDepth) )

	_PURE( void GetAccumulationTargets(IRTextureRT** ppZPos, IRTextureRT** ppNormal, IRTextureRT** ppMaterial, IRTextureRT** ppExtra, uint& numExtra) )

	_PURE( void Flush() )
	_PURE( uint AddContext() )
	_PURE( uint GetNumOfContexts() )
	_PURE( void ResetContext(uint contextID) )

		//   _PURE( void Enable2XDownsampleAccum() )
		//   _PURE( void Disable2XDownsampleAccum() )
		//_PURE( void UpdateScreenSize(uint width, uint height) )
};

struct IDEFXResourceMgr
{
	_PURE( IDEFXPipeline* CreatePipeline() )
	_PURE( IDEFXParamPool* CreateParamPool() )
	_PURE( IDEFXLightGroup* CreateLightGroup() )

	_PURE( IDEFXBRDFTableAtlas* CreateBRDFAtlas(IRTexture2D* pBRDFAtlas, IRTexture2D* pBRDFParams) )
	_PURE( IDEFXBRDFTable* CreateBRDFTable(const DEFXLaFortuneBRDFParam& param, IRTexture2D* pData) )
};

struct IDEFXDeferredFX : public IRefCounted
{
	_PURE( IDEFXResourceMgr& GetResourceMgr() )
	_PURE( IAppCallback& GetAppCallback() )

	_PURE( boolean Initialize(IRRenderer& renderer, IAppCallback& callback) )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IDEFXDeferredFX)
_DECLARE_SMARTPTR(IDEFXParamPool)
_DECLARE_SMARTPTR(IDEFXPipeline)
_DECLARE_SMARTPTR(IDEFXBRDFTable)
_DECLARE_SMARTPTR(IDEFXBRDFTableAtlas)
_DECLARE_SMARTPTR(IDEFXLightGroup)

//////////////////////////////////////////////////////////////////////////
//	Helper Structs
//
//////////////////////////////////////////////////////////////////////////

_NAMESPACE_END


//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IDEFXDeferredFX* (*CreateDeferredFXFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE IDEFXDeferredFX* CreateDeferredFX() )
}
