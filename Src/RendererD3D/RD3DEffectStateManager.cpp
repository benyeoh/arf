//==============================================================================
//
//		RD3DEffectStateManager.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Thursday, January 25, 2007
//
//==============================================================================

#include "RendererD3D.h"

_NAMESPACE_BEGIN

HRESULT 
RD3DEffectStateManager::DoSetRenderState(THIS_ D3DRENDERSTATETYPE State, DWORD Value)
{
	m_RenderStates[State] = Value;
	return m_pDevice->SetRenderState(State, Value);
}

void 
RD3DEffectStateManager::Initialize(IDirect3DDevice9* pDevice)
{
	_DEBUG_ASSERT(pDevice);
	
	m_CurStackIndex = -1;
	m_pPixelShader = NULL;
	m_pVertexShader = NULL;
	m_pDevice = NULL;

	_LOOPi(MAX_TEXTURES)
	{	 
		m_pTextures[i] = NULL;

		_LOOPj(MAX_TEXTURE_STAGE_STATES)
		{
			m_TextureStageState[i][j] = 0xDEADBEEF;
		}

		_LOOPj(MAX_SAMPLER_STAGE_STATES)
		{
			m_SamplerStageState[i][j] = 0xDEADBEEF;
		}
	}

	m_pDevice = pDevice;
	
	// Set default render states
	
	// Turn off fixed function lighting
	DoSetRenderState(D3DRS_LIGHTING, FALSE);
	
	// Alpha blend
	DoSetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	DoSetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	DoSetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	DoSetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	// Alpha test
	DoSetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	DoSetRenderState(D3DRS_ALPHAREF, 0);
	DoSetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	
	// Z buffer ops
	DoSetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	DoSetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	DoSetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	
	// Color writes
	DoSetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
	DoSetRenderState(D3DRS_COLORWRITEENABLE1, 0x0000000F);
	DoSetRenderState(D3DRS_COLORWRITEENABLE2, 0x0000000F);
	DoSetRenderState(D3DRS_COLORWRITEENABLE3, 0x0000000F);

	// Stencil ops
	DoSetRenderState(D3DRS_STENCILENABLE, FALSE);
	DoSetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	DoSetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	DoSetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	DoSetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	DoSetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	DoSetRenderState(D3DRS_STENCILREF, 0);
	DoSetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
	DoSetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		
	// Fill mode
	DoSetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		
	// Cull mode
	DoSetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Fog
	DoSetRenderState(D3DRS_FOGENABLE, FALSE);
	DoSetRenderState(D3DRS_FOGCOLOR, 0xFFFFFFFF);
	DoSetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
	DoSetRenderState(D3DRS_FOGSTART, 0);
	float val = 1.0f;
	DoSetRenderState(D3DRS_FOGEND, *((DWORD*)&val));
	DoSetRenderState(D3DRS_FOGDENSITY, *((DWORD*)&val));
	DoSetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
	DoSetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	
	DoSetRenderState(D3DRS_WRAP0, 0);
	DoSetRenderState(D3DRS_WRAP1, 0);
	DoSetRenderState(D3DRS_WRAP2, 0);
	DoSetRenderState(D3DRS_WRAP3, 0);
	DoSetRenderState(D3DRS_WRAP4, 0);
	DoSetRenderState(D3DRS_WRAP5, 0);
	DoSetRenderState(D3DRS_WRAP6, 0);
	DoSetRenderState(D3DRS_WRAP7, 0);
	DoSetRenderState(D3DRS_WRAP8, 0);
	DoSetRenderState(D3DRS_WRAP9, 0);
	DoSetRenderState(D3DRS_WRAP10, 0);
	DoSetRenderState(D3DRS_WRAP11, 0);
	DoSetRenderState(D3DRS_WRAP12, 0);
	DoSetRenderState(D3DRS_WRAP13, 0);
	DoSetRenderState(D3DRS_WRAP14, 0);
	DoSetRenderState(D3DRS_WRAP15, 0);

	// Safe to leave at defaults
	DoSetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	DoSetRenderState(D3DRS_LASTPIXEL, TRUE);
	DoSetRenderState(D3DRS_DITHERENABLE, FALSE);
	DoSetRenderState(D3DRS_SPECULARENABLE, FALSE);
	DoSetRenderState(D3DRS_COLORVERTEX, TRUE);
	DoSetRenderState(D3DRS_LOCALVIEWER, FALSE);
	DoSetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	DoSetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	DoSetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
	DoSetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	DoSetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	DoSetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
	DoSetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	val = 64.0f;
	DoSetRenderState(D3DRS_POINTSIZE, *((DWORD*)&val));
	DoSetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&val));
	val = 1.0f;
	DoSetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&val));
	DoSetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	DoSetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	DoSetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&val));
	DoSetRenderState(D3DRS_POINTSCALE_B, 0);
	DoSetRenderState(D3DRS_POINTSCALE_C, 0);
	DoSetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&val));
	DoSetRenderState(D3DRS_MINTESSELLATIONLEVEL, *((DWORD*)&val));
	DoSetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *((DWORD*)&val));
	DoSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	DoSetRenderState(D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF);
	DoSetRenderState(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);
	DoSetRenderState(D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE);
	DoSetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
	DoSetRenderState(D3DRS_TWEENFACTOR, 0);
	DoSetRenderState(D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC);
	DoSetRenderState(D3DRS_NORMALDEGREE, D3DDEGREE_LINEAR);
	DoSetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	DoSetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	DoSetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
	DoSetRenderState(D3DRS_ADAPTIVETESS_X, 0);
	DoSetRenderState(D3DRS_ADAPTIVETESS_Y, 0);
	DoSetRenderState(D3DRS_ADAPTIVETESS_Z, *((DWORD*)&val));
	DoSetRenderState(D3DRS_ADAPTIVETESS_W, 0);
	DoSetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION, FALSE);
	DoSetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
	DoSetRenderState(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP);
	DoSetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
	DoSetRenderState(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP);
	DoSetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);	
	DoSetRenderState(D3DRS_BLENDFACTOR, 0xFFFFFFFF);
	DoSetRenderState(D3DRS_SRGBWRITEENABLE, 0);
	DoSetRenderState(D3DRS_DEPTHBIAS, 0);
	
	DoSetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
	DoSetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
	DoSetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
	DoSetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
}
	
