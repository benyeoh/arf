//==============================================================================
//
//		CREffectD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, May 02, 2006
//
//==============================================================================

#include "RendererD3D.h"


_NAMESPACE_BEGIN

bool ParamSortLess(const REffectParamD3D* p1, const REffectParamD3D* p2)
{
	return p1->type < p2->type;
}

//uint
//CREffectD3DTemplate::DoGetNumEffectDesc()
//{
//	return (uint) m_EffectDesc.size();
//}
//
//REffectParam
//CREffectD3DTemplate::DoGetEffectDesc(uint index)
//{
//	_DEBUG_ASSERT(index >= 0);
//	_DEBUG_ASSERT(index < GetNumEffectDesc());
//
//	REffectParam toReturn;
//	m_EffectDesc[index].param.AssignREffectParam(toReturn);
//	return toReturn;
//}
//
//const wchar*
//CREffectD3DTemplate::DoGetEffectDescName(uint index)
//{
//	_DEBUG_ASSERT(index >= 0);
//	_DEBUG_ASSERT(index < GetNumEffectDesc());
//	
//	return m_EffectDesc[index].name.c_str();
//}

uint 
CREffectD3DTemplate::DoGetNumOfInstancedParams(uint techIndex)
{
	_DEBUG_ASSERT(techIndex >= 0);
	_DEBUG_ASSERT(techIndex < GetNumOfTechs());
	
	return (uint)(m_Techniques[techIndex].instancedParams.size());
}
	
uint 
CREffectD3DTemplate::DoGetInstancedParamSemantic(uint techIndex, uint paramIndex)
{
	_DEBUG_ASSERT(techIndex >= 0);
	_DEBUG_ASSERT(techIndex < GetNumOfTechs());
	_DEBUG_ASSERT(paramIndex >=0);
	_DEBUG_ASSERT(paramIndex < GetNumOfInstancedParams(techIndex));

	return m_D3DInstancedParams[m_Techniques[techIndex].instancedParams[paramIndex]].semantic;
}
	
void 
CREffectD3DTemplate::DoApplyInstancedParams(const REffectParam* pEffectParams)
{
	//_DEBUG_ASSERT(m_CurTech >=0);
	//_DEBUG_ASSERT(m_CurTech < (int)(GetNumOfTechs()));
	//
	//uint start = 0;
	//REffectTechD3D& curTech = m_Techniques[m_CurTech];
	//uint numParams = ((uint)(curTech.instancedParams.size()));
	//
	//_LOOPi( numParams )
	//{
	//	const REffectParam& curParam = pEffectParams[i];
	//	D3DXHANDLE paramHandle = m_D3DDynamicParams[curTech.varyingParams[i]].hParam;

	//	_DEBUG_ASSERT
	//	switch(curParam.type)
	//	{
	//		case EPT_VOID_P:
	//		{
	//			HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVal, D3DX_DEFAULT);
	//			_DEBUG_ASSERT(SUCCEEDED(hr));
	//			break;
	//		}

	//		default:	_DEBUG_ASSERT(FALSE); break;
	//	}
	//}

	//if(m_CurPass >= 0)
	//	m_pD3DEffect->CommitChanges();
}

