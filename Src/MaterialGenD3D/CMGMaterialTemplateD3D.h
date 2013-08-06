//==============================================================================
//
//		CMGMaterialTemplateD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGMaterialTemplateD3D : public CMGMaterialTemplate
{
private: 

public: 
	CMGMaterialTemplateD3D(CMGMaterialGen* pGen)
		: CMGMaterialTemplate(pGen)
	{
	}

	virtual ~CMGMaterialTemplateD3D()
	{
	}

protected:
	void WriteGlobal(wstring& out, CMGGlobal* pGlobal);	
	void WriteLocal(wstring& out, CMGLocal* pLocal);
	void WriteFunction(wstring& out, CMGFunction* pFunc);
	void WriteFunctionOp(wstring& out, CMGFunctionOp* pOp);
	void WriteVertexShader(wstring& out, MGVertexShaderHelper* pShader);
	void WritePixelShader(wstring& out, MGPixelShaderHelper* pShader);
	void WriteTechnique(wstring& out, MGTechniqueHelper* pTech);
	void WritePass(wstring& out, MGPassHelper* pPass);
	void WriteRenderState(wstring& out, CMGRenderState* pState);
		
protected:
	void DoCompile(IByteBuffer* pBuffer);
	
};

_NAMESPACE_END
