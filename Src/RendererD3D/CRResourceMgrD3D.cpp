//==============================================================================
//
//		CRResourceMgrD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Friday, April 07, 2006
//
//==============================================================================

#include "RendererD3D.h"


_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
//	D3DXIncludeImpl
//		For parsing #include directives
//------------------------------------------------------------------------------
class D3DXIncludeImpl : public ID3DXInclude
{
private:
	std::map<LPCVOID, IByteBufferPtr> m_Buffers;
	CRRendererD3D* m_pRenderer;
	
public:
	D3DXIncludeImpl(CRRendererD3D* pRenderer)
		: m_pRenderer(pRenderer)
	{
	}
	
	virtual ~D3DXIncludeImpl()
	{
		m_Buffers.clear();
	}
	
public:
	HRESULT __stdcall Open(D3DXINCLUDE_TYPE IncludeType,
						   LPCSTR pFileName,
						   LPCVOID pParentData,
						   LPCVOID *ppData,
						   UINT *pBytes)
	{
		wchar pWFileName[1024];
		ASCIIToUnicode(pFileName, pWFileName);
		wstring fileName = pWFileName;
		
		IByteBuffer* pBuffer = m_pRenderer->GetAppCallback().GetFileData(fileName.c_str());
		if(pBuffer)
		{
			*pBytes = pBuffer->GetDataLength();
			*ppData = pBuffer->GetData();
			
			m_Buffers.insert( std::make_pair<LPCVOID, IByteBufferPtr>(pBuffer->GetData(), pBuffer) );
			
			return S_OK;
		}
		
		return E_FAIL;
	}
	
	HRESULT __stdcall Close(LPCVOID pData)
	{
		m_Buffers.erase(pData);
	
		return S_OK;
	}	
};

ID3DXEffect*
CRResourceMgrD3D::CreateD3DEffectFromData(byte* pData, uint length)
{
	ID3DXEffect* pD3DEffect = NULL;

#ifdef _DEBUG
	DWORD shaderCompileFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
#else
	DWORD shaderCompileFlags = D3DXSHADER_PACKMATRIX_COLUMNMAJOR;// D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	if(pData)
	{	
		// Parse the effect file
		D3DXIncludeImpl d3dxInclude((CRRendererD3D*)m_pRenderer);
		ID3DXBuffer* pErrBuf = NULL;
		if( FAILED( D3DXCreateEffect(((CRRendererD3D*)(m_pRenderer))->GetD3DDevice(),
			pData, 
			length,
			NULL,					// Defines
			&d3dxInclude,			// D3DXInclude
			shaderCompileFlags,		// Compile flags
			NULL,					// Pool
			&pD3DEffect,
			&pErrBuf ) ) )
		{
			// Log error messages
			if(pErrBuf)
			{
				wchar* pErrStr = _NEW wchar[pErrBuf->GetBufferSize()];
				uint copied = ASCIIToUnicode((char*)(pErrBuf->GetBufferPointer()), pErrStr);			
				_DEBUG_ASSERT(copied == pErrBuf->GetBufferSize());

				m_pRenderer->GetAppCallback().Log(pErrStr);

				_DELETE_ARRAY(pErrStr);
				_RELEASE(pErrBuf);
			}

			_RELEASE(pD3DEffect);
			pD3DEffect = NULL;
		}
	}
	
	return pD3DEffect;
}

//------------------------------------------------------------------------------
//	DoLoadEffectTemplate()
//		
//------------------------------------------------------------------------------
CREffectTemplate* 
CRResourceMgrD3D::DoLoadEffectTemplate(IByteBuffer* pBuffer)
{
	CREffectD3DTemplate* pEffectTemplate = NULL;

	if(pBuffer)
	{	
		ID3DXEffect* pD3DEffect = CreateD3DEffectFromData(pBuffer->GetData(), pBuffer->GetDataLength());

		if( pD3DEffect )
		{
			// Create the effect
			pEffectTemplate = _NEW CREffectD3DTemplate(m_pRenderer);
			pEffectTemplate->SetD3DEffect(pD3DEffect);
			
			_RELEASE(pD3DEffect);
		}
	}
		
	return pEffectTemplate;
}

