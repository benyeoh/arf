//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/12/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct ISAOFXPipeline
{
    _PURE( void Render() )

};

//struct ISAOFXParamPool
//{
//
//
//};
//
//struct ISAOFXResourceMgr
//{
//
//};


struct ISAOFXScreenAmbOcclFX : public IRefCounted
{
    _PURE( boolean Initialize(IDEFXDeferredFX& deferredFX, IBFXBaseFX& baseFX, IRRenderer& renderer, IAppCallback& callback) )

    _PURE( ISAOFXPipeline&      GetPipeline() )
   // _PURE( ISAOFXParamPool&		GetParamPool() )
   // _PURE( ISAOFXResourceMgr&	GetResourceMgr() )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(ISAOFXScreenAmbOcclFX)


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
    typedef _NAMESPACE ISAOFXScreenAmbOcclFX* (*CreateScreenAmbOcclFXFn)();
    SCREENAMBOCCLFX_API _NAMESPACE ISAOFXScreenAmbOcclFX* CreateScreenAmbOcclFX();
}
