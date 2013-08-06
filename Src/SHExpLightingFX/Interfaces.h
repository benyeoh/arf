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

struct ISEFXParamPool
{
	_PURE( void SetRadianceCoeff(IRTextureCube** ppCoeff, uint numCubeMapsPerColor)	)
	_PURE( void SetUnitCoeff(float* pUnitCoeff, uint numBands) )

	_PURE( void SetVisibilitySpheresCoeffTex(IRTexture2D* pTex)	)
	_PURE( void SetABTableTex(IRTexture2D* pTex) )
	_PURE( void SetYlmTextures(IRTextureCube** ppCoeff, uint numCubeMaps) )
};

struct ISEFXResourceMgr
{
	_PURE( void	CreateUnitSHCoeffs(float* pDest, uint numSamplesPerSide, uint numBands) )
	_PURE( void	CreateRadianceCosineLobeSHCoeffs(IRTextureCube** ppDest, IRTextureCube* pSource, uint numBands) )
	_PURE( void CreateCosineLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands) )
	_PURE( void	CreateYlmCoeffs(IRTextureCube** ppDest, uint numBands) )
	_PURE( void	CreateSphereVisibilityCoeffs(IRTexture2D* pDest, IRTexture2D* pABTableTex, uint numBands) )

	// To refactor
	_PURE( void	ConvertToZH(float* pZH,  gmtl::Vec3f* pLobeAxis, const float* pSH, uint numBands, void* pD3DDevice) )
};

struct ISEFXPipeline
{
	_PURE( void UpdateScreenSize(uint width, uint height) )
	_PURE( void Render(const Sphere* pSpheres,
					uint numSpheres,
					const Sphere& coverage, 
					const gmtl::MatrixA44f& coverageWorld,
					const AABox& screenCoverage) )
};

struct ISEFXSHExpLightingFX : public IRefCounted
{
	_PURE( boolean Initialize(IDEFXDeferredFX& deferredFX, IBFXBaseFX& baseFX, IRRenderer& renderer, IAppCallback& callback) )

	_PURE( ISEFXParamPool&		GetParamPool() )
	_PURE( ISEFXResourceMgr&	GetResourceMgr() )
	_PURE( ISEFXPipeline&		GetPipeline() )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(ISEFXSHExpLightingFX)


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
	typedef _NAMESPACE ISEFXSHExpLightingFX* (*CreateSHExpLightingFXFn)();
	SHEXPLIGHTINGFX_API _NAMESPACE ISEFXSHExpLightingFX* CreateSHExpLightingFX();
}