IDirect3DTexture9*
CRResourceMgrD3D::CreateD3DTexture(uint width, 
								   uint height, 
								   uint mipMapLevel, 
								   D3DFORMAT format,
								   DWORD usage,
								   D3DPOOL pool)
{
	IDirect3DTexture9* pD3DTex = NULL;

	if( FAILED( D3DXCreateTexture(
					((CRRendererD3D*)(m_pRenderer))->GetD3DDevice(),
					width,
					height,
					mipMapLevel,
					usage,
					format,
					pool,
					&pD3DTex ) ) )
	{
		// Log the error
		m_pRenderer->GetAppCallback().Log( _W("D3D texture create failed!") );
		_RELEASE(pD3DTex);
		pD3DTex = NULL;
	}
	
	return pD3DTex;
}

IDirect3DVolumeTexture9* CRResourceMgrD3D::CreateD3DVolumeTexture(uint width, 
													uint height,
													uint depth,
													uint mipMapLevel, 
													D3DFORMAT format,
													DWORD usage,
													D3DPOOL pool)
{
	IDirect3DVolumeTexture9* pD3DTex = NULL;

	if( FAILED( D3DXCreateVolumeTexture(
		((CRRendererD3D*)(m_pRenderer))->GetD3DDevice(),
		width,
		height,
		depth,
		mipMapLevel,
		usage,
		format,
		pool,
		&pD3DTex ) ) )
	{
		// Log the error
		m_pRenderer->GetAppCallback().Log( _W("D3D volume texture create failed!") );
		_RELEASE(pD3DTex);
		pD3DTex = NULL;
	}

	return pD3DTex;
}

IDirect3DCubeTexture9* 
CRResourceMgrD3D::CreateD3DCubeTexture(uint size, 
										uint mipMapLevel, 
										D3DFORMAT format,
										DWORD usage,
										D3DPOOL pool)
{
	IDirect3DCubeTexture9* pD3DTex = NULL;

	if( FAILED( D3DXCreateCubeTexture(
			((CRRendererD3D*)(m_pRenderer))->GetD3DDevice(),
			size,
			mipMapLevel,
			usage,
			format,
			pool,
			&pD3DTex ) ) )
	{
		// Log the error
		m_pRenderer->GetAppCallback().Log( _W("D3D cube texture create failed!") );
		_RELEASE(pD3DTex);
		pD3DTex = NULL;
	}

	return pD3DTex;
}
											

IDirect3DCubeTexture9* 
CRResourceMgrD3D::CreateD3DCubeTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool)
{
	IDirect3DCubeTexture9* pD3DTex = NULL;
	if(pData)
	{
		if( FAILED( D3DXCreateCubeTextureFromFileInMemoryEx( 
			((CRRendererD3D*)(m_pRenderer))->GetD3DDevice(),
			pData,
			length,
			D3DX_DEFAULT,		// width and height
			D3DX_FROM_FILE,		// mip levels
			0,					// usage
			D3DFMT_FROM_FILE,	// format
			pool,				// pool
			D3DX_DEFAULT,		// filter
			D3DX_DEFAULT,		// mip filter
			0,					// color key
			pInfo,				// image info
			NULL,				// palette
			&pD3DTex ) ) )
		{
			// Log the error
			m_pRenderer->GetAppCallback().Log( _W("D3D cube texture load failed!") );
			_RELEASE(pD3DTex);
			pD3DTex = NULL;
		}					
	}
	
	return pD3DTex;
}

IDirect3DVolumeTexture9* CRResourceMgrD3D::CreateD3DVolumeTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool)
{
	IDirect3DVolumeTexture9* pD3DTex = NULL;

	if(pData)
	{
		if( FAILED( D3DXCreateVolumeTextureFromFileInMemoryEx( 
			((CRRendererD3D*)(m_pRenderer))->GetD3DDevice(),
			pData,
			length,
			D3DX_DEFAULT,		// width
			D3DX_DEFAULT,		// height
			D3DX_DEFAULT,		// depth
			D3DX_FROM_FILE,		// mip levels
			0,					// usage
			D3DFMT_FROM_FILE,	// format
			pool,				// pool
			D3DX_DEFAULT,		// filter
			D3DX_DEFAULT,		// mip filter
			0,					// color key
			pInfo,				// image info
			NULL,				// palette
			&pD3DTex ) ) )
		{
			// Log the error
			m_pRenderer->GetAppCallback().Log( _W("D3D volume texture load failed!") );
			_RELEASE(pD3DTex);
			pD3DTex = NULL;
		}					
	}

	return pD3DTex;
}

