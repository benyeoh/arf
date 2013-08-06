//==============================================================================
//
//		Interfaces.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/29/2008
//
//==============================================================================

#include "MaterialGenManaged.h"

namespace ARFManaged
{
	namespace MaterialGenManaged
	{
		Function^ 
		FunctionOp::GetFunction()
		{
			return gcnew Function(m_pImpl->GetFunction());
		}
		
		VertexShaderOp^
		VertexShader::CreateShaderOp()
		{
			return gcnew VertexShaderOp(m_pImpl->CreateShaderOp());
		}
		
		PixelShaderOp^ 
		PixelShader::CreateShaderOp()
		{
			return gcnew PixelShaderOp(m_pImpl->CreateShaderOp());
		}
		
		void 
		Global::AddDescription(InputSig sig)
		{
			MGInputSig input;
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;
			pin_ptr<const wchar> pDefaultVal = (const wchar*) NULL;

			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);		
			if(sig.hDefaultVal != nullptr)
				pDefaultVal = PtrToStringChars(sig.hDefaultVal);		

			input.pName = pName;
			input.pType = pType;
			input.pDefaultVal = pDefaultVal;
			
			m_pImpl->AddDescription(&input);	
		}

		MaterialTemplate^ 
		ResourceMgr::CreateMaterialTemplate()
		{
			return gcnew MaterialTemplate(m_pImpl->CreateMaterialTemplate());
		}
		
		Local^ 
		ResourceMgr::CreateLocal(LocalSig sig)
		{
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pPreamble = (const wchar*) NULL;
			pin_ptr<const wchar> pPostamble = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;
			
			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);
			if(sig.hPreamble != nullptr)
				pPreamble = PtrToStringChars(sig.hPreamble);
			if(sig.hPostamble != nullptr)
				pPostamble = PtrToStringChars(sig.hPostamble);			

			MGLocalSig local;
			local.pName = pName;
			local.pPostamble = pPostamble;
			local.pPreamble = pPreamble;
			local.pType = pType;
			
