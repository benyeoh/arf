//==============================================================================
//
//		CreateBaseFXMaterialProperty.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/10/2008
//
//==============================================================================

#include "stdafx.h"

int s_CurrentMatGroup = 0;

XSI::CValue
GetSavedValue(const XSI::CString& name)
{
	long count = (*s_pParameterNameArray).GetCount();
	_LOOPi(count)
	{
		if(name == (*s_pParameterNameArray)[i])
		{
			return (*s_pParameterValArray)[i];
		}
	}
	
	return XSI::CValue();
}

XSI::CStatus
RegisterCreateBaseFXMaterialProperty(XSI::PluginRegistrar& in_reg)
{
	in_reg.RegisterProperty(_W("BFXPropCreateBaseFXMaterial"));

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
DefineConstantProperties(XSI::CustomProperty& prop)
{
	XSI::Parameter param;
	
	prop.AddParameter(_W("BFXParamMatGroupIndex"), 
		XSI::CValue::siInt4,
		XSI::siPersistable,
		_W(""),
		_W(""),
		(LONG) s_CurrentMatGroup,
		param);

	//prop.AddParameter(_W("RFXParamAttachedObjectCount"),
	//	XSI::CValue::siInt4,
	//	XSI::siPersistable,
	//	_W(""),
	//	_W(""),
	//	0L,
	//	param	
	//	);
		
	//prop.AddParameter(_W("RParamRequireTangents"), 
	//	XSI::CValue::siBool, 
	//	XSI::siPersistable,
	//	_W("Require tangent and binormals?"),
	//	_W("Select to export with tangent the binormal data for each vertex"),
	//	true,
	//	param);
	
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
DefineDynamicProperties(XSI::CustomProperty& prop)
{
	InitializeMaterialGroups();
	
	XSI::Parameter param;
	
	param = prop.GetParameter(_W("BFXParamMatGroupIndex"));
	if(!param.IsValid())
		return XSI::CStatus::Fail;
		
	s_CurrentMatGroup = (int)((long)(param.GetValue()));
	
	if(s_CurrentMatGroup >= (int) g_pMatGroups->size())
		s_CurrentMatGroup = 0;

	IBFXMaterialGroupTemplatePtr pTemplate = (*g_pMatGroups)[s_CurrentMatGroup];
	uint paramIndices[256];
	uint matIndices[256];
//	uint blockIndices[256];
	uint numCommonParams = GetCommonParams(pTemplate, paramIndices, matIndices);
	_LOOPi(numCommonParams)
	{
		IREffectTemplate* pEffect = pTemplate->GetMaterialByIndex(matIndices[i])->GetEffectTemplate(0x7FFFFFFF);
		eREffectParamType type = pEffect->GetConstantParamType(paramIndices[i]);
		
		//IREffect& effect = pEffectGroup->GetEffect(blockIndices[i], effectIndices[i]);
		//eREffectParamType type = effect.GetTemplate()->GetConstantParamType(paramIndices[i]);
		uint semantic = pEffect->GetConstantParamSemantic(paramIndices[i]);
	
		REffectParam minVal, maxVal, defaultVal, 
					xMin, xMax, yMin, yMax, zMin, zMax, wMin, wMax;
		wstring scriptName;
		defaultVal = pEffect->GetConstantParamDefaultValue(paramIndices[i]);
		
		//wstring name = pSemanticName;	
		uint numParamDesc = pEffect->GetConstantParamNumDesc(paramIndices[i]);
		_LOOPj(numParamDesc)
		{
			wstring paramDescName = pEffect->GetConstantParamDescName(paramIndices[i], j);
			if(paramDescName == _W("min"))
			{
				minVal = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("max"))
			{
				maxVal = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("name"))
			{
				scriptName = pEffect->GetConstantParamDesc(paramIndices[i], j).pStr;
				scriptName.erase( std::remove(scriptName.begin(), scriptName.end(), _W(' ')), scriptName.end() );
			}
			else if(paramDescName == _W("xMin"))
			{
				xMin = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("yMin"))
			{
				yMin = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("zMin"))
			{
				zMin = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("wMin"))
			{
				wMin = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("xMax"))
			{
				xMax = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("yMax"))
			{
				yMax = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("zMax"))
			{
				zMax = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
			else if(paramDescName == _W("wMax"))
			{
				wMax = pEffect->GetConstantParamDesc(paramIndices[i], j);
			}
		}
		
		if(scriptName.size() > 0)
		{
			switch(type)
			{
				case EPT_TEX2D_P:
				{	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("_TEX2D"), XSI::CValue::siString, XSI::siPersistable, _W(""), _W(""),
						_W(""), param);
					
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("_TEX2D"));
					if(!val.IsEmpty())
						param.PutValue(val);										
					break;
				}
				
				case EPT_TEXCUBE_P:
				{	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("_TEXCUBE"), XSI::CValue::siString, XSI::siPersistable, _W(""), _W(""),
						_W(""), param);

					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("_TEXCUBE"));
					if(!val.IsEmpty())
						param.PutValue(val);										
					break;
				}
					
				case EPT_VOID_P:
				{
					// TODO: Require customized types
					
					break;
				}
				
				case EPT_MAT44_P:
				{
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("00"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("00"));
					if(!val.IsEmpty())
						param.PutValue(val);				
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("01"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("01"));
					if(!val.IsEmpty())
						param.PutValue(val);						
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("02"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("02"));
					if(!val.IsEmpty())
						param.PutValue(val);						
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("03"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("03"));
					if(!val.IsEmpty())
						param.PutValue(val);	
						
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("10"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("10"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("11"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("11"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("12"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("12"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("13"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("13"));
					if(!val.IsEmpty())
						param.PutValue(val);	
						
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("20"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("20"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("21"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("21"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("22"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("22"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("23"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("23"));
					if(!val.IsEmpty())
						param.PutValue(val);	

					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("30"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("30"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("31"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("32"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("32"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("32"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("33"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("33"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					break;
				}

				case EPT_MAT33_P:
				{			
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("00"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("00"));
					if(!val.IsEmpty())
						param.PutValue(val);	
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("01"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("01"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("02"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("02"));
					if(!val.IsEmpty())
						param.PutValue(val);

					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("10"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("10"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("11"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("11"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("12"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("12"));
					if(!val.IsEmpty())
						param.PutValue(val);

					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("20"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("20"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("21"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("21"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("22"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("22"));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				case EPT_MAT22_P:
				{
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("00"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("00"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("01"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("01"));
					if(!val.IsEmpty())
						param.PutValue(val);

					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("10"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("10"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("11"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						0.0f, -256.0f, 256.0f, -256.0f, 256.0f, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("11"));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				case EPT_VEC4_P:
				{
					float min1 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max1 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					float min2 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max2 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					float min3 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max3 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					float min4 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max4 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					
					min1 = xMin.type == EPT_FLOAT ? xMin.floatVal : min1;
					min2 = yMin.type == EPT_FLOAT ? yMin.floatVal : min2;
					min3 = zMin.type == EPT_FLOAT ? zMin.floatVal : min3;
					min4 = wMin.type == EPT_FLOAT ? wMin.floatVal : min4;
					max1 = xMax.type == EPT_FLOAT ? xMax.floatVal : max1;
					max2 = yMax.type == EPT_FLOAT ? yMax.floatVal : max2;
					max3 = zMax.type == EPT_FLOAT ? zMax.floatVal : max3;
					max4 = wMax.type == EPT_FLOAT ? wMax.floatVal : max4;
					
					gmtl::Vec4f default(0.0f, 0.0f, 0.0f, 0.0f);
					if(defaultVal.type == EPT_VEC4_P)
						default = *(defaultVal.pVec4);
						
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("X"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[0], min1, max1, min1, max1, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("X"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("Y"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[1], min2, max2, min2, max2, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("Y"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("Z"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[2], min3, max3, min3, max3, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("Z"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("W"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[3], min4, max4, min4, max4, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("W"));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				case EPT_VEC3_P:
				{
					float min1 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max1 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					float min2 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max2 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					float min3 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max3 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					
					min1 = xMin.type == EPT_FLOAT ? xMin.floatVal : min1;
					min2 = yMin.type == EPT_FLOAT ? yMin.floatVal : min2;
					min3 = zMin.type == EPT_FLOAT ? zMin.floatVal : min3;
					max1 = xMax.type == EPT_FLOAT ? xMax.floatVal : max1;
					max2 = yMax.type == EPT_FLOAT ? yMax.floatVal : max2;
					max3 = zMax.type == EPT_FLOAT ? zMax.floatVal : max3;
					
					gmtl::Vec3f default(0.0f, 0.0f, 0.0f);
					if(defaultVal.type == EPT_VEC3_P)
						default = *(defaultVal.pVec3);
					
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("X"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[0], min1, max1, min1, max1, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("X"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("Y"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[1], min2, max2, min2, max2, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("Y"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("Z"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[2], min3, max3, min3, max3, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("Z"));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				case EPT_VEC2_P:
				{
					float min1 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max1 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					float min2 = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max2 = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					
					min1 = xMin.type == EPT_FLOAT ? xMin.floatVal : min1;
					min2 = yMin.type == EPT_FLOAT ? yMin.floatVal : min2;
					max1 = xMax.type == EPT_FLOAT ? xMax.floatVal : max1;
					max2 = yMax.type == EPT_FLOAT ? yMax.floatVal : max2;
					
					gmtl::Vec2f default(0.0f, 0.0f);
					if(defaultVal.type == EPT_VEC2_P)
						default = *(defaultVal.pVec2);
						
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("X"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[0], min1, max1, min1, max1, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("X"));
					if(!val.IsEmpty())
						param.PutValue(val);
					prop.AddParameter(XSI::CString(scriptName.c_str()) + _W("Y"), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default[1], min2, max2, min2, max2, param);
					val = GetSavedValue(XSI::CString(scriptName.c_str()) + _W("Y"));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				case EPT_FLOAT:
				{
					float min = minVal.type == EPT_FLOAT ? minVal.floatVal : 0.0f;
					float max = maxVal.type == EPT_FLOAT ? maxVal.floatVal : 1.0f;
					float default = defaultVal.type == EPT_FLOAT ? defaultVal.floatVal : 0.0f;
					prop.AddParameter(scriptName.c_str(), XSI::CValue::siFloat, XSI::siPersistable, _W(""), _W(""),
						default, min, max, min, max, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				case EPT_INT:
				{
					prop.AddParameter(scriptName.c_str(), XSI::CValue::siInt4, XSI::siPersistable, _W(""), _W(""),
						0L, -256L, 256L, -256L, 256L, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				case EPT_BOOL:
				{
					prop.AddParameter(scriptName.c_str(), XSI::CValue::siBool, XSI::siPersistable, _W(""), _W(""),
						false, param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				case EPT_STRING:
				{
					prop.AddParameter(scriptName.c_str(), XSI::CValue::siString, XSI::siPersistable, _W(""), _W(""),
						_W(""), param);
					XSI::CValue val = GetSavedValue(XSI::CString(scriptName.c_str()));
					if(!val.IsEmpty())
						param.PutValue(val);
					break;
				}
				
				default: _DEBUG_ASSERT(FALSE); break;
			}	
		}
	}
	
	
	
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
BFXPropCreateBaseFXMaterial_Define(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);

	XSI::CustomProperty prop = ctxt.GetSource();
	prop.PutName(_W("Create BaseFX Material"));

	XSI::CStatus status = DefineConstantProperties(prop);
	status.AssertSucceeded(_W("Define material constant properties failed!"));
	if(status != XSI::CStatus::OK)
		return status;
		
	status = DefineDynamicProperties(prop);
	status.AssertSucceeded(_W("Define material dynamic properties failed!"));
	if(status != XSI::CStatus::OK)
		return status;

	return XSI::CStatus::OK;	
}

XSIPLUGINCALLBACK XSI::CStatus
BFXPropCreateBaseFXMaterial_UpdateLayout(XSI::PPGLayout& layout, boolean isAttachedToObject)
{	
	layout.Clear() ;
	layout.AddGroup(_W("BaseFX Material Groups"));
	
	InitializeMaterialGroups();
	
	XSI::CValueArray matGroupEntries;
	int numMatGroups = (int) (*g_pMatGroups).size();
	_LOOPi(numMatGroups)
	{
		IBFXMaterialGroupTemplate* pTemplate = (*g_pMatGroups)[i];
		matGroupEntries.Add( (XSI::CValue((long)(i+1))).GetAsText() + _W(". ") + pTemplate->GetDesc() );
		matGroupEntries.Add( (long) i );
	}
	
	XSI::PPGItem item;
	item = layout.AddEnumControl(_W("BFXParamMatGroupIndex"), matGroupEntries, _W(""), XSI::siControlCombo);
	item.PutAttribute(XSI::siUINoLabel, true);

	layout.EndGroup();

	layout.AddSpacer(1, 8);
	layout.AddGroup(_W("BaseFX Effect Group Parameters"));
	
	if(s_CurrentMatGroup >= (int) g_pMatGroups->size())
		s_CurrentMatGroup = 0;

	IBFXMaterialGroupTemplate* pTemplate = (*g_pMatGroups)[s_CurrentMatGroup];
	
	uint paramIndices[256];
	uint matIndices[256];
	uint numCommonParams = GetCommonParams(pTemplate, paramIndices, matIndices);
	_LOOPi(numCommonParams)
	{
		IREffectTemplate* pEffect = pTemplate->GetMaterialByIndex(matIndices[i])->GetEffectTemplate(0x7FFFFFFF);				
		eREffectParamType type = pEffect->GetConstantParamType(paramIndices[i]);
		uint semantic = pEffect->GetConstantParamSemantic(paramIndices[i]);
		
		uint numParamDesc = pEffect->GetConstantParamNumDesc(paramIndices[i]);
		wstring name;	
		wstring scriptName;
		wstring uiType;
		wstring uiValue;
		
		wstring xName = _W("X");
		wstring yName = _W("Y");
		wstring zName = _W("Z");
		wstring wName = _W("W");
		
		_LOOPj(numParamDesc)
		{
			wstring paramDescName = pEffect->GetConstantParamDescName(paramIndices[i], j);
			if(paramDescName == _W("name"))
			{
				name = pEffect->GetConstantParamDesc(paramIndices[i], j).pStr;
				scriptName = name;
				scriptName.erase( std::remove(scriptName.begin(), scriptName.end(), _W(' ')), scriptName.end() );				
			}
			else
			if(paramDescName == _W("combo"))
			{
				uiType = paramDescName;
				uiValue = pEffect->GetConstantParamDesc(paramIndices[i], j).pStr;	
			}
			else if(paramDescName == _W("xLabel"))
			{
				xName = pEffect->GetConstantParamDesc(paramIndices[i], j).pStr;
			}
			else if(paramDescName == _W("yLabel"))
			{
				yName = pEffect->GetConstantParamDesc(paramIndices[i], j).pStr;
			}
			else if(paramDescName == _W("zLabel"))
			{
				zName = pEffect->GetConstantParamDesc(paramIndices[i], j).pStr;
			}
			else if(paramDescName == _W("wLabel"))
			{
				wName = pEffect->GetConstantParamDesc(paramIndices[i], j).pStr;
			}
		}

		if(name.size() > 0)
		{
			layout.AddSpacer(1, 8);
			item = layout.AddStaticText(name.c_str());
			
			switch(type)
			{
				case EPT_TEX2D_P:
				{
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("_TEX2D"), L"", XSI::siControlFilePath);
					//item.PutAttribute(XSI::siUIWidthPercentage, 93L);
					
					item.PutAttribute(XSI::siUIInitialDir, GetApplicationRoot() );
					item.PutAttribute(XSI::siUIFileMustExist, true);
					item.PutAttribute(XSI::siUIOpenFile, true);
					//item.PutAttribute(XSI::siUIImageFile, true);
					item.PutAttribute(XSI::siUINoLabel, true);
					break;
				}
				
				case EPT_TEXCUBE_P:
				{
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("_TEXCUBE"), L"", XSI::siControlFilePath);
					//item.PutAttribute(XSI::siUIWidthPercentage, 93L);

					item.PutAttribute(XSI::siUIInitialDir, GetApplicationRoot() );
					item.PutAttribute(XSI::siUIFileMustExist, true);
					item.PutAttribute(XSI::siUIOpenFile, true);
					//item.PutAttribute(XSI::siUIImageFile, true);
					item.PutAttribute(XSI::siUINoLabel, true);
					break;
				}
					
				case EPT_VOID_P:
				{
					// TODO: Require customized types

					break;
				}

				case EPT_MAT44_P:
				{	
					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("00"), L"");
					item.PutLabel(L"00");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("01"), L"");
					item.PutLabel(L"01");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("02"), L"");
					item.PutLabel(L"02");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("03"), L"");
					item.PutLabel(L"03");					
					layout.EndRow();				
					layout.AddSpacer(1, 1);
					
					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("10"), L"");
					item.PutLabel(L"10");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("11"), L"");
					item.PutLabel(L"11");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("12"), L"");
					item.PutLabel(L"12");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("13"), L"");
					item.PutLabel(L"13");					
					layout.EndRow();				
					layout.AddSpacer(1, 1);
					
					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("20"), L"");
					item.PutLabel(L"20");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("21"), L"");
					item.PutLabel(L"21");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("22"), L"");
					item.PutLabel(L"22");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("23"), L"");
					item.PutLabel(L"23");					
					layout.EndRow();				
					layout.AddSpacer(1, 1);
					
					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("30"), L"");
					item.PutLabel(L"30");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("31"), L"");
					item.PutLabel(L"31");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("32"), L"");
					item.PutLabel(L"32");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("33"), L"");
					item.PutLabel(L"33");					
					layout.EndRow();				
					//layout.AddSpacer(1, 1);
					break;
				}

				case EPT_MAT33_P:
				{			
					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("00"), L"");
					item.PutLabel(L"00");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("01"), L"");
					item.PutLabel(L"01");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("02"), L"");
					item.PutLabel(L"02");
					layout.EndRow();				
					layout.AddSpacer(1, 1);

					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("10"), L"");
					item.PutLabel(L"10");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("11"), L"");
					item.PutLabel(L"11");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("12"), L"");
					item.PutLabel(L"12");
					layout.EndRow();				
					layout.AddSpacer(1, 1);

					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("20"), L"");
					item.PutLabel(L"20");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("21"), L"");
					item.PutLabel(L"21");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("22"), L"");
					item.PutLabel(L"22");
					layout.EndRow();				
					break;
				}

				case EPT_MAT22_P:
				{
					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("00"), L"");
					item.PutLabel(L"00");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("01"), L"");
					item.PutLabel(L"01");
					layout.EndRow();				
					layout.AddSpacer(1, 1);

					layout.AddRow();						
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("10"), L"");
					item.PutLabel(L"10");	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("11"), L"");
					item.PutLabel(L"11");
					layout.EndRow();				
					break;
				}

				case EPT_VEC4_P:
				{
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("X"), L"");
					item.PutLabel(xName.c_str());	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("Y"), L"");
					item.PutLabel(yName.c_str());	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("Z"), L"");
					item.PutLabel(zName.c_str());	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("W"), L"");
					item.PutLabel(wName.c_str());					
					break;
				}

				case EPT_VEC3_P:
				{
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("X"), _W(""));
					item.PutLabel(xName.c_str());	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("Y"), _W(""));
					item.PutLabel(yName.c_str());	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("Z"), _W(""));
					item.PutLabel(zName.c_str());		
					break;
				}

				case EPT_VEC2_P:
				{
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("X"), _W(""));
					item.PutLabel(xName.c_str());	layout.AddSpacer(1, 1);
					item = layout.AddItem(XSI::CString(scriptName.c_str()) + _W("Y"), _W(""));
					item.PutLabel(yName.c_str());
					break;
				}

				case EPT_FLOAT:
				{
					item = layout.AddItem(scriptName.c_str(), _W(""));
					item.PutAttribute(XSI::siUINoLabel, true);
					break;
				}

				case EPT_INT:
				{
					if(uiType == _W("combo"))
					{
						XSI::CValueArray valueArray;
						wchar tokenStr[256];
						memcpy(tokenStr, uiValue.c_str(), uiValue.size() * sizeof(wchar));
						tokenStr[(int) uiValue.size()] = 0;
						
						wchar* pStr = wcstok(tokenStr, _W(", \n"));
						LONG index = 0;
						while( pStr != NULL )
						{
							valueArray.Add(XSI::CValue(pStr));
							valueArray.Add(XSI::CValue(index));
							pStr = wcstok(NULL, _W(", \n"));
							index++;
						}
						
						item = layout.AddEnumControl(scriptName.c_str(), valueArray, _W(""), XSI::siControlCombo);
					}
					else
					{
						item = layout.AddItem(scriptName.c_str(), _W(""));
					}
						
					item.PutAttribute(XSI::siUINoLabel, true);
					break;
				}

				case EPT_BOOL:
				{
					item = layout.AddItem(scriptName.c_str(), L"");
					item.PutAttribute(XSI::siUINoLabel, true);
					break;
				}

				case EPT_STRING:
				{
					item = layout.AddItem(scriptName.c_str(), L"");
					item.PutAttribute(XSI::siUINoLabel, true);
					break;
				}
				
				default: _DEBUG_ASSERT(FALSE); break;
			}
		}
	}

	layout.EndGroup();

	//if(!isAttachedToObject)
	//{		
	//	layout.AddSpacer(1, 2);
	//	item = layout.AddButton(_W("RFXButtonAttachToObject"), _W("Attach To Selected Object"));
	//	//item.PutAttribute(XSI::siUIAlignCenter, true);
	//	// item.PutAttribute(XSI::siUICX, 365L)
	//}

		
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
BFXPropCreateBaseFXMaterial_DefineLayout(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);
	XSI::PPGLayout layout = ctxt.GetSource();
	
	return BFXPropCreateBaseFXMaterial_UpdateLayout(layout, FALSE);
}

XSIPLUGINCALLBACK XSI::CStatus
BFXPropCreateBaseFXMaterial_PPGEvent(XSI::CRef& in_ctxt)
{
	XSI::PPGEventContext ctxt(in_ctxt);
	XSI::PPGEventContext::PPGEvent eventID = ctxt.GetEventID();

	XSI::Application app;
	//XSI::UIToolkit kit = app.GetUIToolkit();
	XSI::CStatus status = XSI::CStatus::OK;
	XSI::CustomProperty prop = ctxt.GetSource() ;

	if(eventID == XSI::PPGEventContext::siOnInit)
	{
		// This event meant that the UI was just created.
		// It gives us a chance to set some parameter values.
		// We could even change the layout completely at this point.

		// For this event Source() of the event is the CustomProperty object

		
		app.LogMessage( L"OnInit called for " + prop.GetFullName() ) ;
		
		XSI::Parameter param = prop.GetParameter(_W("BFXParamMatGroupIndex"));
		if(!param.IsValid())
			return XSI::CStatus::Fail;

		s_CurrentMatGroup = (int)((long)(param.GetValue()));
		
		XSI::PPGLayout layout = prop.GetPPGLayout();
		XSI::CString name = prop.GetName();
		//LONG attachedObjects = prop.GetParameterValue(_W("RFXParamAttachedObjectCount"));
		boolean isAttachedToObject = FALSE; //attachedObjects > 0 ? TRUE : FALSE;
		BFXPropCreateBaseFXMaterial_UpdateLayout(layout, isAttachedToObject);
		
		//Redraw the PPG to show the new layout
		ctxt.PutAttribute(L"Refresh", true);
		
		// If you regenerate the layout then call this:
		//ctxt.PutAttribute(L"Refresh", true);

	}
	else 
	if(eventID == XSI::PPGEventContext::siButtonClicked)
	{
		// If there are multiple buttons 
		// we can use this attribute to figure out which one was clicked.
		XSI::CValue buttonPressed = ctxt.GetAttribute( L"Button" ) ;
		app.LogMessage( L"Button pressed: " + buttonPressed.GetAsText() ) ;

		//if(buttonPressed.GetAsText() == _W("RFXButtonAttachToObject"))
		//{
		//	AttachMaterialFromProp(prop);
		//	XSI::PPGLayout layout = prop.GetPPGLayout();
		//	XSI::CString name = prop.GetName();
		//	//LONG attachedObjects = prop.GetParameterValue(_W("RFXParamAttachedObjectCount"));
		//	//boolean isAttachedToObject = attachedObjects > 0 ? TRUE : FALSE;
		//	RFXPropCreateRenderFXMaterial_UpdateLayout(layout, FALSE);
		//	ctxt.PutAttribute(L"Refresh", true);
		//}
		
	}
	else 
	if(eventID == XSI::PPGEventContext::siTabChange)
	{
		// We will be called when the PPG is first opened
		// and every time the tab changes

		// Retrieve the label of the tab that is now active
		XSI::CValue tabLabel = ctxt.GetAttribute( L"Tab" ) ;

		app.LogMessage( L"Tab changed to: " + tabLabel .GetAsText() ) ;
	}
	else 
	if(eventID == XSI::PPGEventContext::siParameterChange) 
	{
		// For this event the Source of the event is the parameter
		// itself
		XSI::Parameter changed = ctxt.GetSource() ;
		XSI::CString   paramName = changed.GetScriptName();
		XSI::CustomProperty prop = changed.GetParent() ;
		XSI::PPGLayout layout = prop.GetPPGLayout();
		
		wstring paramNameW = paramName.GetWideString();
		if(paramNameW.find(_W("_TEX2D")) != wstring::npos)
		{
			//wchar culledPath[256];
			//XSI::CString val = changed.GetValue();
			//
			//g_pFileSystem->NormalizePath(val.GetWideString(), culledPath, 256);		
			//changed.PutValue(XSI::CString(culledPath));	
			//
			//ctxt.PutAttribute(L"Refresh", true);
			
			if(paramNameW.find(_W("DiffuseTexture")) != wstring::npos)
			{
				// Assume it's a diffuse texture
				// TODO: We might want to give the user the option of supplying the texture himself
				XSI::Material mat = prop.GetParent();
				if(mat.IsValid())
				{
					XSI::Shader dxDraw = mat.GetShaders().GetItem(_W("DXDraw"));
					XSI::Parameter shaderParam = dxDraw.GetParameter(_W("previous"));
					XSI::Shader dxShade = shaderParam .GetSource(); // DXFx or DXShade
					shaderParam  = dxShade.GetParameter(_W("previous"));
					XSI::Shader dXTexture = shaderParam .GetSource(); // DXTexture // DXTextureFile
					
					shaderParam  = dXTexture.GetParameter(_W("Texture"));
					
					XSI::CValueArray createImageArgs(3);
					XSI::CValue image;

					createImageArgs[0] = (XSI::CString) changed.GetValue();//L"noIcon.pic";
					app.ExecuteCommand(_W("SICreateImageClip"), createImageArgs, image);
					shaderParam.Connect(createImageArgs[2], dxShade);
				}
			}	
		}
		
		if(paramName == _W("BFXParamMatGroupIndex"))
		{
			long index = changed.GetValue();
			app.LogMessage( L"Parameter Changed: " + paramName  + _W(" ") + XSI::CValue(index).GetAsText()) ;
			
			// Remove all previous dynamic parameters
			XSI::CParameterRefArray params = prop.GetParameters();
			long count = params.GetCount();
			_LOOPi(count)
			{
				XSI::Parameter curParam = params[i];
				if(curParam.GetScriptName() != _W("BFXParamMatGroupIndex"))
				 //&&
					//curParam.GetScriptName() != _W("RFXParamAttachedObjectCount"))
				{
					boolean isFound = FALSE;
					long storeCount = (*s_pParameterNameArray).GetCount();
					_LOOPj(storeCount)
					{
						if(curParam.GetScriptName() == (*s_pParameterNameArray)[j])
						{
							(*s_pParameterValArray)[j] = curParam.GetValue();
							isFound = TRUE;
							break;
						}	
					}
					
					if(!isFound)
					{
						(*s_pParameterNameArray).Add(curParam.GetScriptName());
						(*s_pParameterValArray).Add(curParam.GetValue());
					}
					
					prop.RemoveParameter(curParam);
				}				
			}
			
			DefineDynamicProperties(prop);			
			//LONG attachedObjects = prop.GetParameterValue(_W("RFXParamAttachedObjectCount"));
			//boolean isAttachedToObject = attachedObjects > 0 ? TRUE : FALSE;
			BFXPropCreateBaseFXMaterial_UpdateLayout(layout, FALSE);

			ctxt.PutAttribute(L"Refresh", true);
		}
		
		app.LogMessage( L"Parameter Changed: " + paramName ) ;
	}

	return status;
}