IDirect3DTexture9*
CRResourceMgrD3D::CreateD3DTextureFromData(byte* pData, uint length, D3DXIMAGE_INFO* pInfo, D3DPOOL pool)
{
	IDirect3DTexture9* pD3DTex = NULL;
		
	if(pData)
	{
		if( FAILED( D3DXCreateTextureFromFileInMemoryEx( 
						((CRRendererD3D*)(m_pRenderer))->GetD3DDevice(),
						pData,
						length,
						D3DX_DEFAULT,		// width
						D3DX_DEFAULT,		// height
						D3DX_FROM_FILE,		// mip levels
						0,					// usage
						D3DFMT_FROM_FILE,	// format
						pool,				// pool
						D3DX_DEFAULT,		// filter
						D3DX_DEFAULT,		// mip filter
						0,					// color key
						pInfo,				// image info
						NULL,				// palette
						&pD3DTex ) ) )
		{
			// Log the error
			m_pRenderer->GetAppCallback().Log( _W("D3D texture load failed!") );
			_RELEASE(pD3DTex);
			pD3DTex = NULL;
		}					
	}
	
	return pD3DTex;						
}

IRRenderGroup* 
CRResourceMgrD3D::DoCreateRenderGroup(RenderOpSort sortFn, uint maxRenderOps)
{
	return _NEW CRRenderGroupD3D(m_pRenderer, sortFn, maxRenderOps);
}

// TODO: Remove this when manual parsing utility of DDS/TGA format is implemented
void CRResourceMgrD3D::DoSaveTexture2D(IRTexture2D* pTex, IByteBuffer* pData)
{
	ID3DXBuffer* pD3DBuffer = NULL;
	HRESULT hr = D3DXSaveTextureToFileInMemory(&pD3DBuffer, D3DXIFF_DDS, ((CRTexture2DD3D*) pTex)->GetD3DOffscreen(), NULL);
	
	_DEBUG_ASSERT(pD3DBuffer);
	_DEBUG_ASSERT(pD3DBuffer->GetBufferSize() > 0);

	pData->SetDataLength(0);
	pData->ResizeBuffer(pD3DBuffer->GetBufferSize());
	memcpy(pData->GetData(), pD3DBuffer->GetBufferPointer(), pD3DBuffer->GetBufferSize());
	pData->SetDataLength(pD3DBuffer->GetBufferSize());
}

//------------------------------------------------------------------------------
//	DoLoadTexture2D()
//		
//------------------------------------------------------------------------------
CRTexture2D* 
CRResourceMgrD3D::DoLoadTexture2D(IByteBuffer* pBuffer)
{
	CRTexture2DD3D* pTex = NULL;
	
	D3DXIMAGE_INFO info;
	IDirect3DTexture9* pD3DTex = CreateD3DTextureFromData(pBuffer->GetData(),pBuffer->GetDataLength(), &info, D3DPOOL_DEFAULT);
	IDirect3DTexture9* pOffScreen = CreateD3DTextureFromData(pBuffer->GetData(),pBuffer->GetDataLength(), &info, D3DPOOL_SYSTEMMEM);
		
	if(pD3DTex && pOffScreen)
	{
		pTex = _NEW CRTexture2DD3D(m_pRenderer);
		pTex->SetD3DTexture(pD3DTex, pOffScreen);
		pTex->SetFormat(TexFormatFromD3D(info.Format));
		pTex->SetUsage(TEXU_DEFAULT);
	}

	_RELEASE(pD3DTex);
	_RELEASE(pOffScreen);	
	
	return pTex;
}

CRTexture3D* CRResourceMgrD3D::DoLoadTexture3D(IByteBuffer* pData)
{
	CRTexture3DD3D* pTex = NULL;

	D3DXIMAGE_INFO info;
	IDirect3DVolumeTexture9* pD3DTex = CreateD3DVolumeTextureFromData(pData->GetData(), pData->GetDataLength(), &info, D3DPOOL_DEFAULT);
	IDirect3DVolumeTexture9* pOffScreen = CreateD3DVolumeTextureFromData(pData->GetData(), pData->GetDataLength(), &info, D3DPOOL_SYSTEMMEM);

	if(pD3DTex && pOffScreen)
	{
		pTex = _NEW CRTexture3DD3D(m_pRenderer);
		pTex->SetD3DTexture(pD3DTex, pOffScreen);
		pTex->SetFormat(TexFormatFromD3D(info.Format));
		pTex->SetUsage(TEXU_DEFAULT);
	}

	_RELEASE(pD3DTex);
	_RELEASE(pOffScreen);	

	return pTex;
}

