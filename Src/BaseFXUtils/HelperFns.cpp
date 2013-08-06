//==============================================================================
//
//		HelperFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/11/2008
//
//==============================================================================

#include "BaseFXUtils.h"

_NAMESPACE_BEGIN

//uint 
//GetCommonParams(IBFXMaterialGroupTemplate* pMatGroup, 
//				REffectParam* pParams,
//				uint* pSemantics,
//				uint paramBufferSize)
//{
//	uint numCommonParams = 0;
//
//	std::vector<uint> commonSemantics;
//	
//	_LOOPk(RFX_MAX_EFFECT_GROUP_BLOCKS)
//	{
//		uint numEffects = pEffectGroup->GetNumOfEffects(k);
//		
//		_LOOPi(numEffects)
//		{
//			IREffect& effect = pEffectGroup->GetEffect(k, i);
//			IREffectTemplate* pEffectTemplate = effect.GetTemplate();
//			uint numParams = effect.GetNumOfParams();
//			
//			_LOOPj(numParams)
//			{
//				uint semantic = pEffectTemplate->GetConstantParamSemantic(j);
//				
//				// If the parameter has not been found
//				if( std::find(commonSemantics.begin(), commonSemantics.end(), semantic) 
//					== commonSemantics.end() )
//				{
//					commonSemantics.push_back(semantic);
//					
//					if(paramBufferSize == 0 || numCommonParams < paramBufferSize)
//					{
//						pParams[numCommonParams] = *(effect.GetParam(j));
//						pSemantics[numCommonParams] = semantic;
//						numCommonParams++;
//					}
//				}				
//			}			
//		}
//	}
//	
//	return numCommonParams;	
//}

uint 
GetCommonParams(IBFXMaterialGroupTemplate* pTemplate, 
				uint* pParamIndices, 
				uint* pMatIndices, 
				uint paramBufferSize)
{
	uint numCommonParams = 0;

	std::vector<uint> commonSemantics;
	uint numMats = pTemplate->GetNumOfMaterials();
	_LOOPi(numMats)
	{
		IBFXMaterial* pMat = pTemplate->GetMaterialByIndex(i);
		IREffectTemplate* pEffect = pMat->GetEffectTemplate(0x7FFFFFFF);
		
		uint numConstantParams = pEffect->GetNumOfConstantParams();
		_LOOPj(numConstantParams)
		{
			uint semantic = pEffect->GetConstantParamSemantic(j);
			// If the parameter has not been found
			if( std::find(commonSemantics.begin(), commonSemantics.end(), semantic) 
				== commonSemantics.end() )
			{
				commonSemantics.push_back(semantic);

				if(paramBufferSize == 0 || numCommonParams < paramBufferSize)
				{
					pMatIndices[numCommonParams] = i;
					pParamIndices[numCommonParams] = j;
					numCommonParams++;
				}
			}		
		}
	}
	
	return numCommonParams;	
}

RVertexDesc
GetVertexDescFromDesc(uint desc)
{
	RVertexDesc toReturn;
	toReturn.usageIndex = desc & 0x0000000F;
	toReturn.usage		= (eRVDUsage) (desc & 0xFFFFFFF0);
	return toReturn;
}