void 
CREffectD3DTemplate::DoApplyDynamicParams(const REffectParam* pEffectParams)
{
	_DEBUG_ASSERT(m_CurTech >=0);
	_DEBUG_ASSERT(m_CurTech < (int)(GetNumOfTechs()));
	
	uint start = 0;
	REffectTechD3D& curTech = m_Techniques[m_CurTech];
	uint numParams = ((uint)(curTech.varyingParams.size()));
	
	_LOOPi( numParams )
	{
		const REffectParam& curParam = pEffectParams[i];
		D3DXHANDLE paramHandle = m_D3DDynamicParams[curTech.varyingParams[i]].hParam;

		switch(curParam.type)
		{
			case EPT_TEX2D_P:
			{
				CRTexture2DD3D* pTex = (CRTexture2DD3D*)(curParam.pTex);
				HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, pTex->GetD3DTexture()); 
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}	

			case EPT_TEXCUBE_P:
			{
				CRTextureCubeD3D* pCube = (CRTextureCubeD3D*)(curParam.pCube);
				HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, pCube->GetD3DTexture()); 
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
			
			//case EPT_TEXRT_P:
			//{
			//	CRTextureRTD3D* pRT = (CRTextureRTD3D*)(curParam.pRT);
			//	HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, pRT->GetD3DTexture()); 
			//	_DEBUG_ASSERT(SUCCEEDED(hr));
			//	break;
			//}
			
			case EPT_MAT44_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat44, D3DX_DEFAULT);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
			
			case EPT_MAT33_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat33, D3DX_DEFAULT);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
				
			case EPT_MAT22_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat22, D3DX_DEFAULT);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
			
			case EPT_VEC4_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec4, D3DX_DEFAULT);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

			case EPT_VEC3_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec3, D3DX_DEFAULT);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
				
			case EPT_VEC2_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec2, D3DX_DEFAULT);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
				
			case EPT_VOID_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVal, D3DX_DEFAULT);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

			case EPT_FLOAT:
			{
				HRESULT hr = m_pD3DEffect->SetFloat(paramHandle, curParam.floatVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

			case EPT_INT:
			{
				HRESULT hr = m_pD3DEffect->SetInt(paramHandle, curParam.intVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

			case EPT_BOOL:
			{
				HRESULT hr = m_pD3DEffect->SetBool(paramHandle, curParam.boolVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

			case EPT_END:
			{
				break;
			}

			default:	
				_DEBUG_ASSERT(FALSE); 
				break;
		}
	}

	if(m_CurPass >= 0)
		m_pD3DEffect->CommitChanges();
}

void 
CREffectD3DTemplate::DoApplyDynamicParams(const REffectParam* pEffectParams, const REffectParam* pToCompare)
{
	//_DEBUG_ASSERT(pEffectParams);
	_DEBUG_ASSERT(m_CurTech >=0);
	_DEBUG_ASSERT(m_CurTech < (int)(GetNumOfTechs()));
	//_DEBUG_ASSERT( ((uint)(m_Techniques[m_CurTech].varyingParams.size())) == length );
	
	uint start = 0;
	REffectTechD3D& curTech = m_Techniques[m_CurTech];
	uint numParams = ((uint)(curTech.varyingParams.size()));
	
	_LOOPi( numParams )
	{
		const REffectParam& curParam = pEffectParams[i];
		D3DXHANDLE paramHandle = m_D3DDynamicParams[curTech.varyingParams[i]].hParam;
		
		switch(curParam.type)
		{
			case EPT_TEX2D_P:
			{
				CRTexture2DD3D* pTex = (CRTexture2DD3D*)(curParam.pTex);
				if(pTex != pToCompare[i].pTex)
				{
					HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, pTex->GetD3DTexture()); 
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}	
			
			case EPT_TEXCUBE_P:
			{
				CRTextureCubeD3D* pCube = (CRTextureCubeD3D*)(curParam.pCube);
				if(pCube != pToCompare[i].pCube)
				{
					HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, pCube->GetD3DTexture()); 
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}
			
			//case EPT_TEXRT_P:
			//{
			//	CRTextureRTD3D* pRT = (CRTextureRTD3D*)(curParam.pRT);
			//	if(pRT != pToCompare[i].pRT)
			//	{
			//		HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, pRT->GetD3DTexture()); 
			//		_DEBUG_ASSERT(SUCCEEDED(hr));
			//	}
			//	break;
			//}
			
			case EPT_MAT44_P:
			{
				if(curParam.pMat44 != pToCompare[i].pMat44)
				{
					HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat44, D3DX_DEFAULT);
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}
				
			case EPT_MAT33_P:
			{
				if(curParam.pMat33 != pToCompare[i].pMat33)
				{
					HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat33, D3DX_DEFAULT);
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}
			
			case EPT_MAT22_P:
			{
				if(curParam.pMat22 != pToCompare[i].pMat22)
				{
					HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat22, D3DX_DEFAULT);
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}	
		
			case EPT_VEC4_P:
			{
				if(curParam.pVec4 != pToCompare[i].pVec4)
				{	
					HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec4, D3DX_DEFAULT);
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}

			case EPT_VEC3_P:
			{
				if(curParam.pVec3 != pToCompare[i].pVec3)
				{	
					HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec3, D3DX_DEFAULT);
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}
			
			case EPT_VEC2_P:
			{
				if(curParam.pVec2 != pToCompare[i].pVec2)
				{	
					HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec2, D3DX_DEFAULT);
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}
				
			case EPT_VOID_P:
			{
				if(curParam.pVal != pToCompare[i].pVal)
				{	
					HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVal, D3DX_DEFAULT);
					_DEBUG_ASSERT(SUCCEEDED(hr));
				}
				break;
			}
				
			case EPT_FLOAT:
			{
				HRESULT hr = m_pD3DEffect->SetFloat(paramHandle, curParam.floatVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
				
			case EPT_INT:
			{
				HRESULT hr = m_pD3DEffect->SetInt(paramHandle, curParam.intVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
				
			case EPT_BOOL:
			{
				HRESULT hr = m_pD3DEffect->SetBool(paramHandle, curParam.boolVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
				
			default:	_DEBUG_ASSERT(FALSE); break;
		}
	}
	
	if(m_CurPass >= 0)
		m_pD3DEffect->CommitChanges();
}

uint 
CREffectD3DTemplate::DoGetDynamicParamSemantic(uint techIndex, uint paramIndex)
{
	_DEBUG_ASSERT(techIndex >= 0);
	_DEBUG_ASSERT(techIndex < GetNumOfTechs());
	_DEBUG_ASSERT(paramIndex >=0);
	_DEBUG_ASSERT(paramIndex < GetNumOfDynamicParams(techIndex));

	return m_D3DDynamicParams[m_Techniques[techIndex].varyingParams[paramIndex]].semantic;
}

eREffectParamType 
CREffectD3DTemplate::DoGetDynamicParamType(uint techIndex, uint paramIndex)
{
	_DEBUG_ASSERT(techIndex >= 0);
	_DEBUG_ASSERT(techIndex < GetNumOfTechs());
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfDynamicParams(techIndex));
	
	return m_D3DDynamicParams[m_Techniques[techIndex].varyingParams[paramIndex]].type;
}

uint 
CREffectD3DTemplate::DoGetNumOfDynamicParams(uint techIndex)
{
	_DEBUG_ASSERT(techIndex >= 0);
	_DEBUG_ASSERT(techIndex < GetNumOfTechs());
	
	return (uint)(m_Techniques[techIndex].varyingParams.size());
}

uint 
CREffectD3DTemplate::DoGetConstantParamSemantic(uint paramIndex)
{
	_DEBUG_ASSERT(paramIndex >=0);
	_DEBUG_ASSERT(paramIndex < GetNumOfConstantParams());

	return m_D3DConstantParams[paramIndex].semantic;
}

void 
CREffectD3DTemplate::DoApplyConstantParams(const REffectParam* pEffectParams)
{
//	_DEBUG_ASSERT(pEffectParams);
	_DEBUG_ASSERT(m_CurTech >= 0);
	_DEBUG_ASSERT(m_CurTech < (int)(GetNumOfTechs()));
//	_DEBUG_ASSERT( ((uint)(m_Techniques[m_CurTech].constantParams.size())) == length );	
	
	uint numParams = ((uint)(m_Techniques[m_CurTech].constantParams.size()));
	
	_LOOPi( numParams )
	{
		uint paramIndex = m_Techniques[m_CurTech].constantParams[i];

		D3DXHANDLE paramHandle = m_D3DConstantParams[paramIndex].hParam;
		const REffectParam& curParam = pEffectParams[paramIndex];
		
		switch(curParam.type)
		{
		case EPT_TEX2D_P:
			{
				CRTexture2DD3D* pTex = (CRTexture2DD3D*)(curParam.pTex);
				HRESULT hr = m_pD3DEffect->SetTexture(paramHandle,  pTex->GetD3DTexture());
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}	

		//case EPT_TEXRT_P:
		//	{
		//		CRTextureRTD3D* pRT = (CRTextureRTD3D*)(curParam.pRT);
		//		HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, pRT->GetD3DTexture()); 
		//		_DEBUG_ASSERT(SUCCEEDED(hr));
		//		break;
		//	}
			
		case EPT_TEXCUBE_P:
			{
				CRTextureCubeD3D* pCube = (CRTextureCubeD3D*)(curParam.pCube);
				HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, pCube->GetD3DTexture()); 
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}
			
		case EPT_MAT44_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat44->getData(), 64);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_MAT33_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat33->getData(), 36);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_MAT22_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pMat22->getData(), 16);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_VEC4_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec4->getData(), 16);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_VEC3_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec3->getData(), 12);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_VEC2_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, curParam.pVec2->getData(), 8);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_VOID_P:
			{
				HRESULT hr = m_pD3DEffect->SetValue(paramHandle, (curParam.pVal), curParam.numBytes);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_FLOAT:
			{
				HRESULT hr = m_pD3DEffect->SetFloat(paramHandle, curParam.floatVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_INT:
			{
				HRESULT hr = m_pD3DEffect->SetInt(paramHandle, curParam.intVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_BOOL:
			{
				HRESULT hr = m_pD3DEffect->SetBool(paramHandle, curParam.boolVal);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				break;
			}

		case EPT_END:
			{
				break;
			}

		default:	_DEBUG_ASSERT(FALSE); break;
		}
	}
}

uint 
CREffectD3DTemplate::DoGetNumOfConstantParams()
{
	return (uint)(m_D3DConstantParams.size());
}

eREffectParamType 
CREffectD3DTemplate::DoGetConstantParamType(uint paramIndex)
{
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfConstantParams());

	return m_D3DConstantParams[paramIndex].type;
}

int 
CREffectD3DTemplate::DoGetConstantParamIndex(uint semantic)
{
	int count = 0;
	boolean isValid = FALSE;
	
	_ITER_STLi(ParamList, m_D3DConstantParams)
	{
		if((*iitr).semantic == semantic)
		{
			isValid = TRUE;	
			break;
		}
		count++; 
	}
	
	return isValid ? count : R_UNKNOWN;
}

REffectParam
CREffectD3DTemplate::DoGetConstantParamDefaultValue(uint paramIndex)
{
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfConstantParams());

	REffectParam paramToReturn;
	m_D3DConstantParams[paramIndex].defaultValue.AssignREffectParam(paramToReturn);
	return paramToReturn;
}
	
uint 
CREffectD3DTemplate::DoGetConstantParamNumDesc(uint paramIndex)
{
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfConstantParams());

	return (uint)(m_D3DConstantParams[paramIndex].desc.size());
}

REffectParam 
CREffectD3DTemplate::DoGetConstantParamDesc(uint paramIndex, uint descIndex)
{
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfConstantParams());

	REffectParam param;
	m_D3DConstantParams[paramIndex].desc[descIndex].param.AssignREffectParam(param);
	return param;
}

const wchar* 
CREffectD3DTemplate::DoGetConstantParamDescName(uint paramIndex, uint descIndex)
{
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfConstantParams());

	return m_D3DConstantParams[paramIndex].desc[descIndex].name.c_str();
}