CRTextureCube* 
CRResourceMgrD3D::DoLoadTextureCube(IByteBuffer* pData)
{
	CRTextureCubeD3D* pTex = NULL;

	D3DXIMAGE_INFO info;
	IDirect3DCubeTexture9* pD3DTex = CreateD3DCubeTextureFromData(pData->GetData(), pData->GetDataLength(), &info, D3DPOOL_DEFAULT);
	IDirect3DCubeTexture9* pOffScreen = CreateD3DCubeTextureFromData(pData->GetData(), pData->GetDataLength(), &info, D3DPOOL_SYSTEMMEM);

	if(pD3DTex && pOffScreen)
	{
		pTex = _NEW CRTextureCubeD3D(m_pRenderer);
		pTex->SetD3DTexture(pD3DTex, pOffScreen);
		pTex->SetFormat(TexFormatFromD3D(info.Format));
		pTex->SetUsage(TEXU_DEFAULT);
	}

	_RELEASE(pD3DTex);
	_RELEASE(pOffScreen);

	return pTex;
}

//------------------------------------------------------------------------------
//	DoCreateTextureCube()
//		
//------------------------------------------------------------------------------
CRTextureCube* 
CRResourceMgrD3D::DoCreateTextureCube(uint size, 
									 uint mipMapLevel, 
									 eRTexFormat format,
									 eRTexUsage usage)
{
	IDirect3DCubeTexture9* pD3DTex = CreateD3DCubeTexture(size, mipMapLevel, 
		TexFormatToD3D(format), TexUsageToD3D(usage), D3DPOOL_DEFAULT);							

	IDirect3DCubeTexture9* pOffScreen = CreateD3DCubeTexture(size, mipMapLevel, 
		TexFormatToD3D(format), TexUsageToD3D(usage), D3DPOOL_SYSTEMMEM);		

	CRTextureCubeD3D* pTex = NULL;

	if(pD3DTex && pOffScreen)
	{
		pTex = _NEW CRTextureCubeD3D(m_pRenderer);
		pTex->SetD3DTexture(pD3DTex, pOffScreen);
		pTex->SetFormat(format);
		pTex->SetUsage(usage);
	}

	_RELEASE(pD3DTex);
	_RELEASE(pOffScreen);

	return pTex;
}

CRTextureCube* CRResourceMgrD3D::DoCreateTextureCubeRT(uint size, eRTexFormat format)
{
	IDirect3DCubeTexture9* pD3DTex = CreateD3DCubeTexture(size, 1, 
		TexFormatToD3D(format), D3DUSAGE_RENDERTARGET, D3DPOOL_DEFAULT);							

	CRTextureCubeD3D* pTex = NULL;

	if(pD3DTex)
	{
		pTex = _NEW CRTextureCubeD3D(m_pRenderer);
		pTex->SetD3DTextureRT(pD3DTex);
		pTex->SetFormat(format);
		pTex->SetUsage(TEXU_DEFAULT);
	}

	_RELEASE(pD3DTex);
	
	return pTex;
}


CRTexture3D* CRResourceMgrD3D::DoCreateTexture3D(uint width, 
												uint height, 
												uint depth,
												uint mipMapLevel, 
												eRTexFormat format,
												eRTexUsage usage)
{
	IDirect3DVolumeTexture9* pD3DTex = CreateD3DVolumeTexture(width, height, depth, mipMapLevel, 
		TexFormatToD3D(format), TexUsageToD3D(usage), D3DPOOL_DEFAULT);

	IDirect3DVolumeTexture9* pOffScreen = CreateD3DVolumeTexture(width, height, depth, mipMapLevel, 
		TexFormatToD3D(format), TexUsageToD3D(usage), D3DPOOL_SYSTEMMEM);

	CRTexture3DD3D* pTex = NULL;

	if(pD3DTex && pOffScreen)
	{
		pTex = _NEW CRTexture3DD3D(m_pRenderer);
		pTex->SetD3DTexture(pD3DTex, pOffScreen);
		pTex->SetFormat(format);
		pTex->SetUsage(usage);
	}

	_RELEASE(pD3DTex);
	_RELEASE(pOffScreen);

	return pTex;
}

