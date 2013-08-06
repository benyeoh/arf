//==============================================================================
//
//		ExportBFXMaterialFn.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/12/2008
//
//==============================================================================

#include "stdafx.h"

IBFXMaterialGroup*
ExportBFXMaterial(XSI::Material& mat)
{
	InitializeMaterialGroups();

	REffectParamStore effectParamStores[RRenderOp::MAX_NUM_PARAMS];	// We need this to store the values
			
	// Find the Render FX Property
	XSI::CRefArray properties = mat.GetProperties();
	_LOOPi(properties.GetCount())
	{
		XSI::CustomProperty prop = properties[i];
		wstring name = prop.GetName().GetWideString();
		if(name.find(_W("BaseFX_Material_Group_Property")) != wstring::npos)
		{
			// Get the mat group
			LONG index = prop.GetParameterValue(_W("BFXParamMatGroupIndex"));
			IBFXMaterialGroupTemplatePtr pTemplate = (*g_pMatGroups)[index];
			uint paramIndices[RRenderOp::MAX_NUM_PARAMS];
			uint matIndices[RRenderOp::MAX_NUM_PARAMS];
			//uint blockIndices[256];
			REffectParam effectParams[RRenderOp::MAX_NUM_PARAMS];
			uint semantics[RRenderOp::MAX_NUM_PARAMS];
			uint numCommonParams = GetCommonParams(pTemplate, paramIndices, matIndices);
			uint curParamIndex = 0;
						
			_LOOPi(numCommonParams)
			{
				IREffectTemplate* pEffect = pTemplate->GetMaterialByIndex(matIndices[i])->GetEffectTemplate(0x7FFFFFFF);
				eREffectParamType type = pEffect->GetConstantParamType(paramIndices[i]);
				uint semantic = pEffect->GetConstantParamSemantic(paramIndices[i]);
				
				REffectParam discardVal;
				discardVal.type = EPT_END;

				wstring paramName;
				uint numDesc = pEffect->GetConstantParamNumDesc(paramIndices[i]);
				_LOOPj(numDesc)
				{
					wstring paramDescName = pEffect->GetConstantParamDescName(paramIndices[i], j);
					if(paramDescName == _W("name"))
					{
						paramName = pEffect->GetConstantParamDesc(paramIndices[i], j).pStr;
						paramName.erase( std::remove(paramName.begin(), paramName.end(), _W(' ')), paramName.end() );
						
						//break;
					}
					else if(paramDescName == _W("discardVal"))
					{
						// Discard this effect param if the value is the same
						// TODO: Only int type supported for now
						switch(type)
						{
							case EPT_INT:
								{
									discardVal.SetInt( pEffect->GetConstantParamDesc(paramIndices[i], j).intVal );
									break;
								}

							case EPT_FLOAT:
								{
									discardVal.SetFloat( pEffect->GetConstantParamDesc(paramIndices[i], j).floatVal );
									break;
								}

							default: _DEBUG_ASSERT(FALSE); break;
						}
					}
				}

				effectParams[curParamIndex].type = EPT_END;
				
				if(paramName.size() > 0)
				{		
					switch(type)
					{
					case EPT_TEX2D_P:
						{
							wchar normalizedPath[256];
							XSI::CString path = prop.GetParameterValue(XSI::CString(paramName.c_str()) + + _W("_TEX2D"));
							uint length = g_pFileSystem->NormalizePath(path.GetWideString(), normalizedPath, 256);		

							IFFilePtr pFile = g_pFileSystem->GetFile(normalizedPath);
							IByteBufferPtr pBuffer = _NEW CByteBuffer();
							uint bytesRead = AppendData(pBuffer, pFile);
							
							// Use the filename as the id
							CRCDataKey resID;
							resID.Set(normalizedPath, length * sizeof(wchar));	
							IRTexture2DPtr pTex = g_pRenderer->GetRResourceMgr().LoadTexture2D(&resID, pBuffer);
							if(pTex)
							{
								pTex->SetFileName(normalizedPath);
								effectParamStores[curParamIndex].SetTexture2D(pTex);
							}
							break;
						}
						
					case EPT_TEXCUBE_P:
						{
							wchar normalizedPath[256];
							XSI::CString path = prop.GetParameterValue(XSI::CString(paramName.c_str()) + + _W("_TEXCUBE"));
							uint length = g_pFileSystem->NormalizePath(path.GetWideString(), normalizedPath, 256);		

							IFFilePtr pFile = g_pFileSystem->GetFile(normalizedPath);
							IByteBufferPtr pBuffer = _NEW CByteBuffer();
							uint bytesRead = AppendData(pBuffer, pFile);

							// Use the filename as the id
							CRCDataKey resID;
							resID.Set(normalizedPath, length * sizeof(wchar));	
							IRTextureCubePtr pCube = g_pRenderer->GetRResourceMgr().LoadTextureCube(&resID, pBuffer);
							if(pCube)
							{
								pCube->SetFileName(normalizedPath);
								effectParamStores[curParamIndex].SetTextureCube(pCube);
							}
							break;
						}
						
					case EPT_VOID_P:
						{
							// TODO: Require customized types
							break;
						}

					case EPT_MAT44_P:
						{
							gmtl::Matrix44f mat;
							mat[0][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("00"));						
							mat[0][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("01"));						
							mat[0][2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("02"));						
							mat[0][3] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("03"));						
							mat[1][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("10"));						
							mat[1][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("11"));						
							mat[1][2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("12"));						
							mat[1][3] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("13"));						
							mat[2][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("20"));						
							mat[2][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("21"));						
							mat[2][2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("22"));						
							mat[2][3] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("23"));						
							mat[3][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("30"));						
							mat[3][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("31"));						
							mat[3][2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("32"));						
							mat[3][3] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("33"));						
							effectParamStores[curParamIndex].SetMatrix44(&mat);
							break;
						}

					case EPT_MAT33_P:
						{			
							gmtl::Matrix33f mat;
							mat[0][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("00"));						
							mat[0][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("01"));						
							mat[0][2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("02"));						
							mat[1][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("10"));						
							mat[1][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("11"));						
							mat[1][2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("12"));						
							mat[2][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("20"));						
							mat[2][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("21"));						
							mat[2][2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("22"));						
							effectParamStores[curParamIndex].SetMatrix33(&mat);
							break;
						}

					case EPT_MAT22_P:
						{
							gmtl::Matrix22f mat;
							mat[0][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("00"));						
							mat[0][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("01"));						
							mat[1][0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("10"));						
							mat[1][1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("11"));						
							effectParamStores[curParamIndex].SetMatrix22(&mat);
							break;
						}

					case EPT_VEC4_P:
						{
							gmtl::Vec4f vec;
							vec[0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("X"));						
							vec[1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("Y"));						
							vec[2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("Z"));						
							vec[3] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("W"));						
							effectParamStores[curParamIndex].SetVector4(&vec);
							break;
						}

					case EPT_VEC3_P:
						{
							gmtl::Vec3f vec;
							vec[0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("X"));						
							vec[1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("Y"));						
							vec[2] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("Z"));						
							effectParamStores[curParamIndex].SetVector3(&vec);
							break;
						}

					case EPT_VEC2_P:
						{
							gmtl::Vec2f vec;
							vec[0] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("X"));						
							vec[1] = prop.GetParameterValue(XSI::CString(paramName.c_str()) + _W("Y"));						
							effectParamStores[curParamIndex].SetVector2(&vec);
							break;
						}

					case EPT_FLOAT:
						{
							float val = (float)(prop.GetParameterValue(paramName.c_str()));
							if(discardVal.type == EPT_END || discardVal.floatVal != val)
								effectParamStores[curParamIndex].SetFloat(val);

							//effectParamStores[curParamIndex].SetFloat(prop.GetParameterValue(paramName.c_str()));
							break;
						}

					case EPT_INT:
						{
							int val = (long)(prop.GetParameterValue(paramName.c_str()));
							if(discardVal.type == EPT_END || discardVal.intVal != val)
								effectParamStores[curParamIndex].SetInt(val);
							break;
						}

					case EPT_BOOL:
						{
							effectParamStores[curParamIndex].SetBoolean(prop.GetParameterValue(paramName.c_str()));
							break;
						}

					case EPT_STRING:
						{
							effectParamStores[curParamIndex].SetString(((XSI::CString)prop.GetParameterValue(paramName.c_str())).GetWideString());
							break;
						}

					default: _DEBUG_ASSERT(FALSE); break;
					}		
					
					if(effectParamStores[curParamIndex].type != EPT_END)
					{
						semantics[curParamIndex] = semantic; 
						effectParamStores[curParamIndex].AssignREffectParam(effectParams[curParamIndex]);		
						curParamIndex++;
					}
				}										
			}
			
			//IFFilePtr pEffectGroupFile = g_pFileSystem->GetFile(g_EffectGroupFileNames[index].c_str());
			//IByteBufferPtr pEffectGroupFileData = _NEW CByteBuffer();
			//uint bytesRead = AppendData(pEffectGroupFileData, pEffectGroupFile);
			//
			//IRFXEffectGroup* pToSave = 
			//	g_pRenderFX->GetRFXResourceMgr().LoadRFXEffectGroup(
			//		pEffectGroupFileData, 
			//		effectParams, 
			//		semantics, 
			//		curParamIndex);
			//SaveEffectGroup(pToSave, pMatBuffer);
						
			CRCDataKey id;
			ComputeBFXMatGroupTemplateDataKey(pTemplate, effectParams, curParamIndex, id);
			IBFXMaterialGroup* pToSave = g_pBaseFX->GetResourceMgr().
				CreateMaterialGroup(&id, pTemplate, effectParams, semantics, curParamIndex);
			
			return pToSave;
		}
	}
	
	return NULL;
}