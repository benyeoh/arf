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

class IBLFXParamPool
{
	_PURE( void SetExposure(float val) )
	_PURE( float GetExposure() )

	//_PURE( void SetBloomOffset(float val) )
	//_PURE( float GetBloomOffset() )

	_PURE( void SetBloomThreshold(float val) )
	_PURE( float GetBloomThreshold() )
};

struct IBLFXPipeline
{
};

struct IBLFXBloomFX : public IRefCounted
{
	_PURE( boolean Initialize(IRRenderer& renderer, IAppCallback& callback) )
	
	_PURE( void SetBloom(boolean isEnabled) )
	_PURE( void SetStarGlare(boolean isEnabled) )
	
	_PURE( void Render() )
	
	_PURE( IRTextureRT* GetDownSampledSource() )
	_PURE( IBLFXParamPool& GetParamPool() )
	
	_PURE( IRRenderer& GetRenderer() )
	_PURE( IAppCallback& GetAppCallback() )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IBLFXBloomFX)

_NAMESPACE_END


//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IBLFXBloomFX* (*CreateBloomFXFn)();
	BLOOMFX_API _NAMESPACE IBLFXBloomFX* CreateBloomFX();
}