//------------------------------------------------------------------------------
//	DoCreateTexture2D()
//		
//------------------------------------------------------------------------------
CRTexture2D* 
CRResourceMgrD3D::DoCreateTexture2D(uint width, 
								   uint height, 
								   uint mipMapLevel, 
								   eRTexFormat format,
								   eRTexUsage usage)
{
	IDirect3DTexture9* pD3DTex = CreateD3DTexture(width, height, mipMapLevel, 
												TexFormatToD3D(format), TexUsageToD3D(usage), D3DPOOL_DEFAULT);
												  
	IDirect3DTexture9* pOffScreen = CreateD3DTexture(width, height, mipMapLevel, 
												TexFormatToD3D(format), TexUsageToD3D(usage), D3DPOOL_SYSTEMMEM);

	CRTexture2DD3D* pTex = NULL;
	
	if(pD3DTex && pOffScreen)
	{
		pTex = _NEW CRTexture2DD3D(m_pRenderer);
		pTex->SetD3DTexture(pD3DTex, pOffScreen);
		pTex->SetFormat(format);
		pTex->SetUsage(usage);
	}
	
	_RELEASE(pD3DTex);
	_RELEASE(pOffScreen);

	return pTex;
}

boolean
CRResourceMgrD3D::ResetTexture2D(CRTexture2DD3D* pTex)
{
	IDirect3DTexture9* pD3DTex = NULL;
	
	DWORD usage = TexUsageToD3D(pTex->GetUsage());
	if(pTex->GetTextureRT(0))
		usage = D3DUSAGE_RENDERTARGET;

	pD3DTex = CreateD3DTexture(pTex->GetWidth(0), pTex->GetHeight(0), pTex->GetMipMapLevels(), 
									TexFormatToD3D(pTex->GetFormat()), usage, D3DPOOL_DEFAULT);							
	
	if(pD3DTex)
	{
		pTex->SetD3DTexture(pD3DTex, NULL);
	}			

	_RELEASE(pD3DTex);
	
	return (pD3DTex != NULL);
}

boolean CRResourceMgrD3D::ResetTexture3D(CRTexture3DD3D* pTex)
{
	IDirect3DVolumeTexture9* pD3DTex = NULL;

	pD3DTex = CreateD3DVolumeTexture(pTex->GetWidth(0), pTex->GetHeight(0), pTex->GetDepth(0), pTex->GetMipMapLevels(), 
		TexFormatToD3D(pTex->GetFormat()), TexUsageToD3D(pTex->GetUsage()), D3DPOOL_DEFAULT);							

	if(pD3DTex)
	{
		pTex->SetD3DTexture(pD3DTex, NULL);
	}			

	_RELEASE(pD3DTex);

	return (pD3DTex != NULL);
}


boolean
CRResourceMgrD3D::ResetTextureCube(CRTextureCubeD3D* pCube)
{
	IDirect3DCubeTexture9* pD3DTex = NULL;

	DWORD usage = TexUsageToD3D(pCube->GetUsage());
	if(pCube->GetTextureRT(0, CF_POS_Z))
		usage = D3DUSAGE_RENDERTARGET;

	pD3DTex = CreateD3DCubeTexture(pCube->GetSize(0), pCube->GetMipMapLevels(), 
		TexFormatToD3D(pCube->GetFormat()), usage, D3DPOOL_DEFAULT);							

	if(pD3DTex)
	{
		pCube->SetD3DTexture(pD3DTex, NULL);
	}			

	_RELEASE(pD3DTex);

	return (pD3DTex != NULL);
}


boolean
CRResourceMgrD3D::ResetEffectTemplate(CREffectD3DTemplate* pEffectTemplate)
{
	ID3DXEffect* pD3DEffect = NULL;

	//pEffectTemplate->
	//if(pEffectTemplate->GetResourceName())
	//{
	//	IByteBufferPtr pBuffer = m_pRenderer->IAppCallback().GetFileData(pEffectTemplate->GetResourceName());
	//	pD3DEffect = CreateD3DEffectFromData(pBuffer->GetData(), pBuffer->GetDataLength());	

	//	if(pD3DEffect)
	//	{
	//		pEffectTemplate->SetD3DEffect(pD3DEffect);
	//	}
	//}

	//_RELEASE(pD3DEffect);

	return (pD3DEffect != NULL);
}

