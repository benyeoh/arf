//==============================================================================
//
//		CRResourceMgrD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, March 25, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRResourceMgrD3D : public CRResourceMgr
{
private:

public:
	CRResourceMgrD3D(CRRenderer* pRenderer)
		: CRResourceMgr(pRenderer)
	{
	}
	
	virtual ~CRResourceMgrD3D()
	{
	}
	
protected:
	
	IDirect3DVolumeTexture9* CreateD3DVolumeTexture(uint width, 
													uint height,
													uint depth,
													uint mipMapLevel, 
													D3DFORMAT format,
													DWORD usage,
													D3DPOOL pool);

	IDirect3DTexture9* CreateD3DTexture(uint width, 
										uint height, 
										uint mipMapLevel, 
										D3DFORMAT format,
										DWORD usage,
										D3DPOOL pool);
										
	IDirect3DCubeTexture9* CreateD3DCubeTexture(uint size, 
										uint mipMapLevel, 
										D3DFORMAT format,
										DWORD usage,
										D3DPOOL pool);
										
	IDirect3DTexture9* CreateD3DTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool);
	IDirect3DVolumeTexture9* CreateD3DVolumeTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool);
	IDirect3DCubeTexture9* CreateD3DCubeTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool);
	IDirect3DVertexBuffer9* CreateD3DVertexBuffer(uint bufferLen, DWORD usage, D3DPOOL pool);
	IDirect3DIndexBuffer9* CreateD3DIndexBuffer(uint bufferLen, DWORD usage, D3DPOOL pool);
	ID3DXEffect* CreateD3DEffectFromData(byte* pData, uint length);
	
	
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
	boolean ResetBackBufferDepth(CRDepthBufferD3D* pDepth);
	boolean ResetBackBufferColor(CRTextureRTD3D* pRT);

	boolean ResetTexture3D(CRTexture3DD3D* pTex);
	boolean ResetTexture2D(CRTexture2DD3D* pTex);
	boolean ResetTextureCube(CRTextureCubeD3D* pCube);
	boolean ResetIndexBuffer(CRIndexBufferD3D* pIB);
	boolean ResetVertexBuffer(CRVertexBufferD3D* pVB);
	boolean ResetEffectTemplate(CREffectD3DTemplate* pEffectTemplate);
//	boolean ResetTextureRT(CRTextureRTD3D* pRT);
	boolean ResetDepthBuffer(CRDepthBufferD3D* pDepth);

	CRTextureRTD3D* CreateRenderTargetFromSurface(IDirect3DSurface9* pSurf);
};

_NAMESPACE_END
