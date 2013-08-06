//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct SHFXCascadedSplitZ
{
	float splitNear[4];
	float splitFar[4];
};

struct SHFXCascadedProj
{
	gmtl::MatrixA44f cascadedViewProj[4];
};

struct SHFXCascadedFrustumBounds
{
	gmtl::VecA4f bounds[4];

	// Frustum bounds
	// 4 split planes + 1 for far plane
	gmtl::VecA4f frustumSplits[4 * (4+1)];
};

struct ISHFXPointLight : public IRefCounted
{
	// Note: For point lights, the front face is always facing in +y
	_PURE( void Update(const gmtl::Vec3f& pos) )
	_PURE( void SetRange(float range) )

	_PURE( IRTexture2D*	GetShadowMap() )
	//_PURE( IRTextureRT*	GetShadowMapBackup() )

	_PURE( void SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTexture2D* pFinal, IRDepthBuffer* pDepth, float stdDev, uint spreadKernelSizes) )
	_PURE( void SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTexture2D* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur) )
	_PURE( void SetShadowMapParams(IRTexture2D* pFinal, IRDepthBuffer* pDepth) )

	_PURE( void Flush() )
	_PURE( uint AddContext() )
	_PURE( uint GetNumOfContexts() )
	_PURE( void ResetContext(uint contextID) )

	//_PURE( void FlushCasterGroups() )
	//_PURE( void FlushFilterGroups() )
};

struct ISHFXCubePointLight : public IRefCounted
{
	_PURE( void Update(const gmtl::Vec3f& pos) )
	_PURE( void SetRange(float range) )
	_PURE( void SetRangeNoUpdate(float range) )

	_PURE( gmtl::MatrixA44f* GetViewProj(eRCubeFace face) )
	_PURE( float GetViewProjNearPlane() )
	_PURE( float GetViewProjFarPlane() )

	_PURE( IRTextureCube*	GetShadowMap() )
	//_PURE( IRTextureRT*	GetShadowMapBackup() )

	_PURE( void SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTextureCube* pFinal, IRDepthBuffer* pDepth, float stdDev, uint spreadKernelSizes) )
	_PURE( void SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTextureCube* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur) )
	_PURE( void SetShadowMapParamsCubePoissonFilter(IRTextureCube* pSM, IRTextureCube* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur) )
	_PURE( void SetShadowMapParams(IRTextureCube* pFinal, IRDepthBuffer* pDepth) )

	_PURE( void Flush() )
	_PURE( uint AddContext() )
	_PURE( uint GetNumOfContexts() )
	_PURE( void ResetContext(uint contextID) )
};

struct ISHFXCascadedDirLight : public IRefCounted
{
	//_PURE( void UpdateZSplits(float nearBiasExponent, float nearViewZ, float farViewZ) )
	//_PURE( void UpdateCascadedProj(const gmtl::Vec3f& lightDir, 
	//							const gmtl::Vec3f& lightPos, 
	//							const gmtl::Matrix44f& view, 
	//							const gmtl::Matrix44f& proj) )
	//_PURE( void Update(float nearBiasExponent, 
	//				const gmtl::Vec3f& lightDir, 
	//				const gmtl::Vec3f& lightPos, 
	//				const gmtl::Matrix44f& view, 
	//				const gmtl::Matrix44f& proj) )

	_PURE( void UpdateCascadedFrustumBounds(SHFXCascadedFrustumBounds& dest, float nearBiasExponent, const gmtl::Matrix44f& proj) )
	_PURE( void Update(const SHFXCascadedFrustumBounds& frustum, const gmtl::Vec3f& lightDir, const gmtl::Vec3f& lightPos, const gmtl::Matrix44f& view) )

	_PURE( const SHFXCascadedSplitZ& GetSplitZ() )
	_PURE( const SHFXCascadedProj& GetProj() )
	_PURE( const SHFXCascadedProj& GetProjCulling() )

