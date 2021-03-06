//================================================================================
//
//		RendererD3D11Framework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/3/2013
//
//================================================================================

#pragma once

#include <windows.h>

// Platform independent stuff
#include <CoreMemory\CoreMemory.h>
#include <Renderer\RendererFramework.h>

#include <D3D11.h>
#include <D3DX11.h>
#include <D3DX10.h>

_NAMESPACE_BEGIN

_DECLARE_SMARTPTR(ID3D11Device)
_DECLARE_SMARTPTR(ID3D11DeviceContext)
_DECLARE_SMARTPTR(IDXGISwapChain)
_DECLARE_SMARTPTR(ID3D11RenderTargetView)
_DECLARE_SMARTPTR(ID3D11DepthStencilView)
_DECLARE_SMARTPTR(ID3D11Texture2D)
_DECLARE_SMARTPTR(ID3D11Texture3D)
_DECLARE_SMARTPTR(ID3D11ShaderResourceView)
_DECLARE_SMARTPTR(ID3D11UnorderedAccessView)
_DECLARE_SMARTPTR(ID3D11Buffer)
_DECLARE_SMARTPTR(ID3D11Resource)
_DECLARE_SMARTPTR(ID3D11VertexShader)
_DECLARE_SMARTPTR(ID3D11PixelShader)
_DECLARE_SMARTPTR(ID3D11GeometryShader)
_DECLARE_SMARTPTR(ID3D11HullShader)
_DECLARE_SMARTPTR(ID3D11DomainShader)
_DECLARE_SMARTPTR(ID3D11ComputeShader)
_DECLARE_SMARTPTR(ID3D11BlendState)
_DECLARE_SMARTPTR(ID3D11RasterizerState)
_DECLARE_SMARTPTR(ID3D11DepthStencilState)
_DECLARE_SMARTPTR(ID3D11DeviceChild)

class CRRendererD3D11;

struct ScratchPad
{
	void*	pMem;
	uint	size;

	ScratchPad()
		: pMem(NULL)
		, size(0)
	{}

	~ScratchPad()
	{
		Realloc(0);
	}

	void Realloc(uint newSize)
	{
		if(pMem)
			_FREE(pMem);

		if(newSize > 0)
			pMem = _MALLOC(newSize);

		size = newSize;
	}
};

_NAMESPACE_END

#include "D3D11HelperFns.h"

#include "CRTextureRTD3D11.h"
#include "CRTexture2DD3D11.h"
#include "CRVertexBufferD3D11.h"
#include "CRIndexBufferD3D11.h"
#include "CRTextureCubeD3D11.h"
#include "CREffectTemplateD3D11.h"
#include "CRResourceMgrD3D11.h"
#include "CRRendererD3D11.h"