uint 
CREffectD3DTemplate::DoGetInstancedParamNumDesc(uint techIndex, uint paramIndex)
{
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfInstancedParams(techIndex));

	return (uint)(m_D3DInstancedParams[m_Techniques[techIndex].instancedParams[paramIndex]].desc.size());
}
	
REffectParam 
CREffectD3DTemplate::DoGetInstancedParamDesc(uint techIndex, uint paramIndex, uint descIndex)
{
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfInstancedParams(techIndex));

	REffectParam param;
	m_D3DInstancedParams[m_Techniques[techIndex].instancedParams[paramIndex]].
		desc[descIndex].param.AssignREffectParam(param);
	return param;
}
	
const wchar* 
CREffectD3DTemplate::DoGetInstancedParamDescName(uint techIndex, uint paramIndex, uint descIndex)
{
	_DEBUG_ASSERT(paramIndex >= 0);
	_DEBUG_ASSERT(paramIndex < GetNumOfInstancedParams(techIndex));

	return m_D3DInstancedParams[m_Techniques[techIndex].instancedParams[paramIndex]].
		desc[descIndex].name.c_str();
}

void 
CREffectD3DTemplate::DoBeginTechnique(uint techIndex)
{
	_DEBUG_ASSERT(techIndex < (uint)(m_Techniques.size()));
	
	HRESULT hr = m_pD3DEffect->SetTechnique(m_Techniques[techIndex].hTech); _DEBUG_ASSERT(SUCCEEDED(hr));
	hr = m_pD3DEffect->Begin(NULL, 
							 D3DXFX_DONOTSAVESAMPLERSTATE |
							 D3DXFX_DONOTSAVESTATE |
							 D3DXFX_DONOTSAVESHADERSTATE );	
	
	_DEBUG_ASSERT(SUCCEEDED(hr));
	
	m_CurTech = techIndex;	
}

