//==============================================================================
//
//		RD3DEffectStateManager.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Thursday, January 25, 2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

//struct RD3DVertexShaderOp;
//{
//	LPDIRECT3DVERTEXSHADER9 pShader;
//};
//
//struct RD3DPixelShaderOp;
//{
//	LPDIRECT3DPIXELSHADER9 pShader;
//};
//
//enum eRD3DOpType
//{
//	RD3DOP_RENDER_STATE,
//	RD3DOP_TEXTURE,
//	RD3DOP_TEXTURE_STAGE,
//	RD3DOP_SAMPLER_STATE,
//	RD3DOP_VERTEX_SHADER,
//	RD3DOP_PIXEL_SHADER,
//};

//struct RD3DStateChangeOp
//{
//	union
//	{
//		RD3DRenderStateOp renderStateOp;
//		RD3DTextureOp textureOp;
//		RD3DTextureStageOp textureStageOp;
//		RD3DSamplerStateOp samplerStateOp;
//		RD3DVertexShaderOp vertexShaderOp;
//		RD3DPixelShaderOp pixelShaderOp;
//	};
//	
//	eRD3DOpType type;
//};

struct RD3DRenderStateOp
{
	D3DRENDERSTATETYPE state;
	DWORD value;

	RD3DRenderStateOp()
		: state((D3DRENDERSTATETYPE)0xDEADBEEF)
		, value(0xDEADBEEF)
	{
	}
};


const static uint MAX_STATE_CHANGE_STACK = 5;
const static uint MAX_STATE_CHANGES = 30;
const static uint MAX_TEXTURES = 512;
const static uint MAX_RENDER_STATES = 512;
const static uint MAX_TEXTURE_STAGE_STATES = 32;
const static uint MAX_SAMPLER_STAGE_STATES = 16;

struct RD3DStateStackLayer
{
	RD3DRenderStateOp stateChanges[MAX_STATE_CHANGES];
	uint size;
	
	RD3DStateStackLayer()
		: size(0)
	{
	}
	
	~RD3DStateStackLayer()
	{
	}
};


class RD3DEffectStateManager : public ID3DXEffectStateManager
{
private:
	DWORD m_RenderStates[MAX_RENDER_STATES];
	LPDIRECT3DBASETEXTURE9 m_pTextures[MAX_TEXTURES];	
	DWORD m_TextureStageState[MAX_TEXTURES][MAX_TEXTURE_STAGE_STATES];
	DWORD m_SamplerStageState[MAX_TEXTURES][MAX_SAMPLER_STAGE_STATES];
	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;
	LPDIRECT3DPIXELSHADER9 m_pPixelShader;
	
	RD3DStateStackLayer m_StateChangeStack[MAX_STATE_CHANGE_STACK];
	int m_CurStackIndex;

	IDirect3DDevice9* m_pDevice;
	
	LONG m_RefCount;
	
public:
	RD3DEffectStateManager()
		: m_RefCount(0)
	{
	}

	virtual ~RD3DEffectStateManager()
	{
	}
		
private:
	HRESULT STDMETHODCALLTYPE DoSetRenderState(THIS_ D3DRENDERSTATETYPE State, DWORD Value);
	//HRESULT DoSetTexture(THIS_ DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture) ;
	//HRESULT DoSetTextureStageState(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) ;
//	HRESULT DoSetSamplerState(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) ;

public:
	void Initialize(IDirect3DDevice9* pDevice);
	
	void PushStateStack();
	void PopStateStack();
		
	// The user must correctly implement QueryInterface, AddRef, and Release.

	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID iid, LPVOID *ppv) ;
	ULONG STDMETHODCALLTYPE AddRef(THIS) ;
	ULONG STDMETHODCALLTYPE Release(THIS) ;

	// The following methods are called by the Effect when it wants to make 
	// the corresponding device call.  Note that:
	// 1. Users manage the state and are therefore responsible for making the 
	//    the corresponding device calls themselves inside their callbacks.  
	// 2. Effects pay attention to the return values of the callbacks, and so 
	//    users must pay attention to what they return in their callbacks.

	HRESULT STDMETHODCALLTYPE SetTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix) ;
	HRESULT STDMETHODCALLTYPE SetMaterial(THIS_ CONST D3DMATERIAL9 *pMaterial) ;
	HRESULT STDMETHODCALLTYPE SetLight(THIS_ DWORD Index, CONST D3DLIGHT9 *pLight) ;
	HRESULT STDMETHODCALLTYPE LightEnable(THIS_ DWORD Index, BOOL Enable) ;
	HRESULT STDMETHODCALLTYPE SetRenderState(THIS_ D3DRENDERSTATETYPE State, DWORD Value) ;
	HRESULT STDMETHODCALLTYPE SetTexture(THIS_ DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture) ;
	HRESULT STDMETHODCALLTYPE SetTextureStageState(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) ;
	HRESULT STDMETHODCALLTYPE SetSamplerState(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) ;
	HRESULT STDMETHODCALLTYPE SetNPatchMode(THIS_ FLOAT NumSegments) ;
	HRESULT STDMETHODCALLTYPE SetFVF(THIS_ DWORD FVF) ;
	HRESULT STDMETHODCALLTYPE SetVertexShader(THIS_ LPDIRECT3DVERTEXSHADER9 pShader) ;
	HRESULT STDMETHODCALLTYPE SetVertexShaderConstantF(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount) ;
	HRESULT STDMETHODCALLTYPE SetVertexShaderConstantI(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount) ;
	HRESULT STDMETHODCALLTYPE SetVertexShaderConstantB(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount) ;
	HRESULT STDMETHODCALLTYPE SetPixelShader(THIS_ LPDIRECT3DPIXELSHADER9 pShader) ;
	HRESULT STDMETHODCALLTYPE SetPixelShaderConstantF(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount) ;
	HRESULT STDMETHODCALLTYPE SetPixelShaderConstantI(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount) ;
	HRESULT STDMETHODCALLTYPE SetPixelShaderConstantB(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount) ;
};


_DECLARE_SMARTPTR( RD3DEffectStateManager )

_NAMESPACE_END