IDirect3DVertexBuffer9*
CRResourceMgrD3D::CreateD3DVertexBuffer(uint bufferLen, DWORD usage, D3DPOOL pool)
{
	_DEBUG_ASSERT(m_pRenderer);
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);
	
	IDirect3DVertexBuffer9* pD3DVB = NULL;	
	if( FAILED(pDevice->CreateVertexBuffer(bufferLen, usage, 0, pool, &pD3DVB, NULL)) )
	{
		// Log the error
		m_pRenderer->GetAppCallback().Log( _W("D3D vertex buffer create failed!") );
		_RELEASE(pD3DVB);
		pD3DVB = NULL;
	}
	
	return pD3DVB;
}

IDirect3DIndexBuffer9* 
CRResourceMgrD3D::CreateD3DIndexBuffer(uint bufferLen, DWORD usage, D3DPOOL pool)
{
	_DEBUG_ASSERT(m_pRenderer);
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);

	IDirect3DIndexBuffer9* pD3DIB = NULL;	
	if( FAILED(pDevice->CreateIndexBuffer(bufferLen, usage, D3DFMT_INDEX16, pool, &pD3DIB, NULL)) )
	{
		// Log the error
		m_pRenderer->GetAppCallback().Log( _W("D3D index buffer create failed!") );
		_RELEASE(pD3DIB);
		pD3DIB = NULL;
	}

	return pD3DIB;
}

CRDepthBuffer* 
CRResourceMgrD3D::DoCreateDepthBuffer(uint width, 
									uint height, 
									eRDepthFormat format,
									uint multiSampleQuality)
{
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();

	CRDepthBufferD3D* pToReturn = NULL;	
	IDirect3DSurface9* pDepth = NULL;
	HRESULT hr = pDevice->CreateDepthStencilSurface(width, 
													height, 
													DepthFormatToD3D(format), 
													(D3DMULTISAMPLE_TYPE) multiSampleQuality,
													0,
													TRUE,
													&pDepth,
													NULL);
													
	_DEBUG_ASSERT(SUCCEEDED(hr));

	if(pDepth)
	{
		pToReturn = _NEW CRDepthBufferD3D();
		pToReturn->SetDepthStencilSurface(pDepth, false);
		pToReturn->SetSize(width, height);
		pToReturn->SetMultiSampleQuality(multiSampleQuality);
		pToReturn->SetDepthFormat(format);
	}
	
	_RELEASE(pDepth);
	
	return pToReturn;	
}

CRTexture2D* 
CRResourceMgrD3D::DoCreateTexture2DRT(uint width,
								uint height,
								uint mipLevels,
								eRTexFormat format)
{
	IDirect3DTexture9* pD3DTex = CreateD3DTexture(width, height, mipLevels, 
		TexFormatToD3D(format), D3DUSAGE_RENDERTARGET, D3DPOOL_DEFAULT);
		
	CRTexture2DD3D* pRT = NULL;

	if(pD3DTex)
	{
		pRT = _NEW CRTexture2DD3D(m_pRenderer);
		pRT->SetD3DTextureRT(pD3DTex);
		pRT->SetFormat(format);
		pRT->SetUsage(TEXU_DEFAULT);
	}

	_RELEASE(pD3DTex);

	return pRT;
}
								
CRVertexBufferGroup* 
CRResourceMgrD3D::DoCreateVertexBufferGroup()
{
	return _NEW CRVertexBufferGroupD3D(m_pRenderer);
}

//------------------------------------------------------------------------------
//	DoCreateVertexBuffer()
//		
//------------------------------------------------------------------------------
CRVertexBuffer* 
CRResourceMgrD3D::DoCreateVertexBuffer(uint bufferLen, eRBufferUsage usage)
{
	CRVertexBufferD3D* pVB = NULL;

	IDirect3DVertexBuffer9* pD3DVB = CreateD3DVertexBuffer(bufferLen, BufUsageToD3D(usage), D3DPOOL_DEFAULT);
	IByteBuffer* pOffScreen = _NEW CByteBuffer(bufferLen);
	//IDirect3DVertexBuffer9* pOffScreen = CreateD3DVertexBuffer(bufferLen, 0, D3DPOOL_SYSTEMMEM);
	
	if(pD3DVB && pOffScreen)
	{
		pVB = _NEW CRVertexBufferD3D(m_pRenderer);
		pVB->SetD3DVB(pD3DVB, pOffScreen);
	}
	
	_RELEASE(pD3DVB);
	
	return pVB;
}

