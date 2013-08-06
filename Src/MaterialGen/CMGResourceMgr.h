//==============================================================================
//
//		CMGResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/3/2008
//
//==============================================================================

#pragma once
_NAMESPACE_BEGIN

class CMGResourceMgr : public IMGResourceMgr
{
protected: 
	CMGMaterialGen* m_pGen;
	
public: 
	CMGResourceMgr(CMGMaterialGen* pGen)
		: m_pGen(pGen)
	{
	}

	virtual ~CMGResourceMgr()
	{
	}

protected:
	_PURE( IMGMaterialTemplate* DoCreateMaterialTemplate() )
	
public: 
	IMGMaterialTemplate* CreateMaterialTemplate();
	IMGLocal* CreateLocal(const MGLocalSig* pParam);
	IMGGlobal* CreateGlobal(MGParamSig* pParam, const MGInputSig* pIn, uint numInputs);
	IMGVertexIn* CreateVertexIn(const MGParamSig* pParam);
	IMGVertexOut* CreateVertexOut(const MGParamSig* pParam);
	IMGPixelIn* CreatePixelIn(const MGParamSig* pParam);
	IMGPixelOut* CreatePixelOut(const MGParamSig* pParam);
	IMGFunction* CreateFunction(MGParamSig* pParam, const MGInputSig* pIn, uint numInputs);
	IMGVertexShader* CreateVertexShader(const wchar* pName);
	IMGPixelShader* CreatePixelShader(const wchar* pName);
	IMGPass* CreatePass(const wchar* pName);
	IMGTechnique* CreateTechnique(const wchar* pName);
	IMGRenderState* CreateRenderState(const MGParamSig* pParam, const MGInputSig* pIn, uint numInputs);

	CMGFunctionOp* CreateFunctionOp(CMGFunction* pOwner);
	CMGOutputResource* CreateOutputResource(IMGShaderNode* pOwner, const MGParamSig* pParam);
	CMGInputResource* CreateInputResource(MGInputSigStore* pSig);
	CMGVertexShaderOp* CreateVertexShaderOp(CMGVertexShader* pShader);
	CMGPixelShaderOp* CreatePixelShaderOp(CMGPixelShader* pShader);

};


_NAMESPACE_END
