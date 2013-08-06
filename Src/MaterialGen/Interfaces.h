//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

struct IMGInputResource;
struct IMGOutputResource;
struct IMGFunction;
struct IMGGlobal;
struct IMGTechnique;
struct IMGPass;
struct IMGVertexShader;
struct IMGPixelShader;
struct IMGLocal;
struct IMGMaterialTemplate;
struct IMGMaterialGen;
struct IMGPixelShaderOp;
struct IMGVertexShaderOp;

enum eMGUsage
{
	MGU_DYNAMIC = 0,
	MGU_CONSTANT,
	MGU_INSTANCED
};

struct MGParamSig
{
	const wchar* pName;
	const wchar* pType;
	
	MGParamSig()
		: pName(NULL)
		, pType(NULL)
	{

	}
};

struct MGInputSig
{
	const wchar* pName;
	const wchar* pType;
	const wchar* pDefaultVal;

	MGInputSig()
		: pDefaultVal(NULL)
		, pName(NULL)
		, pType(NULL)
	{
	}
};

struct MGLocalSig
{
	const wchar* pPreamble;
	const wchar* pName;
	const wchar* pType;
	const wchar* pPostamble;

	MGLocalSig()
		: pPostamble(NULL)
		, pName(NULL)
		, pType(NULL)
		, pPreamble(NULL)
	{

	}
};

struct IMGOutputResource
{
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )
};

struct IMGInputResource
{
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )
	_PURE( const wchar* GetDefaultVal() )

	_PURE( void BindValue(IMGOutputResource* pOutput) )
	_PURE( void BindAltValue(IMGOutputResource* pOutput) )
	
	_PURE( IMGOutputResource* GetBinding() )
	_PURE( IMGOutputResource* GetAltBinding() )
};

struct IMGVertexIn : public IRefCounted
{
	_PURE( IMGOutputResource& GetOutput() )
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )	
};

struct IMGVertexOut : public IRefCounted
{
	_PURE( IMGInputResource& GetInput() )
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )
};

struct IMGPixelIn : public IRefCounted
{
	_PURE( IMGOutputResource& GetOutput() )
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )
};

struct IMGPixelOut : public IRefCounted
{
	_PURE( IMGInputResource& GetInput() )
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )
};

struct IMGGlobal : public IRefCounted
{
	_PURE( void SetSemantic(uint semantic) )
	_PURE( void SetUsage(eMGUsage val) )
	_PURE( void SetValue(const wchar* val) )
	_PURE( void AddDescription(MGInputSig* pSig) )
	_PURE( void SetIsAlwaysUsed(boolean val) )
	_PURE( void SetIsVolatile(boolean val) )
	
	_PURE( uint GetNumOfInputs() )
	_PURE( IMGInputResource& GetInput(uint index) )
	_PURE( IMGOutputResource& GetOutput() )
	
	_PURE( uint GetSemantic() )
	_PURE( eMGUsage GetUsage() )
	_PURE( const wchar* GetValue() )
	_PURE( boolean GetIsAlwaysUsed() )
	_PURE( boolean GetIsVolatile() )
	
	_PURE( uint GetNumOfDesc() )
	_PURE( const wchar* GetDescName(uint i) )
	_PURE( const wchar* GetDescType(uint i) )
	_PURE( const wchar* GetDescVal(uint i) )
	
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )

};

struct IMGLocal : public IRefCounted
{
	_PURE( void SetValue(const wchar* pVal) )
	_PURE( const wchar* GetValue() )
	_PURE( const wchar* GetPreamble() )
	_PURE( const wchar* GetPostamble() )
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )
};

struct IMGFunctionOp : public IRefCounted
{
	_PURE( uint GetNumOfInputs() )
	_PURE( IMGInputResource& GetInput(uint index) )
	_PURE( IMGOutputResource& GetOutput() )
	_PURE( IMGFunction* GetFunction() )
};