			return gcnew Local(m_pImpl->CreateLocal(&local));			
		}
		
		Global^ 
		ResourceMgr::CreateGlobal(ParamSig sig, array<InputSig>^ pIn)
		{
			MGParamSig param;
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;

			MGInputSig input[32];
			wstring pInName[32];
			wstring pInDefaultVal[32];
			wstring pInType[32];
			
			_LOOPi(pIn->Length)
			{
				if(pIn[i].hName != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hName);
					pInName[i] = pVal;
				}

				if(pIn[i].hType != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hType);
					pInType[i] = pVal;
				}

				if(pIn[i].hDefaultVal != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hDefaultVal);
					pInDefaultVal[i] = pVal;
				}
					
				input[i].pName = pInName[i].size() > 0 ? pInName[i].c_str() : NULL;
				input[i].pType = pInType[i].size() > 0 ? pInType[i].c_str() : NULL;
				input[i].pDefaultVal = pInDefaultVal[i].size() > 0 ? pInDefaultVal[i].c_str() : NULL;
			}
			
			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);		

			param.pName = pName;
			param.pType = pType;

			return gcnew Global(m_pImpl->CreateGlobal(&param, input, pIn->Length));			
		}
		
		VertexIn^ 
		ResourceMgr::CreateVertexIn(ParamSig sig)
		{
			MGParamSig param;
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;

			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);		

			param.pName = pName;
			param.pType = pType;
		
			return gcnew VertexIn(m_pImpl->CreateVertexIn(&param));	
		}
		
		VertexOut^ 
		ResourceMgr::CreateVertexOut(ParamSig sig)
		{
			MGParamSig param;
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;

			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);		

			param.pName = pName;
			param.pType = pType;

			return gcnew VertexOut(m_pImpl->CreateVertexOut(&param));	
		}
		
		PixelIn^ 
		ResourceMgr::CreatePixelIn(ParamSig sig)
		{
			MGParamSig param;
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;

			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);		

			param.pName = pName;
			param.pType = pType;

			return gcnew PixelIn(m_pImpl->CreatePixelIn(&param));	
		}
		
		PixelOut^ 
		ResourceMgr::CreatePixelOut(ParamSig sig)
		{
			MGParamSig param;
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;

			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);		

			param.pName = pName;
			param.pType = pType;

			return gcnew PixelOut(m_pImpl->CreatePixelOut(&param));	
		}
		
		Function^ 
		ResourceMgr::CreateFunction(ParamSig sig, array<InputSig>^ pIn)
		{
			MGParamSig param;
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;

			MGInputSig input[32];
			wstring pInName[32];
			wstring pInDefaultVal[32];
			wstring pInType[32];

			_LOOPi(pIn->Length)
			{
				if(pIn[i].hName != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hName);
					pInName[i] = pVal;
				}

				if(pIn[i].hType != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hType);
					pInType[i] = pVal;
				}

				if(pIn[i].hDefaultVal != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hDefaultVal);
					pInDefaultVal[i] = pVal;
				}

				input[i].pName = pInName[i].size() > 0 ? pInName[i].c_str() : NULL;
				input[i].pType = pInType[i].size() > 0 ? pInType[i].c_str() : NULL;
				input[i].pDefaultVal = pInDefaultVal[i].size() > 0 ? pInDefaultVal[i].c_str() : NULL;
			}

			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);		

			param.pName = pName;
			param.pType = pType;

			return gcnew Function(m_pImpl->CreateFunction(&param, input, pIn->Length));
		}
		
		VertexShader^ 
		ResourceMgr::CreateVertexShader(String^ hName)
		{
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			if(hName != nullptr)
				pName = PtrToStringChars(hName);
			
			return gcnew VertexShader(m_pImpl->CreateVertexShader(pName));
		}
		
		PixelShader^ 
		ResourceMgr::CreatePixelShader(String^ hName)
		{
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			if(hName != nullptr)
				pName = PtrToStringChars(hName);

			return gcnew PixelShader(m_pImpl->CreatePixelShader(pName));
		}
		
		Pass^ 
		ResourceMgr::CreatePass(String^ hName)
		{
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			if(hName != nullptr)
				pName = PtrToStringChars(hName);

			return gcnew Pass(m_pImpl->CreatePass(pName));
		}
		
		Technique^ 
		ResourceMgr::CreateTechnique(String^ hName)
		{
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			if(hName != nullptr)
				pName = PtrToStringChars(hName);

			return gcnew Technique(m_pImpl->CreateTechnique(pName));
		}
		
		RenderState^ 
		ResourceMgr::CreateRenderState(ParamSig sig, array<InputSig>^ pIn)
		{
			MGParamSig param;
			pin_ptr<const wchar> pName = (const wchar*) NULL;
			pin_ptr<const wchar> pType = (const wchar*) NULL;

			MGInputSig input[32];
			wstring pInName[32];
			wstring pInDefaultVal[32];
			wstring pInType[32];

			_LOOPi(pIn->Length)
			{
				if(pIn[i].hName != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hName);
					pInName[i] = pVal;
				}
				
				if(pIn[i].hType != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hType);
					pInType[i] = pVal;
				}
				
				if(pIn[i].hDefaultVal != nullptr)
				{
					pin_ptr<const wchar> pVal = PtrToStringChars(pIn[i].hDefaultVal);
					pInDefaultVal[i] = pVal;
				}
				
				input[i].pName = pInName[i].size() > 0 ? pInName[i].c_str() : NULL;
				input[i].pType = pInType[i].size() > 0 ? pInType[i].c_str() : NULL;
				input[i].pDefaultVal = pInDefaultVal[i].size() > 0 ? pInDefaultVal[i].c_str() : NULL;
			}

			if(sig.hName != nullptr)
				pName = PtrToStringChars(sig.hName);
			if(sig.hType != nullptr)
				pType = PtrToStringChars(sig.hType);		

			param.pName = pName;
			param.pType = pType;

			return gcnew RenderState(m_pImpl->CreateRenderState(&param, input, pIn->Length));
		}
	}
}