void 
CREffectD3DTemplate::DoBeginPass(uint passIndex)
{
	_DEBUG_ASSERT(m_pStateManager);
	
	m_pStateManager->PushStateStack();
	HRESULT hr = m_pD3DEffect->BeginPass(passIndex);	_DEBUG_ASSERT(SUCCEEDED(hr));

	m_CurPass = passIndex;
}

void 
CREffectD3DTemplate::DoEndTechnique()
{
	HRESULT hr = m_pD3DEffect->End(); _DEBUG_ASSERT(SUCCEEDED(hr));

	m_CurTech = R_UNKNOWN;
}

void 
CREffectD3DTemplate::DoEndPass()
{
	_DEBUG_ASSERT(m_pStateManager);

	HRESULT hr = m_pD3DEffect->EndPass();	_DEBUG_ASSERT(SUCCEEDED(hr));
	m_pStateManager->PopStateStack();
	
	m_CurPass = R_UNKNOWN;
}

uint 
CREffectD3DTemplate::DoGetNumOfPasses(uint techIndex)
{
	_DEBUG_ASSERT(techIndex >= 0);
	_DEBUG_ASSERT(techIndex < GetNumOfTechs());
	
	return (uint)(m_Techniques[techIndex].passes.size());
}

uint 
CREffectD3DTemplate::DoGetNumOfTechs()
{
	return (uint)(m_Techniques.size());
}

void
CREffectD3DTemplate::ParsePasses(REffectTechD3D& tech)
{
	D3DXTECHNIQUE_DESC desc;
	m_pD3DEffect->GetTechniqueDesc(tech.hTech, &desc);
	
	_LOOPi(desc.Passes)
	{
		REffectPassD3D pass;
		pass.hPass = m_pD3DEffect->GetPass(tech.hTech, i);
		//ParseParams(pass);

		// TODO: Find pass state
		
		tech.passes.push_back(pass);
	}
}

void
CREffectD3DTemplate::ParseTechniques(const D3DXEFFECT_DESC& desc)
{
	_LOOPi(desc.Techniques)
	{
		REffectTechD3D tech;
		tech.hTech = m_pD3DEffect->GetTechnique(i);
	
		// For each "varying" parameter, find out if it is used within the technique and link to it
		_LOOPj(m_D3DDynamicParams.size())
		{
			if(m_pD3DEffect->IsParameterUsed(m_D3DDynamicParams[j].hParam, tech.hTech))
				tech.varyingParams.push_back(j);				
		}

		// Sort the instanced params here, because we need the params to be order independent
		if(m_D3DInstancedParams.size() > 0)
		{
			struct __D3DInstParamSort
			{
				CREffectD3DTemplate* pTemplate;
				bool operator()(const uint& a, const uint& b)
				{
					uint aHash = pTemplate->m_D3DInstancedParams[a].type | (pTemplate->m_D3DInstancedParams[a].desc[0].param.GetInt() << 16);
					uint bHash = pTemplate->m_D3DInstancedParams[b].type | (pTemplate->m_D3DInstancedParams[b].desc[0].param.GetInt() << 16);

					return aHash < bHash;
				}
			};

			_LOOPj(m_D3DInstancedParams.size())
			{
				// TODO: Figure out how to make this per technique
				//if(m_pD3DEffect->IsParameterUsed(m_D3DInstancedParams[j].hParam, tech.hTech))

				_DEBUG_ASSERT((uint)(m_D3DInstancedParams[j].desc.size()) > 0);
				_DEBUG_ASSERT(m_D3DInstancedParams[j].desc[0].param.type == EPT_INT);

				tech.instancedParams.push_back(j);				
			}
		
			__D3DInstParamSort sorter;
			sorter.pTemplate = this;
			std::sort(tech.instancedParams.begin(), tech.instancedParams.end(), sorter);
		}

		_LOOPj(m_D3DConstantParams.size())
		{
			if(m_pD3DEffect->IsParameterUsed(m_D3DConstantParams[j].hParam, tech.hTech))
				tech.constantParams.push_back(j);				
		}
	
		// Get the pass handles
		ParsePasses(tech);
		
		m_Techniques.push_back(tech);
	}
}

