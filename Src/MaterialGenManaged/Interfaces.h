//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/29/2008
//
//==============================================================================

#pragma once 

namespace ARFManaged
{
	namespace MaterialGenManaged
	{
		ref class InputResource;
		ref class OutputResource;
		ref class Function;
		ref class Global;
		ref class Technique;
		ref class Pass;
		ref class VertexShader;
		ref class PixelShader;
		ref class Local;
		ref class MaterialTemplate;
		ref class MaterialGen;
		ref class PixelShaderOp;
		ref class VertexShaderOp;
		
		public enum struct eMGMUsage
		{
			MGMU_CONSTANT	= _NAMESPACE MGU_CONSTANT,
			MGMU_DYNAMIC	= _NAMESPACE MGU_DYNAMIC,
			MGMU_INSTANCED	= _NAMESPACE MGU_INSTANCED,
		};

		public value struct ParamSig
		{
			String^ hName;
			String^ hType;
		};

		public value struct InputSig
		{
			String^ hName;
			String^ hType;
			String^ hDefaultVal;
		};

		public value struct LocalSig
		{
			String^ hPreamble;
			String^ hName;
			String^ hType;
			String^ hPostamble;
		};
		
		_PROXY_CLASS_BEGIN_NO_REF(OutputResource, IMGOutputResource)	
				
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }
			
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN_NO_REF(InputResource, IMGInputResource)			
		
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }
			String^ GetDefaultVal() { return gcnew String(m_pImpl->GetDefaultVal()); }

			void BindValue(OutputResource^ pOutput) { m_pImpl->BindValue(pOutput->GetImpl()); }
			void BindAltValue(OutputResource^ pOutput) { m_pImpl->BindAltValue(pOutput->GetImpl()); }
			OutputResource^ GetBinding() { return gcnew OutputResource(m_pImpl->GetBinding()); }
			
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(VertexIn, IMGVertexIn)			
		
			OutputResource^ GetOutput() { return gcnew OutputResource(&m_pImpl->GetOutput()); }
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }
			
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(VertexOut, IMGVertexOut)			

			InputResource^ GetInput() { return gcnew InputResource(&m_pImpl->GetInput()); }
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }
		
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(PixelIn, IMGPixelIn)			
		
			OutputResource^ GetOutput() { return gcnew OutputResource(&m_pImpl->GetOutput()); }
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }

		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(PixelOut, IMGPixelOut)			

			InputResource^ GetInput() { return gcnew InputResource(&m_pImpl->GetInput()); }
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }

		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(Global, IMGGlobal)			
		
			void SetSemantic(uint semantic)		{ m_pImpl->SetSemantic(semantic); }
			void SetUsage(uint val)				{ m_pImpl->SetUsage((eMGUsage) val); }
			void SetValue(String^ val)			{ pin_ptr<const wchar> pVal = PtrToStringChars(val); m_pImpl->SetValue(pVal); }
			void AddDescription(InputSig sig);
			void SetIsAlwaysUsed(bool val)		{ m_pImpl->SetIsAlwaysUsed(val ? TRUE : FALSE); }
			void SetIsVolatile(bool val)		{ m_pImpl->SetIsVolatile(val ? TRUE : FALSE); }

			uint GetNumOfInputs()				{ return m_pImpl->GetNumOfInputs(); }
			InputResource^ GetInput(uint index) { return gcnew InputResource(&m_pImpl->GetInput(index)); }
			OutputResource^ GetOutput()			{ return gcnew OutputResource(&m_pImpl->GetOutput()); }

			uint GetSemantic()		{ return m_pImpl->GetSemantic(); }
			uint GetUsage()			{ return (uint) m_pImpl->GetUsage(); }
			String^ GetValue()		{ return gcnew String(m_pImpl->GetValue()); } 
			bool GetIsAlwaysUsed()	{ return m_pImpl->GetIsAlwaysUsed() ? true : false; }
			bool GetIsVolatile()	{ return m_pImpl->GetIsVolatile() ? true : false; }

			uint GetNumOfDesc()			{ return m_pImpl->GetNumOfDesc(); }
			String^ GetDescName(uint i) { return gcnew String(m_pImpl->GetDescName(i)); }
			String^ GetDescType(uint i) { return gcnew String(m_pImpl->GetDescType(i)); }
			String^ GetDescVal(uint i)	{ return gcnew String(m_pImpl->GetDescVal(i)); }

			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }

		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(Local, IMGLocal)			

			void SetValue(String^ val)	{ pin_ptr<const wchar> pVal = PtrToStringChars(val); m_pImpl->SetValue(pVal); }
			String^ GetValue()			{ return gcnew String(m_pImpl->GetValue()); } 

			String^ GetPreamble()	{ return gcnew String(m_pImpl->GetPreamble()); }
			String^ GetPostamble()	{ return gcnew String(m_pImpl->GetPostamble()); }
			String^ GetName()		{ return gcnew String(m_pImpl->GetName()); }
			String^ GetType()		{ return gcnew String(m_pImpl->GetType()); }
		
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(FunctionOp, IMGFunctionOp)			

			uint GetNumOfInputs()	{ return m_pImpl->GetNumOfInputs(); }
			InputResource^ GetInput(uint index)  { return gcnew InputResource(&m_pImpl->GetInput(index)); }
			OutputResource^ GetOutput() { return gcnew OutputResource(&m_pImpl->GetOutput()); }
			Function^ GetFunction();

		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(Function, IMGFunction)			

			uint GetNumOfInputs()	{ return m_pImpl->GetNumOfInputs(); }
			String^ GetInputDefaultVal(uint index) { return gcnew String(m_pImpl->GetInputDefaultVal(index)); }
			String^ GetInputName(uint index) { return gcnew String(m_pImpl->GetInputName(index)); }
			String^ GetInputType(uint index) { return gcnew String(m_pImpl->GetInputType(index)); }
			
			String^ GetValue() { return gcnew String(m_pImpl->GetValue()); }
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }
			
			void SetValue(String^ val)	{ pin_ptr<const wchar> pVal = PtrToStringChars(val); m_pImpl->SetValue(pVal); }
			
			FunctionOp^ CreateFunctionOp() { return gcnew FunctionOp(m_pImpl->CreateFunctionOp()); }

		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(VertexShader, IMGVertexShader)			

			void AddFunctionOp(FunctionOp^ pOp) { m_pImpl->AddFunctionOp(pOp->GetImpl()); }
			void AddVertexIn(VertexIn^ pIn) { m_pImpl->AddVertexIn(pIn->GetImpl()); }
			
			void AddVertexOut(VertexOut^ pOut) { m_pImpl->AddVertexOut(pOut->GetImpl()); }
			void SetVersion(String^ pMajor, String^ pMinor) 
			{ 
				pin_ptr<const wchar> pVal = PtrToStringChars(pMajor); 
				pin_ptr<const wchar> pVal2 = PtrToStringChars(pMinor); 
				m_pImpl->SetVersion(pVal, pVal2); 
			}

			String^ GetVersionMajor() { return gcnew String(m_pImpl->GetVersionMajor()); }
			String^ GetVersionMinor() { return gcnew String(m_pImpl->GetVersionMinor()); }
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }

			uint GetNumOfVertexIn() { return m_pImpl->GetNumOfVertexIn(); }
			VertexIn^ GetVertexIn(uint index) { return gcnew VertexIn(m_pImpl->GetVertexIn(index)); }
		
			uint GetNumOfVertexOut() { return m_pImpl->GetNumOfVertexOut(); }
			VertexOut^ GetVertexOut(uint index) { return gcnew VertexOut(m_pImpl->GetVertexOut(index)); }

			uint GetNumOfFunctionOp() { return m_pImpl->GetNumOfFunctionOp(); }
			FunctionOp^ GetFunctionOp(uint index) { return gcnew FunctionOp(m_pImpl->GetFunctionOp(index)); }

			VertexShaderOp^ CreateShaderOp();
			
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(VertexShaderOp, IMGVertexShaderOp)			

			VertexShader^ GetShader() { return gcnew VertexShader(m_pImpl->GetShader()); }

		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(PixelShader, IMGPixelShader)			

			void AddFunctionOp(FunctionOp^ pOp) { m_pImpl->AddFunctionOp(pOp->GetImpl()); }
			void AddPixelIn(PixelIn^ pIn) { m_pImpl->AddPixelIn(pIn->GetImpl()); }

			void AddPixelOut(PixelOut^ pOut) { m_pImpl->AddPixelOut(pOut->GetImpl()); }
			void SetVersion(String^ pMajor, String^ pMinor) 
			{ 
				pin_ptr<const wchar> pVal = PtrToStringChars(pMajor); 
				pin_ptr<const wchar> pVal2 = PtrToStringChars(pMinor); 
				m_pImpl->SetVersion(pVal, pVal2); 
			}

			String^ GetVersionMajor() { return gcnew String(m_pImpl->GetVersionMajor()); }
			String^ GetVersionMinor() { return gcnew String(m_pImpl->GetVersionMinor()); }
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }

			uint GetNumOfPixelIn() { return m_pImpl->GetNumOfPixelIn(); }
			PixelIn^ GetPixelIn(uint index) { return gcnew PixelIn(m_pImpl->GetPixelIn(index)); }

			uint GetNumOfPixelOut() { return m_pImpl->GetNumOfPixelOut(); }
			PixelOut^ GetPixelOut(uint index) { return gcnew PixelOut(m_pImpl->GetPixelOut(index)); }

			uint GetNumOfFunctionOp() { return m_pImpl->GetNumOfFunctionOp(); }
			FunctionOp^ GetFunctionOp(uint index) { return gcnew FunctionOp(m_pImpl->GetFunctionOp(index)); }

			PixelShaderOp^ CreateShaderOp();
		
		_PROXY_CLASS_END


		_PROXY_CLASS_BEGIN(PixelShaderOp, IMGPixelShaderOp)			
	
			PixelShader^ GetShader() { return gcnew PixelShader(m_pImpl->GetShader()); }
	
		_PROXY_CLASS_END


		_PROXY_CLASS_BEGIN(RenderState, IMGRenderState)			

			uint GetNumOfInputs() { return m_pImpl->GetNumOfInputs(); }
			InputResource^ GetInput(uint index) { return gcnew InputResource(&m_pImpl->GetInput(index)); }

			String^ GetValue() { return gcnew String(m_pImpl->GetValue()); }
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
			String^ GetType() { return gcnew String(m_pImpl->GetType()); }

			void SetValue(String^ hVal) 
			{ 
				pin_ptr<const wchar> pVal = PtrToStringChars(hVal); 
				m_pImpl->SetValue(pVal); 
			}
			
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(Pass, IMGPass)			

			void SetVertexShaderOp(VertexShaderOp^ pShader) { m_pImpl->SetVertexShaderOp(pShader->GetImpl()); }
			void SetPixelShaderOp(PixelShaderOp^ pShader) { m_pImpl->SetPixelShaderOp(pShader->GetImpl()); }
			void AddRenderState(RenderState^ pState) { m_pImpl->AddRenderState(pState->GetImpl()); }

			VertexShaderOp^ GetVertexShaderOp() { return gcnew VertexShaderOp(m_pImpl->GetVertexShaderOp()); }
			PixelShaderOp^ GetPixelShaderOp()	{ return gcnew PixelShaderOp(m_pImpl->GetPixelShaderOp()); }

			uint GetNumOfRenderStates() { return m_pImpl->GetNumOfRenderStates(); }
			RenderState^ GetRenderState(uint index) { return gcnew RenderState(m_pImpl->GetRenderState(index)); }
			
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
		
		_PROXY_CLASS_END

		
		_PROXY_CLASS_BEGIN(Technique, IMGTechnique)			
	
			void AddPass(Pass^ pPass) { m_pImpl->AddPass(pPass->GetImpl()); }

			uint GetNumOfPass() { return m_pImpl->GetNumOfPass(); }
			Pass^ GetPass(uint index) { return gcnew Pass(m_pImpl->GetPass(index)); }
			
			String^ GetName() { return gcnew String(m_pImpl->GetName()); }
		
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(MaterialTemplate, IMGMaterialTemplate)			
		
			void AddGlobal(Global^ pGlobal) { m_pImpl->AddGlobal(pGlobal->GetImpl()); }
			void AddTechnique(Technique^ pTech) { m_pImpl->AddTechnique(pTech->GetImpl()); }
			void AddLocal(Local^ pLocal) { m_pImpl->AddLocal(pLocal->GetImpl()); }
			void AddFunction(Function^ pFunction) { m_pImpl->AddFunction(pFunction->GetImpl()); }
			void AddPixelShader(PixelShader^ pShader) { m_pImpl->AddPixelShader(pShader->GetImpl()); }
			void AddVertexShader(VertexShader^ pShader) { m_pImpl->AddVertexShader(pShader->GetImpl()); }

			uint GetNumOfGlobals() { return m_pImpl->GetNumOfGlobals(); }
			Global^ GetGlobal(uint index) { return gcnew Global(m_pImpl->GetGlobal(index)); }
			uint GetNumOfFunctions() { return m_pImpl->GetNumOfFunctions(); }
			Function^ GetFunction(uint index) { return gcnew Function(m_pImpl->GetFunction(index)); }
			uint GetNumOfTechniques() { return m_pImpl->GetNumOfTechniques(); }
			Technique^ GetTechnique(uint index) { return gcnew Technique(m_pImpl->GetTechnique(index)); }
			uint GetNumOfLocals() { return m_pImpl->GetNumOfLocals(); }
			Local^ GetLocal(uint index) { return gcnew Local(m_pImpl->GetLocal(index)); }
			uint GetNumOfVertexShaders() { return m_pImpl->GetNumOfVertexShaders(); }
			VertexShader^ GetVertexShader(uint index) { return gcnew VertexShader(m_pImpl->GetVertexShader(index)); }

			uint GetNumOfPixelShaders() { return m_pImpl->GetNumOfPixelShaders(); }
			PixelShader^ GetPixelShader(uint index) { return gcnew PixelShader(m_pImpl->GetPixelShader(index)); }

			bool Compile(CoreManaged::ByteBuffer^ pBuffer, array<bool>^ pGlobalAvailable) 
			{
				pin_ptr<bool> pAvail = &pGlobalAvailable[0];
				boolean* pBool = (boolean*) pAvail;
				IByteBuffer* pNativeBuffer = pBuffer->GetImpl();
				return m_pImpl->Compile(pNativeBuffer, pBool) ? true : false;
			}
			
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN_NO_REF(ResourceMgr, IMGResourceMgr)			
		
			MaterialTemplate^ CreateMaterialTemplate();
			Local^ CreateLocal(LocalSig sig);
			Global^ CreateGlobal(ParamSig sig, array<InputSig>^ pIn);
			VertexIn^ CreateVertexIn(ParamSig sig);
			VertexOut^ CreateVertexOut(ParamSig sig);
			PixelIn^ CreatePixelIn(ParamSig sig);
			PixelOut^ CreatePixelOut(ParamSig sig);
			Function^ CreateFunction(ParamSig sig, array<InputSig>^ pIn);
			VertexShader^ CreateVertexShader(String^ hName);
			PixelShader^ CreatePixelShader(String^ hName);
			Pass^ CreatePass(String^ hName);
			Technique^ CreateTechnique(String^ hName);
			RenderState^ CreateRenderState(ParamSig sig, array<InputSig>^ pIn);
		
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(MaterialGen, IMGMaterialGen)			
		
			//!MaterialGen()
			//{
			//	if(m_pImpl)
			//		m_pImpl->Release();
			//	
			//}
			void Initialize(CoreManaged::AppCallback^ hCallback) { m_pImpl->Initialize(*(hCallback->GetImpl())); }
			ResourceMgr^ GetResourceMgr() { return gcnew ResourceMgr(&m_pImpl->GetResourceMgr()); }
		
		_PROXY_CLASS_END


	}
}