	_PURE( void SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTexture2D* pFinal, IRDepthBuffer* pDepth, float* pStdDevs, uint* pSpreadKernelSizes) )
	_PURE( void SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTexture2D* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples* pBlurSamples, float* pSpreadScales, boolean isUseNoiseBlur) )
	_PURE( void SetShadowMapParams(IRTexture2D* pFinal, IRDepthBuffer* pDepth) )

	_PURE( IRTexture2D* GetShadowMap() )

	_PURE( void Flush() )
	_PURE( uint AddContext() )
	_PURE( uint GetNumOfContexts() )
	_PURE( void ResetContext(uint contextID) )
	//_PURE( void FlushCasterGroups() )
	//_PURE( void FlushFilterGroups() )
};

struct ISHFXParamPool : public IBFXParamCallback, public IRefCounted
{
	_PURE( void	SetZDistanceFromCamera(float zDist)	)
	
	//_PURE( void SetCascadedCasterIndex(uint index) )
	_PURE( void SetCascadedCasterFlag(uint cascadedFlag) )
	_PURE( void SetCascadedDirLight(ISHFXCascadedDirLight* pDirLight) )

	_PURE( void	SetPointLights(ISHFXPointLight** ppPointLight, uint* pCasterFlag, uint numPointLights) )

	_PURE( void	SetCubePointLights(ISHFXCubePointLight** ppPointLight, uint numPointLights) )
	_PURE( void	SetCasterCubePointLight(ISHFXCubePointLight* pCaster, eRCubeFace face) )

	_PURE( ISHFXCascadedDirLight* GetCascadedDirLight() )

};

struct ISHFXPipeline : public IBFXRenderCallback, public IRefCounted
{
};

struct ISHFXResourceMgr
{
	_PURE( ISHFXCascadedDirLight*	CreateCascadedDirLight() )
	_PURE( ISHFXPointLight*			CreatePointLight() )
	_PURE( ISHFXCubePointLight*	CreateCubePointLight() )

	//_PURE( IRTextureRT*	CreateCascadedDirLightExpShadowMap(uint size) )
	//_PURE( IRTextureRT* CreatePointLightExpShadowMap(uint size) )
	//_PURE( IRDepthBuffer* CreateShadowMapDepthBuffer(uint size) )

	_PURE( eRTexFormat GetSMCompatibleFormat() )
	_PURE( eRDepthFormat GetSMCompatibleDepthFormat() )

	_PURE( ISHFXPipeline* CreatePipeline() )
	_PURE( ISHFXParamPool* CreateParamPool() )
};

struct ISHFXDebug
{	
	_PURE( void SetViewport(uint left, uint top, uint width, uint height) )
	_PURE( void AddRenderTarget(IRTextureRT* pTexRT) )
	_PURE( void ClearRenderTargets() )
	_PURE( void SetDepthBuffer(IRDepthBuffer* pDepth) )

	_PURE( void RenderCascSM(const gmtl::Vec4f& screenSpacePos, const IRTexture2D* pCascSm) )
	_PURE( void RenderPLSM(const gmtl::Vec4f& screenSpacePos, const IRTexture2D* pPLSM) )
};

struct ISHFXShadowFX : public IRefCounted
{
	_PURE( boolean Initialize(IRRenderer& renderer, IAppCallback& callback) )

	_PURE( ISHFXResourceMgr&	GetResourceMgr() )
	_PURE( ISHFXDebug&			GetDebug() )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(ISHFXCascadedDirLight)
_DECLARE_SMARTPTR(ISHFXPointLight)
_DECLARE_SMARTPTR(ISHFXCubePointLight)
_DECLARE_SMARTPTR(ISHFXShadowFX)
_DECLARE_SMARTPTR(ISHFXPipeline)
_DECLARE_SMARTPTR(ISHFXParamPool)

_NAMESPACE_END


//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE ISHFXShadowFX* (*CreateShadowFXFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE ISHFXShadowFX* CreateShadowFX() )
}
