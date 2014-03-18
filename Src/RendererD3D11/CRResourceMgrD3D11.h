//==============================================================================
//
//		CRResourceMgrD3D11.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, March 25, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRResourceMgrD3D11 : public CRResourceMgr
{
private:

public:
	CRResourceMgrD3D11(CRRenderer* pRenderer)
		: CRResourceMgr(pRenderer)
	{
	}

	virtual ~CRResourceMgrD3D11()
	{
	}

protected:

	//IDirect3DVolumeTexture9* CreateD3DVolumeTexture(uint width, 
	//	uint height,
	//	uint depth,
	//	uint mipMapLevel, 
	//	D3DFORMAT format,
	//	DWORD usage,
	//	D3DPOOL pool);

	ID3D11Texture2D* CreateD3DTexture3D(D3D11_TEXTURE3D_DESC& desc);

	ID3D11Texture2D* CreateD3DTexture(D3D11_TEXTURE2D_DESC& desc);
	ID3D11Texture2D* CreateD3DTexture(uint width, uint height, uint mipMapLevel, uint arraySize,
								      DXGI_FORMAT format, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag, D3D11_RESOURCE_MISC_FLAG miscFlags);

	//IDirect3DCubeTexture9* CreateD3DCubeTexture(uint size, 
	//	uint mipMapLevel, 
	//	D3DFORMAT format,
	//	DWORD usage,
	//	D3DPOOL pool);

	//IDirect3DTexture9* CreateD3DTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool);
	//IDirect3DVolumeTexture9* CreateD3DVolumeTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool);
	//IDirect3DCubeTexture9* CreateD3DCubeTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool);

	ID3D11Buffer* CreateD3DVertexBuffer(uint bufferLen, D3D11_USAGE usage);
	ID3D11Buffer* CreateD3DIndexBuffer(uint bufferLen, D3D11_USAGE usage);
	//ID3DXEffect* CreateD3DEffectFromData(byte* pData, uint length);

protected:
	IRRenderGroup* DoCreateRenderGroup(RenderOpSort sortFn, uint maxRenderOps); 

	CREffectTemplate* DoLoadEffectTemplate(IByteBuffer* pBuffer);
	CRTexture2D* DoLoadTexture2D(IByteBuffer* pBuffer);
	CRTexture3D* DoLoadTexture3D(IByteBuffer* pData);
	CRTextureCube* DoLoadTextureCube(IByteBuffer* pData);
	void DoSaveTexture2D(IRTexture2D* pTex, IByteBuffer* pData);

	CRTextureCube* DoCreateTextureCube(uint size, 
		uint mipMapLevel, 
		eRTexFormat format,
		eRTexUsage usage);
	CRTexture2D* DoCreateTexture2D(uint width, 
		uint height, 
		uint mipMapLevel, 
		eRTexFormat format,
		eRTexUsage usage);

	CRTexture3D* DoCreateTexture3D(uint width, 
		uint height, 
		uint depth,
		uint mipMapLevel, 
		eRTexFormat format,
		eRTexUsage usage);

	CRVertexBuffer* DoCreateVertexBuffer(uint bufferLen, eRBufferUsage usage);
	CRIndexBuffer* DoCreateIndexBuffer(uint bufferLen, eRBufferUsage usage);
	CRVertexBuffer* DoCreateVertexBufferShared(uint bufferLen, CRVertexBuffer* pSource);
	CRIndexBuffer* DoCreateIndexBufferShared(uint bufferLen, CRIndexBuffer* pSource);

	CRVertexBufferGroup* DoCreateVertexBufferGroup();
	CRDepthBuffer* DoCreateDepthBuffer(uint width,
		uint height,
		eRDepthFormat format,
		uint multiSampleQuality);

	CRTexture2D* DoCreateTexture2DRT(uint width,
		uint height,
		uint mipLevels,
		eRTexFormat format);

	CRTextureCube* DoCreateTextureCubeRT(uint size, eRTexFormat format);

	CRTextureRT* DoCreateBackBufferColor();
	CRDepthBuffer* DoCreateBackBufferDepth();

	void DoOnRestoreDevice() {}
	void DoOnResetDevice() {}
	void DoOnLostDevice() {}

public:
	ID3D11Texture3D* CreateOffscreenTexture3D(D3D11_TEXTURE3D_DESC& desc);
	ID3D11Texture2D* CreateOffscreenTexture2D(D3D11_TEXTURE2D_DESC& desc);
	ID3D11Buffer* CreateOffscreenIB(const D3D11_BUFFER_DESC& desc);
	ID3D11Buffer* CreateOffscreenVB(const D3D11_BUFFER_DESC& desc);
	CRTextureRTD3D11* CreateRenderTargetFromResource(ID3D11Resource* pRes, D3D11_RENDER_TARGET_VIEW_DESC* pDesc);
};

_NAMESPACE_END
