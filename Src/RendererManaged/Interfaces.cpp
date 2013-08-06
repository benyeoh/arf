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

#include "RendererManaged.h"

namespace ARFManaged
{
	namespace RendererManaged
	{
		bool
		Renderer::Initialize(InitInfo hInfo)
		{
			RInitInfo init;
			init.pApplication = hInfo.hApplication->GetImpl();
			init.presentInfo.backBufferHeight = hInfo.presentInfo.backBufferHeight;
			init.presentInfo.backBufferWidth = hInfo.presentInfo.backBufferWidth;
			init.presentInfo.fullScreen = hInfo.presentInfo.fullScreen;
			init.presentInfo.hWnd = (HWND) hInfo.presentInfo.hWnd.ToPointer();
			
			m_pImpl->Initialize(init);
			
			return true;
		}

		bool Renderer::SetDisplayInfo(DisplayInfo hInfo)
		{
			RDisplayInfo displayInfo;
			displayInfo.hWnd = (HWND) hInfo.hWnd.ToPointer();
			displayInfo.backBufferWidth = hInfo.backBufferWidth;
			displayInfo.backBufferHeight = hInfo.backBufferHeight;
			displayInfo.fullScreen = hInfo.fullScreen;

			return m_pImpl->SetDisplayInfo(displayInfo) ? true : false;
		}

		DisplayInfo Renderer::GetDisplayInfo()
		{
			const RDisplayInfo& displayInfo = GetImpl()->GetDisplayInfo();
			DisplayInfo toReturn;
			toReturn.hWnd = IntPtr(displayInfo.hWnd);
			toReturn.backBufferWidth = displayInfo.backBufferWidth;
			toReturn.backBufferHeight = displayInfo.backBufferHeight;
			toReturn.fullScreen = displayInfo.fullScreen ? true : false;

			return toReturn;
		}
		
		Effect^ 
		ResourceMgr::LoadEffect(CoreManaged::CRCDataKey^ hKey,
			CoreManaged::ByteBuffer^ hBuffer,
			array<EffectParam^>^ hConstantParams,
			array<uint>^ hSemantics)
		{
			REffectParam pParams[256];
			uint length = hConstantParams != nullptr ? hConstantParams->Length : 0;			
			_LOOPi(length)
			{
				pParams[i] = *(hConstantParams[i]->GetImpl());
			}
			
			IREffect* pEffect = NULL;
			
			CRCDataKey* pDataKey = hKey ? hKey->GetImpl() : NULL;
			
			if(length > 0)
			{
				pin_ptr<uint> pSemantics = &hSemantics[0]; 					
				pEffect = m_pImpl->LoadEffect(
					pDataKey, hBuffer->GetImpl(), pParams, pSemantics, length);			
			}
			else
			{
				pEffect = m_pImpl->LoadEffect(
					pDataKey, hBuffer->GetImpl(), NULL, NULL, 0);	
			}
			
			if(pEffect)
			{
				return gcnew Effect(pEffect);
			}
			
			return nullptr;				
		}

	}
}