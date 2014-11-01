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
	struct InputLayoutKey
	{
		const static int MAX_NUM_ELEMS = 16;
		uint hashVal;
		size_t vsPtr;
		D3D11_INPUT_ELEMENT_DESC desc[MAX_NUM_ELEMS];
		uint numElems;

		bool operator==(const InputLayoutKey& other) const
		{
			bool isEqual = (vsPtr == other.vsPtr) && (numElems == other.numElems);
			if(isEqual)
			{
				_LOOPi(numElems)
				{
					isEqual &&= (desc[i].AlignedByteOffset == other.desc[i].AlignedByteOffset);
					isEqual &&= (desc[i].Format == other.desc[i].Format);
					isEqual &&= (desc[i].InputSlot == other.desc[i].InputSlot);
					isEqual &&= (desc[i].InputSlotClass == other.desc[i].InputSlotClass);
					isEqual &&= (desc[i].InstanceDataStepRate == other.desc[i].InstanceDataStepRate);
					isEqual &&= (desc[i].SemanticIndex == other.desc[i].SemanticIndex);
					isEqual &&= (strcmp(pDesc[i].SemanticName, other.desc[i].SemanticName) == 0);
				}
			}

			return isEqual;
		}
	};

	struct InputLayoutKeyHash
	{
		inline uint operator() (const InputLayoutKey& key) { return key.hashVal; }
	};

private:
	DenseChainedHashMap<InputLayoutKey, ID3D11InputLayout*, InputLayoutKeyHash> m_InputLayoutMap;

public:
	CRResourceMgrD3D11(CRRenderer* pRenderer)
		: CRResourceMgr(pRenderer)
	{
		CREffectTemplateD3D11::__Initialize();

		m_InputLayoutMap.Initialize(32, 0.8f);
	}

	virtual ~CRResourceMgrD3D11()
	{
		// TODO: Delete effects first
		// ... 

		CREffectTemplateD3D11::__Shutdown();
	}

protected:
	void ComputeInputLayoutHash(InputLayoutKey& dest, ID3D11VertexShader* pVS, D3D11_INPUT_ELEMENT_DESC* pDesc, uint numElems);

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

	ID3D11InputLayout* CreateInputLayout(CREffectTemplateD3D11* pTemplate, D3D11_INPUT_ELEMENT_DESC* pDesc, uint numElems);
	void RemoveInputLayout(ID3D11VertexShader* pKey, D3D11_INPUT_ELEMENT_DESC* pDesc, uint numElems);
};

_NAMESPACE_END