void
CREffectD3DTemplate::ParseAllParams(const D3DXEFFECT_DESC& desc)
{	
	// Then we attempt to expose each parameter
	_LOOPi(desc.Parameters)
	{
		D3DXPARAMETER_DESC paramDesc;
		D3DXHANDLE hParam = m_pD3DEffect->GetParameter(NULL, i);
		HRESULT hr = m_pD3DEffect->GetParameterDesc(hParam, &paramDesc); _DEBUG_ASSERT(SUCCEEDED(hr));
		REffectParamD3D curParam;
		
		curParam.hParam = hParam;
		curParam.paramName = ASCIIToUnicodeAuto(paramDesc.Name);
		//if(curParam.paramName == _W("EffectDesc") )
		//	continue;
			
		curParam.type = EPT_END;

		BOOL isUsed = FALSE;
		for(UINT j=0; j < desc.Techniques; ++j)
		{
			D3DXHANDLE techHandle = m_pD3DEffect->GetTechnique(j);
			if(m_pD3DEffect->IsParameterUsed(hParam, techHandle))
			{
				isUsed = TRUE;
				break;
			}
		}
			
		curParam.semantic	= R_UNKNOWN;
		uint	usage		= 0;
		boolean isInit		= FALSE;
		
		_LOOPj(paramDesc.Annotations)
		{
			D3DXHANDLE hAnnotation = m_pD3DEffect->GetAnnotation(curParam.hParam, j);
			if(hAnnotation)
			{
				D3DXPARAMETER_DESC annotDesc;
				m_pD3DEffect->GetParameterDesc(hAnnotation, &annotDesc);
				std::string annotName = annotDesc.Name;
				if( annotName == "semantic" )
				{
					int val;
					m_pD3DEffect->GetInt(hAnnotation, &val);
					curParam.semantic = (uint) val;
				}
				else
				if( annotName == "usage" )
				{
					INT val;
					m_pD3DEffect->GetInt(hAnnotation, &val);
					usage = val;
				}	
				else
				if( annotName == "always_used" )
				{
					BOOL val;
					m_pD3DEffect->GetBool(hAnnotation, &val);
					isUsed = val ? (boolean) val : isUsed;			
				}
				else
				if( annotName == "isInit" )
				{
					BOOL val;
					m_pD3DEffect->GetBool(hAnnotation, &val);
					isInit = val ? (boolean) val : isInit;	
				}				
				else
				{
					// Store it as a param desc
					curParam.desc.push_back( ParseParamEffectDesc( hAnnotation ) );								
				}		
			}
		}
		
		// TODO: Add proper support for array times
		const static uint MAX_TEMP_STORAGE = 1024;
		if(paramDesc.Elements > 1)
		{
			curParam.type = EPT_VOID_P;
			if(isInit)
			{
				byte valBuffer[MAX_TEMP_STORAGE];
				_DEBUG_ASSERT(paramDesc.Bytes <= MAX_TEMP_STORAGE);
				HRESULT hr = m_pD3DEffect->GetValue(curParam.hParam, (void*) valBuffer, D3DX_DEFAULT);			
				_DEBUG_ASSERT(SUCCEEDED(hr));
				curParam.defaultValue.SetValue((void*)valBuffer, paramDesc.Bytes);
			}
		}
		else
		{	
			switch(paramDesc.Type)
			{
				case D3DXPT_STRING:
				{
					curParam.type = EPT_STRING;
					if(isInit)
					{
						char* pVal = NULL;
						HRESULT hr = m_pD3DEffect->GetString(curParam.hParam, (LPCSTR*) &pVal);
						_DEBUG_ASSERT(SUCCEEDED(hr));	
						curParam.defaultValue.SetString(ASCIIToUnicodeAuto(pVal));
					}
					break;
				}
				case D3DXPT_VOID:	
				{
					curParam.type = EPT_VOID_P;
					if(isInit)
					{
						byte valBuffer[MAX_TEMP_STORAGE];
						_DEBUG_ASSERT(paramDesc.Bytes <= MAX_TEMP_STORAGE);					
						HRESULT hr = m_pD3DEffect->GetValue(curParam.hParam, (void*) valBuffer, D3DX_DEFAULT);
						_DEBUG_ASSERT(SUCCEEDED(hr));		
						curParam.defaultValue.SetValue((void*)valBuffer, paramDesc.Bytes);
					}
					break;
				}
				case D3DXPT_BOOL:	
				{
					curParam.type = EPT_BOOL;		
					if(isInit)
					{
						BOOL val = FALSE;
						HRESULT hr = m_pD3DEffect->GetBool(curParam.hParam, &val);
						_DEBUG_ASSERT(SUCCEEDED(hr));
						curParam.defaultValue.SetBoolean(val);
					}
					break;
				}
				case D3DXPT_INT:	
				{
					curParam.type = EPT_INT;	
					if(isInit)
					{
						int val = FALSE;
						HRESULT hr = m_pD3DEffect->GetInt(curParam.hParam, &val);
						_DEBUG_ASSERT(SUCCEEDED(hr));
						curParam.defaultValue.SetInt(val);
					}
					break;
				}
				case D3DXPT_FLOAT:
				{
					switch(paramDesc.Class)
					{
						case D3DXPC_SCALAR:			
						{
							curParam.type = EPT_FLOAT;	
							if(isInit)
							{		
								float val = 0.0f;
								HRESULT hr = m_pD3DEffect->GetFloat(curParam.hParam, &val);
								_DEBUG_ASSERT(SUCCEEDED(hr));
								curParam.defaultValue.SetFloat(val);	
							}
							break;
						}
						case D3DXPC_VECTOR:			
						{
							switch(paramDesc.Columns)
							{
								case 2: 
								{
									curParam.type = EPT_VEC2_P;
									if(isInit)
									{		
										D3DXVECTOR4 valProxy;
										gmtl::Vec2f val;
										HRESULT hr = m_pD3DEffect->GetVector(curParam.hParam, &valProxy);
										_DEBUG_ASSERT(SUCCEEDED(hr));
										val[0] = valProxy.x; val[1] = valProxy.y;								
										curParam.defaultValue.SetVector2(&val);
									}
									break;
								}
								case 3: 
								{
									curParam.type = EPT_VEC3_P;	
									if(isInit)
									{		
										D3DXVECTOR4 valProxy;
										gmtl::Vec3f val;
										HRESULT hr = m_pD3DEffect->GetVector(curParam.hParam, &valProxy);
										_DEBUG_ASSERT(SUCCEEDED(hr));
										val[0] = valProxy.x; val[1] = valProxy.y; val[2] = valProxy.z;										
										curParam.defaultValue.SetVector3(&val);
									}
									break;
								}
								case 4: 
								{
									curParam.type = EPT_VEC4_P;
									if(isInit)
									{		
										D3DXVECTOR4 valProxy;
										gmtl::Vec4f val;
										HRESULT hr = m_pD3DEffect->GetVector(curParam.hParam, &valProxy);
										_DEBUG_ASSERT(SUCCEEDED(hr));
										val[0] = valProxy.x; val[1] = valProxy.y; val[2] = valProxy.z; val[3] = valProxy.w;	 									
										curParam.defaultValue.SetVector4(&val);	
									}
									break;
								}
								default: _DEBUG_ASSERT(FALSE);	break;
							}	
							break;
						}
						case D3DXPC_MATRIX_ROWS:	
						case D3DXPC_MATRIX_COLUMNS: 
						{
							switch(paramDesc.Rows)
							{
								case 2: 
								{
									_DEBUG_ASSERT(paramDesc.Columns == 2); 
									curParam.type = EPT_MAT22_P;	
									if(isInit)
									{		
										D3DXMATRIX valProxy;
										gmtl::Matrix22f val;
										HRESULT hr = m_pD3DEffect->GetMatrix(curParam.hParam, &valProxy);
										_DEBUG_ASSERT(SUCCEEDED(hr));
										val[0][0] = valProxy._11; val[1][0] = valProxy._12; 
										val[0][1] = valProxy._21; val[1][1] = valProxy._22; 
										curParam.defaultValue.SetMatrix22(&val);	
									}
									break;
								}
								case 3: 
								{
									_DEBUG_ASSERT(paramDesc.Columns == 3); 
									curParam.type = EPT_MAT33_P;	
									if(isInit)
									{		
										D3DXMATRIX valProxy;
										gmtl::Matrix33f val;
										HRESULT hr = m_pD3DEffect->GetMatrix(curParam.hParam, &valProxy);
										_DEBUG_ASSERT(SUCCEEDED(hr));
										val[0][0] = valProxy._11; val[1][0] = valProxy._12; val[2][0] = valProxy._13; 
										val[0][1] = valProxy._21; val[1][1] = valProxy._22; val[2][1] = valProxy._23; 
										val[0][2] = valProxy._31; val[1][2] = valProxy._32; val[2][2] = valProxy._33; 
										curParam.defaultValue.SetMatrix33(&val);
									}
									break;
								}
								case 4: 
								{
									_DEBUG_ASSERT(paramDesc.Columns == 4); 
									curParam.type = EPT_MAT44_P;
									if(isInit)
									{		
										D3DXMATRIX valProxy;
										gmtl::Matrix44f val;
										HRESULT hr = m_pD3DEffect->GetMatrix(curParam.hParam, &valProxy);
										_DEBUG_ASSERT(SUCCEEDED(hr));
										val[0][0] = valProxy._11; val[1][0] = valProxy._12; val[2][0] = valProxy._13; val[3][0] = valProxy._14;
										val[0][1] = valProxy._21; val[1][1] = valProxy._22; val[2][1] = valProxy._23; val[3][1] = valProxy._24;
										val[0][2] = valProxy._31; val[1][2] = valProxy._32; val[2][2] = valProxy._33; val[3][2] = valProxy._34;
										val[0][3] = valProxy._41; val[1][3] = valProxy._42; val[2][3] = valProxy._43; val[3][3] = valProxy._44; 
										curParam.defaultValue.SetMatrix44(&val);
									}
									break;
								}
								default: _DEBUG_ASSERT(FALSE);	break;
							}	
							break;
						}
						
						default: _DEBUG_ASSERT(FALSE); break;
					}
					break;
				}
				case D3DXPT_TEXTURE:
				case D3DXPT_TEXTURE1D:
				case D3DXPT_TEXTURE2D:
				case D3DXPT_TEXTURE3D:
					{	
						curParam.type = EPT_TEX2D_P;
						break;
					}
					
				case D3DXPT_TEXTURECUBE:
					{	
						curParam.type = EPT_TEXCUBE_P;
						break;
					}

				case D3DXPT_SAMPLER:
				case D3DXPT_SAMPLER1D:
				case D3DXPT_SAMPLER2D:
				case D3DXPT_SAMPLER3D:
				case D3DXPT_SAMPLERCUBE:
				case D3DXPT_PIXELSHADER:
				case D3DXPT_VERTEXSHADER:
				case D3DXPT_PIXELFRAGMENT:
				case D3DXPT_VERTEXFRAGMENT:
					break;
			}
		}
		
		if(curParam.semantic != R_UNKNOWN)
		{
			_DEBUG_ASSERT(curParam.type != EPT_END);

			if(isUsed)
			{
				switch(usage)
				{
					case 0:
						m_D3DDynamicParams.push_back( curParam );	
						break;
						
					case 1:
						m_D3DConstantParams.push_back( curParam );				
						break;
						
					case 2:
						m_D3DInstancedParams.push_back( curParam );
						break;
						
					default:
						_DEBUG_ASSERT(FALSE);
						break;
				}					
			}	
			//else
			//{
			//	ParseEffectDesc(curParam);
			//}
		}
	}

}

