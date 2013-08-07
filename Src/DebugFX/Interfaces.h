//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/21/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct IDFXParams
{
	_PURE( void SetViewMatrix(const gmtl::Matrix44f& view) )
	_PURE( void SetProjMatrix(const gmtl::Matrix44f& proj) )
};

struct IDFXResourceMgr
{

};

struct IDFXDebugFX : public IRefCounted
{
	_PURE( void SetViewport(uint left, uint top, uint width, uint height) )
	_PURE( void AddRenderTarget(IRTextureRT* pTexRT) )
	_PURE( void ClearRenderTargets() )
	_PURE( void SetDepthBuffer(IRDepthBuffer* pDepth) )

	_PURE( void BeginWireFrame() )
	_PURE( void EndWireFrame() )

	_PURE( IDFXParams& GetParams() )
	_PURE( boolean Initialize(IRRenderer& renderer, IAppCallback& callback) )
	
	_PURE( void RenderWireBox(const AABox& box,
		const gmtl::Vec4f& color) )
		//const gmtl::Matrix44f& transform) )
	_PURE( void RenderWireBox(const OOBox& box,
		const gmtl::Vec4f& color) )
		
	_PURE( void RenderWireSphere(const gmtl::Vec3f& pos,
		float radius,
		const gmtl::Vec4f& color) )
	_PURE( void RenderLine(const gmtl::Vec3f& pos1, 
		const gmtl::Vec3f& pos2, 
		const gmtl::Vec4f& color) )	
		
	_PURE( void RenderQuad(const gmtl::Vec3f& pos, 
		const gmtl::Vec2f& size,
		const gmtl::Vec2f& topLeftTexPos,
		const gmtl::Vec2f& texSize,
		IRTexture2D* pTex) )
	//_PURE( void RenderQuad(const gmtl::Vec3f& pos, 
	//	const gmtl::Vec2f& size,
	//	const gmtl::Vec2f& topLeftTexPos,
	//	const gmtl::Vec2f& texSize,
	//	IRTextureRT* pTex) )
		
	_PURE( void Flush() )

	_PURE( IDFXResourceMgr& GetResourceMgr() )
};


//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IDFXDebugFX)

_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IDFXDebugFX* (*CreateDebugFXFn)();
	DEBUGFX_API _NAMESPACE IDFXDebugFX* CreateDebugFX();
}