CRVertexBuffer* CRResourceMgrD3D::DoCreateVertexBufferShared(uint bufferLen, CRVertexBuffer* pSource)
{
	CRVertexBufferD3D* pVB = NULL;

	//IDirect3DVertexBuffer9* pD3DVB = ((CRVertexBufferD3D*) pSource)->GetD3DVB();
	//IByteBuffer* pOffScreen = ((CRVertexBufferD3D*) pSource)->GetOffscreenVB();

//	_DEBUG_ASSERT(pOffScreen->GetBufferSize() >= bufferLen);
	
	//if(pD3DVB && pOffScreen)
	{
		pVB = _NEW CRVertexBufferD3D(m_pRenderer);
		pVB->SetD3DSharedVB((CRVertexBufferD3D*) pSource);
		//pVB->SetD3DVB(pD3DVB, pOffScreen);
	}

	return pVB;
}

//------------------------------------------------------------------------------
//	DoCreateIndexBuffer()
//		
//------------------------------------------------------------------------------
CRIndexBuffer* 
CRResourceMgrD3D::DoCreateIndexBuffer(uint bufferLen, eRBufferUsage usage)
{
	CRIndexBufferD3D* pIB = NULL;

	IDirect3DIndexBuffer9* pD3DIB = CreateD3DIndexBuffer(bufferLen, BufUsageToD3D(usage), D3DPOOL_DEFAULT);
	IDirect3DIndexBuffer9* pOffScreen = CreateD3DIndexBuffer(bufferLen, 0, D3DPOOL_SYSTEMMEM);
	
	if(pD3DIB && pOffScreen)
	{
		pIB = _NEW CRIndexBufferD3D(m_pRenderer);
		pIB->SetD3DIB(pD3DIB, pOffScreen);		
	}
	
	_RELEASE(pD3DIB);
	_RELEASE(pOffScreen);
	
	return pIB;
}

CRIndexBuffer* CRResourceMgrD3D::DoCreateIndexBufferShared(uint bufferLen, CRIndexBuffer* pSource)
{
	CRIndexBufferD3D* pIB = NULL;

	//IDirect3DIndexBuffer9* pD3DIB = ((CRIndexBufferD3D*) pSource)->GetD3DIB();
	//IDirect3DIndexBuffer9* pOffScreen = ((CRIndexBufferD3D*) pSource)->GetOffscreenIB();
	
	//if(pD3DIB && pOffScreen)
	{
		pIB = _NEW CRIndexBufferD3D(m_pRenderer);
		pIB->SetD3DSharedIB((CRIndexBufferD3D*) pSource);
		//pIB->SetD3DIB(pD3DIB, pOffScreen);		
	}

	return pIB;
}

boolean
CRResourceMgrD3D::ResetVertexBuffer(CRVertexBufferD3D* pVB)
{	
	_DEBUG_ASSERT(pVB);
	
	eRBufferUsage usage = pVB->GetUsage();
		
	IDirect3DVertexBuffer9* pD3DVB = CreateD3DVertexBuffer(pVB->GetSize() * pVB->GetVertexSize(), 
														   BufUsageToD3D(usage),
														   D3DPOOL_DEFAULT);														   
	if(pD3DVB)
	{
		pVB->SetD3DVB(pD3DVB, NULL);
	}
	
	_RELEASE(pD3DVB);

	return (pD3DVB != NULL);
}

boolean
CRResourceMgrD3D::ResetIndexBuffer(CRIndexBufferD3D* pIB)
{
	_DEBUG_ASSERT(pIB);
	
	eRBufferUsage usage = pIB->GetUsage();
	IDirect3DIndexBuffer9* pD3DIB = CreateD3DIndexBuffer(pIB->GetSize() * sizeof(ushort), 
														BufUsageToD3D(usage),
														D3DPOOL_DEFAULT);			

	if(pD3DIB)
	{
		pIB->SetD3DIB(pD3DIB, NULL);
	}
	
	_RELEASE(pD3DIB);

	return (pD3DIB != NULL);
}

//boolean 
//CRResourceMgrD3D::ResetTextureRT(CRTextureRTD3D* pRT)
//{
//	IDirect3DTexture9* pD3DTex = NULL;
//
//	pD3DTex = CreateD3DTexture(pRT->GetWidth(), pRT->GetHeight(), 1, 
//		TexFormatToD3D(pRT->GetFormat()), D3DUSAGE_RENDERTARGET, D3DPOOL_DEFAULT);							
//
//	if(pD3DTex)
//	{
//		pRT->SetD3DTexture(pD3DTex);
//	}			
//
//	_RELEASE(pD3DTex);
//
//	return (pD3DTex != NULL);
//}