void
CREffectD3DTemplate::ProcessInstancedParams()
{
	_LOOPi( (uint) m_Techniques.size() )
	{
		InstancingSetup instSetup;
		
		// Reset the size
		instSetup.instancedParamSize = 0;
			
		// Reset the packed description
		instSetup.instancedParamDescPacked[0] = 0;
		instSetup.instancedParamDescPacked[1] = 0;
		instSetup.instancedParamDescPacked[2] = 0;
		instSetup.instancedParamDescPacked[3] = 0;
		
		uint curDescIndex = 0;
		_LOOPj( (uint) m_Techniques[i].instancedParams.size() )
		{
			// Set the size
			REffectParamD3D& curParam = m_D3DInstancedParams[m_Techniques[i].instancedParams[j]];
			instSetup.instancedParamSize += GetEffectParamSize(curParam.type);
					
			// Set the vb desc
			_DEBUG_ASSERT((uint)(curParam.desc.size()) > 0);
			_DEBUG_ASSERT(curParam.desc[0].param.type == EPT_INT);

			uint paramIndex = curParam.desc[0].param.GetInt();
			switch(curParam.type)
			{
				case EPT_VEC4_P:
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF4; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex;				
					break;
				case EPT_VEC3_P:		
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF3; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex;
					break;
				case EPT_VEC2_P:		
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF2; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex;
					break;
				case EPT_FLOAT:		
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF1; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex; 
					break;
					
				case EPT_MAT44_P:	
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF4; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex;
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF4; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex+1;
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF4; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex+2;
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF4; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex+3;
					break;
				case EPT_MAT33_P:
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF3; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex;
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF3; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex+1;
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF3; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex+2;
					break;					
				case EPT_MAT22_P:
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF2; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex;
					instSetup.instancedDesc[curDescIndex].usage = VDU_TEXCOORDF2; 
					instSetup.instancedDesc[curDescIndex++].usageIndex = paramIndex+1;
					break;
					
				default: _DEBUG_ASSERT(FALSE); break;
			}

			// TODO: Actually, we need it to be order dependent because parameters are
			// copied into the instanced VB in the order that they are declared
			// therefore the VB declaration needs to be ordered too...
			
			// We therefore sort the params by their semantics first on parsing

			// Set the packed desc
			// We pack the description usage type according 
			// to the parameter usage index to 4 ints
			// Therefore, the order in which the instanced parameters
			// are declared does not matter anymore when used
			// as a key to index stuff

			// Basically, just packing the curParam.type into a 128-bit integer flag
			// offset by the paramIndex
			uint curPackedIndex		= paramIndex >> 2;	// divide by 4
			_DEBUG_ASSERT(curPackedIndex < 4);
			uint curPackedOffset	= paramIndex & 3;	// mod 4
					
			_DEBUG_ASSERT((uint) curParam.type <= 0xF);
			_DEBUG_ASSERT( (instSetup.instancedParamDescPacked[curPackedIndex] & (0xFF << (curPackedOffset * 8))) == 0x00 );				
			
			instSetup.instancedParamDescPacked[curPackedIndex] |= (curParam.type << (curPackedOffset * 8));				
		}
		
		instSetup.instancedDesc[curDescIndex].usage = VDU_END;
		instSetup.instancedDesc[curDescIndex++].usageIndex = 0xFF;
		
		// Compute the hash of the packed params
		instSetup.instancedParamHash = MurmurHash(instSetup.instancedParamDescPacked, sizeof(uint)*4);
		
		m_InstancingSetups.push_back(instSetup);
	}
}

