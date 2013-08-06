//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/30/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN


struct IFFXParams
{

};

struct IFFXFontFace : public IRefCounted
{
	//_PURE( void RenderText(const wchar* pStr, const gmtl::Vec2f& pos) )
	_PURE( void RenderString(const wchar* pStr, const gmtl::Vec2i& pos, uint color) )
	_PURE( void Flush() )
};

struct IFFXResourceMgr
{
	_PURE( IFFXFontFace* CreateFontFace(IByteBuffer* pFontData, uint pointSize, uint faceIndex) )
};

struct IFFXFontFX : public IRefCounted
{
	_PURE( boolean Initialize(IRRenderer& renderer, IAppCallback& callback) )	
	
	_PURE( void SetViewport(uint left, uint top, uint width, uint height) )
	_PURE( void AddRenderTarget(IRTextureRT* pTexRT) )
	_PURE( void ClearRenderTargets() )

	_PURE( IFFXResourceMgr& GetResourceMgr() )
	_PURE( IFFXParams& GetParams() )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IFFXFontFace)
_DECLARE_SMARTPTR(IFFXFontFX)

_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IFFXFontFX* (*CreateFontFXFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE IFFXFontFX* CreateFontFX() )
}
