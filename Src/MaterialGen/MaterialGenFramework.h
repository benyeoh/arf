//==============================================================================
//
//		MaterialGenFramework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/3/2008
//
//==============================================================================

#include "MaterialGen.h"

_NAMESPACE_BEGIN

struct MGInputSigStore
{
	wstring name;
	wstring type;
	wstring defaultVal;
	
	MGInputSigStore()
		: name(_W(""))
		, type(_W(""))
		, defaultVal(_W(""))
	{
	}
	
	MGInputSigStore(const MGInputSigStore& other)
	{
		*this = other;
	}
	
	MGInputSigStore(const MGInputSig& other)
	{
		*this = other;
	}
	
	inline MGInputSigStore& operator =(const MGInputSigStore& rhs)
	{
		name = rhs.name;
		type = rhs.type;
		defaultVal = rhs.defaultVal;
		
		return *this;
	}
	
	inline MGInputSigStore& operator =(const MGInputSig& rhs)
	{
		name = _W("");
		type = _W("");
		defaultVal = _W("");
		
		if(rhs.pName)
			name = rhs.pName;
		if(rhs.pType)
			type = rhs.pType;
		if(rhs.pDefaultVal)
			defaultVal = rhs.pDefaultVal;

		return *this;
	}
};

struct MGParamSigStore
{
	wstring name;
	wstring type;
	
	MGParamSigStore()
		: name(_W(""))
		, type(_W(""))
	{
	}
	
	MGParamSigStore(const MGParamSigStore& other)
	{
		*this = other;
	}

	MGParamSigStore(const MGParamSig& other)
	{
		*this = other;
	}

	inline MGParamSigStore& operator =(const MGParamSigStore& rhs)
	{
		name = rhs.name;
		type = rhs.type;
		
		return *this;
	}

	inline MGParamSigStore& operator =(const MGParamSig& rhs)
	{
		name = _W("");
		type = _W("");
		if(rhs.pName)
			name = rhs.pName;
		if(rhs.pType)
			type = rhs.pType;

		return *this;
	}
};

class CMGMaterialTemplate;
class CMGResourceMgr;
class CMGMaterialGen;

_NAMESPACE_END

#include "IMGShaderNode.h"
#include "CMGOutputResource.h"
#include "CMGInputResource.h"
#include "CMGFunction.h"
#include "CMGFunctionOp.h"
#include "CMGRenderState.h"
#include "CMGGlobal.h"
#include "CMGLocal.h"
#include "CMGPixelIn.h"
#include "CMGPixelOut.h"
#include "CMGPixelShader.h"
#include "CMGVertexIn.h"
#include "CMGVertexOut.h"
#include "CMGVertexShader.h"
#include "CMGVertexShaderOp.h"
#include "CMGPixelShaderOp.h"
#include "CMGPass.h"
#include "CMGTechnique.h"
#include "CMGMaterialTemplate.h"
#include "CMGResourceMgr.h"
#include "CMGMaterialGen.h"