void 
CREffectD3DTemplate::SetupInstancing(CRVertexBufferGroupD3D* pVBGroup, uint techIndex)
{
	_DEBUG_ASSERT(techIndex >= 0);
	_DEBUG_ASSERT(techIndex < (uint) m_InstancingSetups.size());
	
	InstancingSetup& inst = m_InstancingSetups[techIndex];
	pVBGroup->SetInstancedVB(inst.instancedParamSize, inst.instancedDesc, 
		inst.instancedParamDescPacked, inst.instancedParamHash);	
}

REffectDesc
CREffectD3DTemplate::ParseParamEffectDesc(D3DXHANDLE hAnnotation)
{
	D3DXPARAMETER_DESC annotDesc;
	m_pD3DEffect->GetParameterDesc(hAnnotation, &annotDesc);

	REffectDesc desc;
	
	switch(annotDesc.Type)
	{
		case D3DXPT_STRING:
		{
			char* pVal;
			m_pD3DEffect->GetString(hAnnotation, (LPCSTR*) &pVal);
			desc.param.SetString( ASCIIToUnicodeAuto(pVal) ); 
			break;
		}
		
		case D3DXPT_BOOL:	
		{
			BOOL val;
			m_pD3DEffect->GetBool(hAnnotation, &val);
			desc.param.SetBoolean(val);	
			break;
		}
	
		case D3DXPT_INT:	
		{	int val;
			m_pD3DEffect->GetInt(hAnnotation, &val);
			desc.param.SetInt(val);	
			break;
		}
	
		case D3DXPT_FLOAT:
		{
			switch(annotDesc.Class)
			{
				case D3DXPC_SCALAR:	
					float val;
					m_pD3DEffect->GetFloat(hAnnotation, &val);
					desc.param.SetFloat(val);	
					break;
				case D3DXPC_VECTOR:			
				{
						switch(annotDesc.Columns)
						{
							case 2: { D3DXVECTOR4 val; m_pD3DEffect->GetVector(hAnnotation, &val); desc.param.SetVector2((gmtl::Vec2f*)(&val)); break; }
							case 3: { D3DXVECTOR4 val; m_pD3DEffect->GetVector(hAnnotation, &val); desc.param.SetVector3((gmtl::Vec3f*)(&val)); break; }
							case 4: { D3DXVECTOR4 val; m_pD3DEffect->GetVector(hAnnotation, &val); desc.param.SetVector4((gmtl::Vec4f*)(&val)); break; }
							default: _DEBUG_ASSERT(FALSE);	break;
						}	
						break;
				}
				default: _DEBUG_ASSERT(FALSE); break;
			}
			break;
		}
			
		default: _DEBUG_ASSERT(FALSE); break;	
	}
	
	desc.name = ASCIIToUnicodeAuto(annotDesc.Name);

	return desc;
}

