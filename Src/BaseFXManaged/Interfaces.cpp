//==============================================================================
//
//		Interfaces.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/4/2009
//
//==============================================================================

#include "BaseFXManaged.h"

namespace ARFManaged
{
	namespace BaseFXManaged
	{
		_NAMESPACE IREffect* MaterialCreateCallbackAdapter::CreateEffect(_NAMESPACE IBFXMaterialGroup* pGroup, uint type, uint availability)
		{
			MaterialGroup^ hGroup = gcnew MaterialGroup(pGroup);
			Effect^ hEffect = m_hCallback->CreateEffect(hGroup, type, availability);
			return hEffect->GetImpl();
		}

		boolean 
		BaseFX::Initialize(RendererManaged::Renderer^ hRenderer, 
							MaterialGenManaged::MaterialGen^ hMatGen, 
							CoreManaged::AppCallback^ hCallback)
		{
			return m_pImpl->Initialize(
				*(hRenderer->GetImpl()), *(hMatGen->GetImpl()), *(hCallback->GetImpl()));
			
		}

		void BaseFX::Render(Mesh^ hMesh, float detailLevel, uint matType, uint libIndex, ParamContainer^ hParamContainer, RenderContainer^ hRenderContainer, MaterialCreateCallback^ hCreateCallback)	
		{			
			IBFXMaterialCreateCallback* pCreateCallback = NULL;
			if(hCreateCallback != nullptr)
			{
				pCreateCallback = hCreateCallback->GetImpl();
			}

			m_pImpl->Render( *(hMesh->GetImpl()), detailLevel, matType, libIndex, *(hParamContainer->GetImpl()), *(hRenderContainer->GetImpl()), pCreateCallback ); 
		}
			
		MaterialGroupTemplate^ 
		ResourceMgr::CreateMaterialGroupTemplate(CoreManaged::CRCDataKey^ hKey, 
												CoreManaged::ByteBuffer^ hBuffer, 
												String^ hFileName)
		{
			pin_ptr<const wchar> pStr = PtrToStringChars(hFileName);
			IBFXMaterialGroupTemplate* pTemplate = 
				m_pImpl->CreateMaterialGroupTemplate(hKey->GetImpl(), hBuffer->GetImpl(), pStr);
			return gcnew MaterialGroupTemplate(pTemplate);
		}
			
		MaterialGroup^ 
		ResourceMgr::CreateMaterialGroup(CoreManaged::CRCDataKey^ hKey, 
										MaterialGroupTemplate^ hTemplate,
										array<EffectParam^>^ hConstantParams,
										array<uint>^ hSemantics)
		{
			uint semantics[128];
			REffectParam constantParams[128];
			assert(hConstantParams->Length == hSemantics->Length);
			assert(hConstantParams->Length < 128);
			
			_LOOPi(hConstantParams->Length)
			{
				semantics[i]		= hSemantics[i];
				constantParams[i]	= *(hConstantParams[i]->GetImpl());
			}
			
			IBFXMaterialGroup* pGroup = 
				m_pImpl->CreateMaterialGroup(hKey->GetImpl(), hTemplate->GetImpl(), 
					constantParams, semantics, hConstantParams->Length);
					
			return gcnew MaterialGroup(NULL);
		}
	}
}