boolean CRResourceMgrD3D::ResetBackBufferColor(CRTextureRTD3D* pRT)
{
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();

	IDirect3DSurface9* pBB = NULL;
	HRESULT hr = pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB);
	_DEBUG_ASSERT(SUCCEEDED(hr));

	pRT->SetD3DSurfaceOnly(pBB);
	_RELEASE(pBB);

	return pBB != NULL;
}

boolean 
CRResourceMgrD3D::ResetDepthBuffer(CRDepthBufferD3D* pDepth)
{
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();

	IDirect3DSurface9* pDepthSurface = NULL;
	HRESULT hr = pDevice->CreateDepthStencilSurface(
		pDepth->GetWidth(), 
		pDepth->GetHeight(), 
		DepthFormatToD3D(pDepth->GetDepthFormat()), 
		(D3DMULTISAMPLE_TYPE) pDepth->GetMultiSampleQuality(),
		0,
		TRUE,
		&pDepthSurface,
		NULL);

	_DEBUG_ASSERT(SUCCEEDED(hr));

	if(pDepthSurface)
	{
		pDepth->SetDepthStencilSurface(pDepthSurface, FALSE);
	}
	
	_RELEASE(pDepthSurface);
	
	return (pDepthSurface != NULL);
}

boolean CRResourceMgrD3D::ResetBackBufferDepth(CRDepthBufferD3D* pDepth)
{
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();

	IDirect3DSurface9* pDepthSurface = NULL;
	HRESULT hr = pDevice->GetDepthStencilSurface(&pDepthSurface);
	_DEBUG_ASSERT(SUCCEEDED(hr) && pDepthSurface);

	pDepth->SetDepthStencilSurface(pDepthSurface, TRUE);

	_RELEASE(pDepthSurface);

	return (pDepthSurface != NULL);
}

CRTextureRTD3D* CRResourceMgrD3D::CreateRenderTargetFromSurface(IDirect3DSurface9* pSurf)
{
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();

	CRTextureRTD3D* pRT = _NEW CRTextureRTD3D(m_pRenderer);

	IDirect3DSurface9* pBB = pSurf;
	
	D3DSURFACE_DESC desc;
	pBB->GetDesc(&desc);

	pRT->SetD3DSurfaceOnly(pBB);
	pRT->SetFormat(TexFormatFromD3D(desc.Format));
	pRT->SetUsage(TEXU_DEFAULT);
	
	return pRT;
}

CRTextureRT* CRResourceMgrD3D::DoCreateBackBufferColor()
{
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();

	CRTextureRTD3D* pRT = _NEW CRTextureRTD3D(m_pRenderer);

	IDirect3DSurface9* pBB = NULL;
	HRESULT hr = pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB);
	_DEBUG_ASSERT(SUCCEEDED(hr));

	D3DSURFACE_DESC desc;
	pBB->GetDesc(&desc);

	pRT->SetD3DSurfaceOnly(pBB);
	pRT->SetFormat(TexFormatFromD3D(desc.Format));
	pRT->SetUsage(TEXU_DEFAULT);
	_RELEASE(pBB);

	return pRT;
}

CRDepthBuffer* CRResourceMgrD3D::DoCreateBackBufferDepth()
{
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();

	CRDepthBufferD3D* pDB = _NEW CRDepthBufferD3D();
	
	IDirect3DSurface9* pDepthSurface = NULL;
	HRESULT hr = pDevice->GetDepthStencilSurface(&pDepthSurface);
	_DEBUG_ASSERT(SUCCEEDED(hr) && pDepthSurface);

	D3DSURFACE_DESC desc;
	pDepthSurface->GetDesc(&desc);
	
	pDB->SetDepthStencilSurface(pDepthSurface, true);
	pDB->SetSize(desc.Width, desc.Height);
	pDB->SetMultiSampleQuality(MultiSampleFromD3D(desc.MultiSampleType));
	pDB->SetDepthFormat(DepthFormatFromD3D(desc.Format));

	_RELEASE(pDepthSurface);

	return pDB;
}

_NAMESPACE_END