//void
//CREffectD3DTemplate::ParseEffectDesc(REffectParamD3D& param)
//{
//	REffectDesc descParam;
//	descParam.name = param.paramName;
//	
//	switch(param.type)
//	{
//		case EPT_INT:
//		{
//			int val;
//			m_pD3DEffect->GetInt(param.hParam, &val);
//			descParam.param.SetInt(val);
//			//#ifdef _DEBUG
//			//	wchar buf[255];
//			//	wsprintf(buf, _W("EffectDesc: %s %d\n"), descParam.name.c_str(), descParam.param.GetInt());
//			//	((CRRendererD3D*)m_pRenderer)->IAppCallback().Log(buf);
//			//#endif
//			break;
//		}
//		
//		case EPT_FLOAT:
//		{
//			float val;
//			m_pD3DEffect->GetFloat(param.hParam, &val);
//			descParam.param.SetFloat(val);
//			//#ifdef _DEBUG
//			//	wchar buf[255];
//			//	wsprintf(buf, _W("EffectDesc: %s %f\n"), descParam.name.c_str(), descParam.param.GetFloat());
//			//	((CRRendererD3D*)m_pRenderer)->IAppCallback().Log(buf);
//			//#endif
//			break;
//		}
//		
//		case EPT_STRING:
//		{
//			char* pVal;
//			m_pD3DEffect->GetString(param.hParam, (LPCSTR*) &pVal);
//			//ASCIIToUnicode(pVal, (wchar*)(descParam.data));
//			descParam.param.SetString( ASCIIToUnicodeAuto(pVal) ); 
//			//SetString((wchar*)(descParam.data));
//			//#ifdef _DEBUG
//			//	wchar buf[255];
//			//	wsprintf(buf, _W("EffectDesc: %s \"%s\"\n"), descParam.name.c_str(), descParam.param.GetString());
//			//	((CRRendererD3D*)m_pRenderer)->IAppCallback().Log(buf);
//			//#endif
//			break;
//		}
//		
//		default: _DEBUG_ASSERT(FALSE); break;
//	}
//	
//	m_EffectDesc.push_back( descParam );
//}

void 
CREffectD3DTemplate::ParseEffect()
{
	D3DXEFFECT_DESC desc;
	HRESULT hr = m_pD3DEffect->GetDesc(&desc); _DEBUG_ASSERT(SUCCEEDED(hr));

	// Expose all params that have a semantic
	ParseAllParams(desc);
	
	// Get all technique information
	ParseTechniques(desc);
	
	ProcessInstancedParams();
}

void 
CREffectD3DTemplate::SetD3DEffect(ID3DXEffect* pD3DEffect)
{
	m_pD3DEffect = pD3DEffect;
	m_pStateManager = (((CRRendererD3D*)m_pRenderer))->GetStateManager();
	m_pD3DEffect->SetStateManager(m_pStateManager);
	ParseEffect();
}


void 
CREffectD3DTemplate::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost effect template: %s\n"), GetResourceName());

	//m_Type = R_UNKNOWN;
	m_CurTech = R_UNKNOWN;
	m_CurPass = R_UNKNOWN;
	
	//m_Techniques.clear();
	//m_D3DConstantParams.clear();
	//m_D3DDynamicParams.clear();
		
	//m_pD3DEffect->SetStateManager(NULL);
	//m_pStateManager = NULL;
	m_pD3DEffect->OnLostDevice();
	
	
	//
	//_LOOPi( GetNumOfTechs() )
	//{
	//	uint numParams = ((uint)(m_Techniques[i].constantParams.size()));

	//	_LOOPj( numParams )
	//	{
	//		D3DXHANDLE paramHandle = m_D3DConstantParams[m_Techniques[i].constantParams[j]].hParam;

	//		HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, NULL); 
	//	}	
	//	
	//	numParams = ((uint)(m_Techniques[i].varyingParams.size()));
	//	_LOOPj( numParams )
	//	{
	//		D3DXHANDLE paramHandle = m_D3DDynamicParams[m_Techniques[i].varyingParams[j]].hParam;

	//		HRESULT hr = m_pD3DEffect->SetTexture(paramHandle, NULL); 
	//	}	
	//}
}

void 
CREffectD3DTemplate::OnResetDevice()
{
	
	_DEBUG_TRACE(_W("Resetting effect template: %s\n"), GetResourceName());
	
	HRESULT hr = m_pD3DEffect->OnResetDevice();
	_DEBUG_ASSERT(SUCCEEDED(hr));
	
	//boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetEffectTemplate(this);
}

void 
CREffectD3DTemplate::OnRestoreDevice()
{
}


_NAMESPACE_END