void 
RD3DEffectStateManager::PushStateStack()
{
	++m_CurStackIndex;
	
	_DEBUG_ASSERT(m_CurStackIndex < MAX_STATE_CHANGE_STACK);
}
	
void 
RD3DEffectStateManager::PopStateStack()
{
	_DEBUG_ASSERT(m_CurStackIndex >= 0);
	
	RD3DStateStackLayer& layer = m_StateChangeStack[m_CurStackIndex];
	for(int i=layer.size-1; i >= 0; --i)
	{
		RD3DRenderStateOp& stateOp = layer.stateChanges[i];
		DoSetRenderState(stateOp.state, stateOp.value);
	}
	
	layer.size = 0;
	--m_CurStackIndex;
}

// IUnknown
HRESULT 
RD3DEffectStateManager::QueryInterface(THIS_ REFIID iid, LPVOID *ppv) 
{
	if (iid == IID_IUnknown || iid == IID_ID3DXEffectStateManager)
	{
		*ppv = static_cast<ID3DXEffectStateManager*>(this);
	} 
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(this)->AddRef();
	return S_OK;
}

ULONG 
RD3DEffectStateManager::AddRef(THIS)
{
	return (ULONG)InterlockedIncrement(&m_RefCount);
}
	
ULONG 
RD3DEffectStateManager::Release(THIS)
{
	if( 0L == InterlockedDecrement(&m_RefCount) )
	{
		delete this;
		return 0L;
	}
	
	return m_RefCount;
}

//////////////////////////////////////////////////////////////////////////
//	
//	NOT SUPPORTED
//
//////////////////////////////////////////////////////////////////////////
HRESULT 
RD3DEffectStateManager::SetTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
	_DEBUG_ASSERT(FALSE);
	return E_FAIL;
}

HRESULT 
RD3DEffectStateManager::SetMaterial(THIS_ CONST D3DMATERIAL9 *pMaterial)
{
	_DEBUG_ASSERT(FALSE);
	return E_FAIL;
}

