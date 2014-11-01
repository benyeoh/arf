//==============================================================================
//
//		CRVertexBufferGroupD3D11.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 16 March, 2014
//
//==============================================================================

#include "RendererD3D11Framework.h"

_NAMESPACE_BEGIN

const static uint MAX_NUM_SEMANTICS = 16;
const static char* s_pTexCoordSemanticStr[MAX_NUM_SEMANTICS] =
{
	"TEXCOORD0",
	"TEXCOORD1",
	"TEXCOORD2",
	"TEXCOORD3",
	"TEXCOORD4",
	"TEXCOORD5",
	"TEXCOORD6",
	"TEXCOORD7",
	"TEXCOORD8",
	"TEXCOORD9",
	"TEXCOORD10",
	"TEXCOORD11",
	"TEXCOORD12",
	"TEXCOORD13",
	"TEXCOORD14",
	"TEXCOORD15",
};

void CRVertexBufferGroupD3D11::ResetCache()
{
	if(m_InputLayoutMap.GetNumOfElements() > 0)
	{
		// Decrement refcount for each input-layout and notify if deleted
		DenseChainedHashMap<size_t, ID3D11InputLayout*, NullSizeTHash>::Iterator itr;
		itr = m_InputLayoutMap.BeginIter();
		while(itr.IsValid())
		{
			uint refCount = (uint) (*(itr.GetValue()))->Release();
			if(refCount == 0)
			{
				((CRResourceMgrD3D11*) &(m_pRenderer->GetCRResourceMgr()))->RemoveInputLayout((ID3D11VertexShader*) itr.GetKey(), m_InputDesc, m_NumElems);
			}
		}

		m_InputLayoutMap.Reset();
	}
}

ID3D11InputLayout* CRVertexBufferGroupD3D11::GetInputLayout(ID3D11VertexShader* pVS)
{
	ID3D11InputLayout** ppInputLayout = m_InputLayoutMap.Get((size_t) pVS);
	if(ppInputLayout)
		return *ppInputLayout;

	return NULL;
}

void CRVertexBufferGroupD3D11::AddInputLayout(ID3D11InputLayout* pInputLayout, ID3D11VertexShader* pKey)
{
	_DEBUG_ASSERT(m_InputLayoutMap.Get((size_t) pKey) == NULL);
	m_InputLayoutMap.Add((size_t) pKey, pInputLayout);
}

void CRVertexBufferGroupD3D11::CacheInputLayout()
{
	ResetCache();

	uint curElemIndex = 0;
	uint size = (uint)(m_VBs.size());

	// For each vertex buffer
	_LOOPi( size )
	{
		CRVertexBufferD3D11* pCurVB = (CRVertexBufferD3D11*)(m_VBs[i].GetPtr());
		const RVertexDesc* pCurDesc = pCurVB->GetDescription();

		_DEBUG_ASSERT(pCurDesc);

		uint j = 0;
		uint typeOffset = 0;

		// Convert the vertex description into D3D11 specific structs
		while( pCurDesc[j].usage != VDU_END )
		{
			_DEBUG_ASSERT(j < MAX_VERTEX_DESC);

			switch(pCurDesc[j].usage)
			{
			case VDU_POSITION:
				{
					m_InputDesc[j].SemanticName			= "POSITION"; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R32G32B32_FLOAT; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 12;
					break;
				}

			case VDU_NORMAL:
				{
					m_InputDesc[j].SemanticName			= "NORMAL"; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R32G32B32_FLOAT; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 12;
					break;
				}

			case VDU_BINORMAL:
				{
					m_InputDesc[j].SemanticName			= "BINORMAL"; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R32G32B32_FLOAT; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 12;
					break;
				}

			case VDU_TANGENT:
				{
					m_InputDesc[j].SemanticName			= "TANGENT"; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R32G32B32_FLOAT; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 12;
					break;
				}

			case VDU_BLENDINDICES:	
				{
					m_InputDesc[j].SemanticName			= "BLENDINDICES"; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R8G8B8A8_UNORM; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 4;
					break;
				}

			case VDU_BLENDWEIGHT:
				{
					m_InputDesc[j].SemanticName			= "BLENDWEIGHT"; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R8G8B8A8_UNORM; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 4;
					break;
				}

			case VDU_COLOR:
				{
					m_InputDesc[j].SemanticName			= "COLOR"; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R8G8B8A8_UNORM; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 4;
					break;
				}

			case VDU_TEXCOORDF2:
				{
					_DEBUG_ASSERT(pCurDesc[j].usageIndex < MAX_NUM_SEMANTICS);

					m_InputDesc[j].SemanticName			= s_pTexCoordSemanticStr[pCurDesc[j].usageIndex]; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R32G32_FLOAT; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 8;
					break;
				}

			case VDU_TEXCOORDF1:
				{
					_DEBUG_ASSERT(pCurDesc[j].usageIndex < MAX_NUM_SEMANTICS);

					m_InputDesc[j].SemanticName			= s_pTexCoordSemanticStr[pCurDesc[j].usageIndex]; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R32_FLOAT; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 4;
					break;
				}

			case VDU_TEXCOORDF3:		
				{
					_DEBUG_ASSERT(pCurDesc[j].usageIndex < MAX_NUM_SEMANTICS);

					m_InputDesc[j].SemanticName			= s_pTexCoordSemanticStr[pCurDesc[j].usageIndex]; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R32G32B32_FLOAT; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 12;
					break;
				}

			case VDU_TEXCOORDF4:
				{
					_DEBUG_ASSERT(pCurDesc[j].usageIndex < MAX_NUM_SEMANTICS);

					m_InputDesc[j].SemanticName			= s_pTexCoordSemanticStr[pCurDesc[j].usageIndex]; 
					m_InputDesc[j].SemanticIndex		= 0; 
					m_InputDesc[j].Format				= DXGI_FORMAT_R32G32B32A32_FLOAT; 
					m_InputDesc[j].InputSlot			= i;
					m_InputDesc[j].AlignedByteOffset	= typeOffset;
					m_InputDesc[j].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
					m_InputDesc[j].InstanceDataStepRate = 0;

					typeOffset += 16;
					break;
				}

			default: _DEBUG_ASSERT(FALSE); break;
			}

			++j;
		}
	}

	m_NumElems = j;
}

void CRVertexBufferGroupD3D11::DoNotifyAddedVertexBuffer(CRVertexBuffer* pVB)
{
	// TODO: This is inefficient
	// We should do this once upon a "Finalize" or something
	CacheInputLayout();	
}

void CRVertexBufferGroupD3D11::DoNotifyRemovedVertexBuffer(CRVertexBuffer* pVB)
{
	CacheInputLayout();	
}


_NAMESPACE_END