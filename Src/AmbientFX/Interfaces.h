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

struct AFXAmbientCoeffs
{
	//gmtl::Vec3f color[9];
	gmtl::VecA4f rTerm1;
	gmtl::VecA4f rTerm2;
	gmtl::VecA4f gTerm1;
	gmtl::VecA4f gTerm2;
	gmtl::VecA4f bTerm1;
	gmtl::VecA4f bTerm2;
	gmtl::VecA3f rgbTerm;
};

// Transfer matrix for easier computation in SIMD/GPU
struct AFXTransferMatrix
{
	gmtl::VecA4f terms[9 * 2];
	gmtl::VecA4f scalars[2];
	float		scalarLast;

	void Set(const Matrix99f& src)
	{
		_LOOPi(9)
		{
			terms[i * 2][0] = src[i][0];
			terms[i * 2][1] = src[i][1];
			terms[i * 2][2] = src[i][2];
			terms[i * 2][3] = src[i][3];

			terms[i * 2 + 1][0] = src[i][4];
			terms[i * 2 + 1][1] = src[i][5];
			terms[i * 2 + 1][2] = src[i][6];
			terms[i * 2 + 1][3] = src[i][7];
		}

		scalars[0][0] = src[0][8];
		scalars[0][1] = src[1][8];
		scalars[0][2] = src[2][8];
		scalars[0][3] = src[3][8];

		scalars[1][0] = src[4][8];
		scalars[1][1] = src[5][8];
		scalars[1][2] = src[6][8];
		scalars[1][3] = src[7][8];

		scalarLast = src[8][8];
	}
};

struct IAFXParamPool : public IBFXParamCallback, public IRefCounted
{
	_PURE( void SetAmbientSHCoeff(AFXAmbientCoeffs* pSHCoeff) )
	_PURE( void SetRadianceCoeff(IRTextureCube** ppCoeff, uint numCubeMapsPerColor)	)
	_PURE( void SetUnitCoeff(float* pUnitCoeff, uint numBands) )
	_PURE( void SetTransferMatrixRGB(AFXTransferMatrix* pR, AFXTransferMatrix* pG, AFXTransferMatrix* pB) )
};

struct IAFXResourceMgr
{
	_PURE( IAFXParamPool* CreateParamPool() )

	_PURE( void CompressAmbientCubeMap(IRTextureCube* pTexCube, AFXAmbientCoeffs* pRGBOut, float freqContrast, float brightenFactor) )
};

struct IAFXSHLightToolKit
{
	_PURE( void	CreateUnitSHCoeffs(float* pDest, uint numSamplesPerSide, uint numBands) )
	_PURE( void	CreateRadianceCosineLobeSHCoeffs(IRTextureCube** ppDest, IRTextureCube* pSource, uint numBands) )
	_PURE( void CreateCosineLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands) )
	_PURE( void	CreateHemiLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands) )

	//_PURE( void	ConvertToZH(float* pZH,  gmtl::Vec3f* pLobeAxis, const float* pSH, uint numBands, void* pD3DDevice) )
};

struct IAFXAmbientFX : public IRefCounted
{
	_PURE( boolean Initialize(IAppCallback& callback) )
	_PURE( void LerpAmbientSHCoeff(AFXAmbientCoeffs& dest, const AFXAmbientCoeffs& src1, const AFXAmbientCoeffs& src2, float amount) )

	_PURE( IAFXSHLightToolKit& GetSHToolKit() )
	_PURE( IAFXResourceMgr&	GetResourceMgr() )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IAFXAmbientFX)
_DECLARE_SMARTPTR(IAFXParamPool)


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
	typedef _NAMESPACE IAFXAmbientFX* (*CreateAmbientFXFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE IAFXAmbientFX* CreateAmbientFX() )
}