struct IMGFunction : public IRefCounted
{
	_PURE( uint GetNumOfInputs() )		
	_PURE( const wchar* GetInputDefaultVal(uint index) )
	_PURE( const wchar* GetInputName(uint index) )
	_PURE( const wchar* GetInputType(uint index) )

	_PURE( const wchar* GetValue() )
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )

	_PURE( void SetValue(const wchar* val) )
	_PURE( IMGFunctionOp* CreateFunctionOp() )
};

struct IMGVertexShader : public IRefCounted
{
	_PURE( void AddFunctionOp(IMGFunctionOp* pOp) )
	_PURE( void AddVertexIn(IMGVertexIn* pIn) )
	_PURE( void AddVertexOut(IMGVertexOut* pOut) )
	_PURE( void SetVersion(const wchar* pMajor, const wchar* pMinor) )
	
	_PURE( const wchar* GetVersionMajor() )
	_PURE( const wchar* GetVersionMinor() )
	_PURE( const wchar* GetName() )

	_PURE( uint GetNumOfVertexIn() )
	_PURE( IMGVertexIn* GetVertexIn(uint index) )
	_PURE( uint GetNumOfVertexOut() )
	_PURE( IMGVertexOut* GetVertexOut(uint index) )
	_PURE( uint GetNumOfFunctionOp() )
	_PURE( IMGFunctionOp* GetFunctionOp(uint index) )
	
	_PURE( IMGVertexShaderOp* CreateShaderOp() )
};

struct IMGVertexShaderOp : public IRefCounted
{
	_PURE( IMGVertexShader* GetShader() )
};

struct IMGPixelShader : public IRefCounted
{
	_PURE( void AddFunctionOp(IMGFunctionOp* pOp) )
	_PURE( void AddPixelIn(IMGPixelIn* pIn) )
	_PURE( void AddPixelOut(IMGPixelOut* pOut) )
	_PURE( void SetVersion(const wchar* pMajor, const wchar* pMinor) )
	
	_PURE( uint GetNumOfPixelIn() )
	_PURE( IMGPixelIn* GetPixelIn(uint index) )
	_PURE( uint GetNumOfPixelOut() )
	_PURE( IMGPixelOut* GetPixelOut(uint index) )
	_PURE( uint GetNumOfFunctionOp() )
	_PURE( IMGFunctionOp* GetFunctionOp(uint index) )

	_PURE( const wchar* GetVersionMajor() )
	_PURE( const wchar* GetVersionMinor() )
	_PURE( const wchar* GetName() )
	
	_PURE( IMGPixelShaderOp* CreateShaderOp() )
};

struct IMGPixelShaderOp : public IRefCounted
{
	_PURE( IMGPixelShader* GetShader() )
};

struct IMGRenderState : public IRefCounted
{
	_PURE( uint GetNumOfInputs() )
	_PURE( IMGInputResource& GetInput(uint index) )
	
	_PURE( const wchar* GetValue() )
	_PURE( const wchar* GetName() )
	_PURE( const wchar* GetType() )
	
	_PURE( void SetValue(const wchar* pVal) )
	
};

struct IMGPass : public IRefCounted
{
	_PURE( void SetVertexShaderOp(IMGVertexShaderOp* pShader) )
	_PURE( void SetPixelShaderOp(IMGPixelShaderOp* pShader) )
	_PURE( void AddRenderState(IMGRenderState* pState) )
	
	_PURE( IMGVertexShaderOp* GetVertexShaderOp() )
	_PURE( IMGPixelShaderOp* GetPixelShaderOp() )
	_PURE( uint GetNumOfRenderStates() )
	_PURE( IMGRenderState* GetRenderState(uint index) )
	
	_PURE( const wchar* GetName() )

};

struct IMGTechnique : public IRefCounted
{
	_PURE( void AddPass(IMGPass* pPass) )
	_PURE( uint GetNumOfPass() )
	_PURE( IMGPass* GetPass(uint index) )	
	_PURE( const wchar* GetName() )
};