void
AppendVertexDesc(IBFXMaterialGroupTemplate* pTemplate, RVertexDesc* pDesc)
{
	uint numMats = pTemplate->GetNumOfMaterials();
	_LOOPi(numMats)
	{
		IBFXMaterial* pMat = pTemplate->GetMaterialByIndex(i);
		uint maxAvailFlag = ( 1 << (pMat->GetNumOfConstantVolatileParams() + pMat->GetNumOfDynamicVolatileParams()) ) - 1;

		IREffectTemplate* pEffect = pMat->GetEffectTemplate(maxAvailFlag);
		int paramIndex = pEffect->GetConstantParamIndex(BFX_EFFECT_VERTEX_DESC1);
		for(uint j=1; paramIndex >= 0; j++)
		{
			REffectParam vertexDescVal = pEffect->GetConstantParamDefaultValue(paramIndex);
			RVertexDesc desc = GetVertexDescFromDesc(vertexDescVal.intVal);
			uint k = 0;
			while(pDesc[k].usage != VDU_END)
			{
				if(desc.usage == pDesc[k].usage &&
					desc.usageIndex == pDesc[k].usageIndex)
					break;
				++k;
			}

			if(pDesc[k].usage == VDU_END)
			{
				pDesc[k] = desc;
			}	
			
			if(BFX_EFFECT_VERTEX_DESC1+j > BFX_EFFECT_VERTEX_DESC5)
				break;
				
			paramIndex = pEffect->GetConstantParamIndex(BFX_EFFECT_VERTEX_DESC1+j);
		}
	}	
}

void
ComputeBFXMatGroupTemplateDataKey(IBFXMaterialGroupTemplate* pTemplate, REffectParam* pParams, uint numParams, CRCDataKey& key)
{
	IByteBufferPtr pBuffer = _NEW CByteBuffer(256);
	wstring fileName = pTemplate->GetFileName();
	AppendData(pBuffer, fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	
	_LOOPi(numParams)
	{
		switch(pParams[i].type)
		{
			case EPT_TEX2D_P:
			{
				wstring str = pParams[i].pTex->GetFileName();
				AppendData(pBuffer, str.c_str(), (uint)str.size() * sizeof(wchar));
				break;
			}
				
			case EPT_VOID_P:
			{
				AppendData(pBuffer, pParams[i].pVal, pParams[i].numBytes);
				break;
			}
			
			case EPT_MAT44_P:
			{
				AppendData(pBuffer, pParams[i].pMat44, sizeof(gmtl::Matrix44f));
				break;
			}
			
			case EPT_MAT33_P:
			{
				AppendData(pBuffer, pParams[i].pMat33, sizeof(gmtl::Matrix33f));
				break;
			}
			
			case EPT_MAT22_P:
			{
				AppendData(pBuffer, pParams[i].pMat22, sizeof(gmtl::Matrix22f));
				break;
			}
			
			case EPT_VEC4_P:
			{
				AppendData(pBuffer, pParams[i].pVec4, sizeof(gmtl::Vec4f));
				break;
			}
			
			case EPT_VEC3_P:
			{
				AppendData(pBuffer, pParams[i].pVec3, sizeof(gmtl::Vec3f));
				break;
			}
			
			case EPT_VEC2_P:
			{
				AppendData(pBuffer, pParams[i].pVec2, sizeof(gmtl::Vec2f));
				break;
			}
			
			case EPT_FLOAT:
			{
				AppendData(pBuffer, pParams[i].floatVal);
				break;
			}
			
			case EPT_INT:
			{
				AppendData(pBuffer, pParams[i].intVal);
				break;
			}
	
			case EPT_BOOL:
			{
				AppendData(pBuffer, pParams[i].boolVal);
				break;
			}
	
			case EPT_STRING:
			{
				wstring str = pParams[i].pStr;
				AppendData(pBuffer, str.c_str(), (uint)str.size() * sizeof(wchar));
				break;
			}

			//case EPT_TEXRT_P:
			//{
			//	wstring str = pParams[i].pRT->GetFileName();
			//	AppendData(pBuffer, str.c_str(), (uint)str.size() * sizeof(wchar));
			//	break;
			//}
			
			case EPT_TEXCUBE_P:
			{
				wstring str = pParams[i].pCube->GetFileName();
				AppendData(pBuffer, str.c_str(), (uint)str.size() * sizeof(wchar));
				break;			
			}
			
			default: _DEBUG_ASSERT(FALSE); break;
		}
	}
	
	key.Set(pBuffer->GetData(), pBuffer->GetDataLength());
}


_NAMESPACE_END