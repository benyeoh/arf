//==============================================================================
//
//		CMGMaterialTemplate.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/3/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct MGVertexShaderHelper
{
	IMGVertexShader* pShader;
	std::vector<IMGFunctionOp*> functionOps;
	std::vector<IMGVertexIn*> vertexIn;
	std::vector<IMGVertexOut*> vertexOut;
};

struct MGPixelShaderHelper
{
	IMGPixelShader* pShader;
	std::vector<IMGFunctionOp*> functionOps;
	std::vector<IMGPixelIn*> pixelIn;
	std::vector<IMGPixelOut*> pixelOut;
};

struct MGPassHelper
{
	IMGPass* pPass;
	//MGVertexShaderHelper vs;
	//MGPixelShaderHelper ps;
	std::vector<IMGRenderState*> states;
};

struct MGTechniqueHelper
{
	IMGTechnique* pTech;	
	std::vector<MGPassHelper> passes;
};

class CMGMaterialTemplate : public CRefCounted<IMGMaterialTemplate>
{
private:
	std::vector<IMGGlobalPtr> m_Globals;
	
	std::vector<IMGTechniquePtr> m_Techs;
	std::vector<IMGFunctionPtr> m_Functions;
	std::vector<IMGVertexShaderPtr> m_VS;
	std::vector<IMGPixelShaderPtr> m_PS;

	const boolean* m_pGlobalAvailable;
	uint m_CurFrame;
	
	CMGMaterialGen* m_pGen;
	
protected:
	// TODO: Locals are not resolved
	std::vector<IMGLocalPtr> m_Locals;

	std::vector<IMGGlobal*> m_ResolvedGlobals;
	std::vector<MGTechniqueHelper> m_ResolvedTechniques;
	std::vector<IMGFunction*> m_ResolvedFunctions;
	std::vector<MGVertexShaderHelper> m_ResolvedVertexShaders;
	std::vector<MGPixelShaderHelper> m_ResolvedPixelShaders;
	
	MGTechniqueHelper* m_pCurrentTech;
	MGPassHelper* m_pCurrentPass;
	MGVertexShaderHelper* m_pCurrentVS;
	MGPixelShaderHelper* m_pCurrentPS;

public: 
	CMGMaterialTemplate(CMGMaterialGen* pGen)
		: m_pCurrentVS(NULL)
		, m_pCurrentPS(NULL)
		, m_pCurrentPass(NULL)
		, m_pCurrentTech(NULL)
		, m_pGlobalAvailable(NULL)
		, m_CurFrame(0)
		, m_pGen(pGen)
	{
	}

	virtual ~CMGMaterialTemplate()
	{
	}

protected:
	_PURE( void DoCompile(IByteBuffer* pBuffer) )
	
public: 
	void ResolvedInputResource(CMGInputResource* pIn);
	void ResolvedFunctionOp(CMGFunctionOp* pOp);
	void ResolvedFunction(CMGFunction* pFunc);
	void ResolvedGlobal(CMGGlobal* pGlobal);
	void ResolvedOutputResource(CMGOutputResource* pOut);
	void ResolvedRenderState(CMGRenderState *pState);
	
	boolean IsGlobalAvailable(uint index);
	
	void BeginPass(CMGPass* pPass);
	void ResolvedPass(CMGPass* pPass);
	void EndPass(CMGPass* pPass);
	
	void BeginPixelShader(CMGPixelShader* pShader);
	void ResolvedPixelShader(CMGPixelShader* pShader);
	void EndPixelShader(CMGPixelShader* pShader);

	void BeginVertexShader(CMGVertexShader* pShader);
	void ResolvedVertexShader(CMGVertexShader* pShader);
	void EndVertexShader(CMGVertexShader* pShader);
	
	void BeginTechnique(CMGTechnique* pTech);
	void ResolvedTechnique(CMGTechnique* pTech);
	void EndTechnique(CMGTechnique* pTech);
	
	void ResolvedVertexIn(CMGVertexIn* pIn);
	void ResolvedVertexOut(CMGVertexOut* pOut);
	void ResolvedPixelIn(CMGPixelIn* pIn);
	void ResolvedPixelOut(CMGPixelOut* pOut);

	void AddGlobal(IMGGlobal* pGlobal);
	void AddTechnique(IMGTechnique* pTech);
	void AddLocal(IMGLocal* pLocal);
	void AddFunction(IMGFunction* pFunction);
	void AddVertexShader(IMGVertexShader* pShader);
	void AddPixelShader(IMGPixelShader* pShader);

	uint GetNumOfGlobals() { return (uint) m_Globals.size(); }
	IMGGlobal* GetGlobal(uint index) { return m_Globals[index]; }
	uint GetNumOfFunctions() { return (uint) m_Functions.size(); }
	IMGFunction* GetFunction(uint index) { return m_Functions[index]; }
	uint GetNumOfTechniques() { return (uint) m_Techs.size(); }
	IMGTechnique* GetTechnique(uint index) { return m_Techs[index]; }
	uint GetNumOfLocals() { return (uint) m_Locals.size(); }
	IMGLocal* GetLocal(uint index) { return m_Locals[index]; }
	uint GetNumOfVertexShaders() { return (uint) m_VS.size(); }
	IMGVertexShader* GetVertexShader(uint index) { return m_VS[index]; }
	uint GetNumOfPixelShaders() { return (uint) m_PS.size(); }
	IMGPixelShader* GetPixelShader(uint index) { return m_PS[index]; }

	boolean Compile(IByteBuffer* pBuffer, const boolean* pGlobalAvailable);	
};

_NAMESPACE_END