HRESULT 
RD3DEffectStateManager::SetNPatchMode(THIS_ FLOAT NumSegments)
{
	_DEBUG_ASSERT(FALSE);
	return E_FAIL;
}

HRESULT 
RD3DEffectStateManager::SetFVF(THIS_ DWORD FVF)
{
	_DEBUG_ASSERT(FALSE);
	return E_FAIL;
}
	
HRESULT 
RD3DEffectStateManager::SetLight(THIS_ DWORD Index, CONST D3DLIGHT9 *pLight)
{
	_DEBUG_ASSERT(FALSE);
	return E_FAIL;
}
	
HRESULT 
RD3DEffectStateManager::LightEnable(THIS_ DWORD Index, BOOL Enable)
{
	_DEBUG_ASSERT(FALSE);
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
//
//	These are sent immediately to the runtime/driver
//
//////////////////////////////////////////////////////////////////////////
HRESULT 
RD3DEffectStateManager::SetVertexShaderConstantF(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
{
	return m_pDevice->SetVertexShaderConstantF(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT 
RD3DEffectStateManager::SetVertexShaderConstantI(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
{
	return m_pDevice->SetVertexShaderConstantI(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT 
RD3DEffectStateManager::SetVertexShaderConstantB(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
{
	return m_pDevice->SetVertexShaderConstantB(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT 
RD3DEffectStateManager::SetPixelShaderConstantF(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
{
	return m_pDevice->SetPixelShaderConstantF(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT 
RD3DEffectStateManager::SetPixelShaderConstantI(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
{
	return m_pDevice->SetPixelShaderConstantI(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT 
RD3DEffectStateManager::SetPixelShaderConstantB(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
{
	return m_pDevice->SetPixelShaderConstantB(RegisterIndex, pConstantData, RegisterCount);
}


//////////////////////////////////////////////////////////////////////////
//
//	These states are filtered before sending to the renderer
//
//////////////////////////////////////////////////////////////////////////
HRESULT 
RD3DEffectStateManager::SetTexture(THIS_ DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if(m_pTextures[Stage] != pTexture)
	{
		m_pTextures[Stage] = pTexture;
		return m_pDevice->SetTexture(Stage, pTexture);
	}
	
	return S_OK;
}

HRESULT 
RD3DEffectStateManager::SetVertexShader(THIS_ LPDIRECT3DVERTEXSHADER9 pShader)
{
	if(m_pVertexShader != pShader)
	{
		m_pVertexShader = pShader;
		return m_pDevice->SetVertexShader(pShader);
	}

	return S_OK;
}
	
HRESULT 
RD3DEffectStateManager::SetPixelShader(THIS_ LPDIRECT3DPIXELSHADER9 pShader)
{
	if(m_pPixelShader != pShader)
	{
		m_pPixelShader = pShader;
		return m_pDevice->SetPixelShader(pShader);
	}
	
	return S_OK;
}

HRESULT 
RD3DEffectStateManager::SetTextureStageState(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	DWORD& val = m_TextureStageState[Stage][Type];
	if(val != Value)
	{
		val = Value;
		return m_pDevice->SetTextureStageState(Stage, Type, Value);
	}
	
	return S_OK;
}

HRESULT 
RD3DEffectStateManager::SetSamplerState(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	DWORD& val = m_SamplerStageState[Sampler][Type];
	if(val != Value)
	{
		val = Value;
		return m_pDevice->SetSamplerState(Sampler, Type, Value);
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//	These are filtered and undo states also stored
//
//////////////////////////////////////////////////////////////////////////
HRESULT
RD3DEffectStateManager::SetRenderState(THIS_ D3DRENDERSTATETYPE State, DWORD Value)
{
	if(m_RenderStates[State] != Value)
	{
		_DEBUG_ASSERT( m_CurStackIndex >= 0 );
		RD3DStateStackLayer& layer = m_StateChangeStack[m_CurStackIndex];
		_DEBUG_ASSERT( layer.size < MAX_STATE_CHANGES );
		RD3DRenderStateOp& stateOp = layer.stateChanges[layer.size++];
		stateOp.state = State;
		stateOp.value = m_RenderStates[State];
		
		HRESULT res = DoSetRenderState(State, Value);
	}
	
	return S_OK;
}

_NAMESPACE_END