struct IMGResourceMgr
{
	_PURE( IMGMaterialTemplate* CreateMaterialTemplate() )
	_PURE( IMGLocal* CreateLocal(const MGLocalSig* pParam) )
	_PURE( IMGGlobal* CreateGlobal(MGParamSig* pParam, const MGInputSig* pIn, uint numInputs) )
	_PURE( IMGVertexIn* CreateVertexIn(const MGParamSig* pParam) )
	_PURE( IMGVertexOut* CreateVertexOut(const MGParamSig* pParam) )
	_PURE( IMGPixelIn* CreatePixelIn(const MGParamSig* pParam) )
	_PURE( IMGPixelOut* CreatePixelOut(const MGParamSig* pParam) )
	_PURE( IMGFunction* CreateFunction(MGParamSig* pParam, const MGInputSig* pIn, uint numInputs) )
	_PURE( IMGVertexShader* CreateVertexShader(const wchar* pName) )
	_PURE( IMGPixelShader* CreatePixelShader(const wchar* pName) )
	_PURE( IMGPass* CreatePass(const wchar* pName) )
	_PURE( IMGTechnique* CreateTechnique(const wchar* pName) )
	_PURE( IMGRenderState* CreateRenderState(const MGParamSig* pParam, const MGInputSig* pIn, uint numInputs) )
};

struct IMGMaterialGen : public IRefCounted
{
	_PURE( void Initialize(IAppCallback& callback) )
	_PURE( IMGResourceMgr& GetResourceMgr() )
};

struct IMGMaterialTemplate : public IRefCounted
{
	_PURE( void AddGlobal(IMGGlobal* pGlobal) )
	_PURE( void AddTechnique(IMGTechnique* pTech) )
	_PURE( void AddLocal(IMGLocal* pLocal) )
	_PURE( void AddFunction(IMGFunction* pFunction) )
	_PURE( void AddPixelShader(IMGPixelShader* pShader) )
	_PURE( void AddVertexShader(IMGVertexShader* pShader) )

	_PURE( uint GetNumOfGlobals() )
	_PURE( IMGGlobal* GetGlobal(uint index) )
	_PURE( uint GetNumOfFunctions() )
	_PURE( IMGFunction* GetFunction(uint index) )
	_PURE( uint GetNumOfTechniques() )
	_PURE( IMGTechnique* GetTechnique(uint index) )
	_PURE( uint GetNumOfLocals() )
	_PURE( IMGLocal* GetLocal(uint index) )
	_PURE( uint GetNumOfVertexShaders() )
	_PURE( IMGVertexShader* GetVertexShader(uint index) )
	_PURE( uint GetNumOfPixelShaders() )
	_PURE( IMGPixelShader* GetPixelShader(uint index) )

	_PURE( boolean Compile(IByteBuffer* pBuffer, const boolean* pGlobalAvailable) )	
};

_DECLARE_SMARTPTR(IMGMaterialGen)
_DECLARE_SMARTPTR(IMGMaterialTemplate)
_DECLARE_SMARTPTR(IMGInputResource)
_DECLARE_SMARTPTR(IMGOutputResource)
_DECLARE_SMARTPTR(IMGVertexIn)
_DECLARE_SMARTPTR(IMGVertexOut)
_DECLARE_SMARTPTR(IMGPixelIn)
_DECLARE_SMARTPTR(IMGPixelOut)
_DECLARE_SMARTPTR(IMGGlobal)
_DECLARE_SMARTPTR(IMGLocal)
_DECLARE_SMARTPTR(IMGFunction)
_DECLARE_SMARTPTR(IMGFunctionOp)
_DECLARE_SMARTPTR(IMGVertexShader)
_DECLARE_SMARTPTR(IMGPixelShader)
_DECLARE_SMARTPTR(IMGVertexShaderOp)
_DECLARE_SMARTPTR(IMGPixelShaderOp)
_DECLARE_SMARTPTR(IMGPass)
_DECLARE_SMARTPTR(IMGTechnique)
_DECLARE_SMARTPTR(IMGRenderState)

_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IMGMaterialGen* (*CreateMaterialGenFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE IMGMaterialGen* CreateMaterialGen() )
}
