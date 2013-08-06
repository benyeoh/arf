//==============================================================================
//
//		CRTextureD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#include "RendererD3D.h"


_NAMESPACE_BEGIN

void 
CRTextureD3D::SetD3DTexture(IDirect3DTexture9* pTex)
{
	m_pD3DTexture = pTex;
}

//void
//CRTextureD3D::SetD3DDescription(boolean isManaged)
//{
//	m_IsManaged = isManaged;
//}

void 
CRTextureD3D::OnResetDevice()
{
	//if(!m_IsManaged)
	{
		_DEBUG_TRACE(_W("Resetting texture: %s\n"), GetResourceName());

		boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetTexture(this);
		_DEBUG_ASSERT(success);
	}
}

void 
CRTextureD3D::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost texture: %s\n"), GetResourceName());

	m_pD3DTexture = NULL;
}

void 
CRTextureD3D::OnRestoreDevice()
{
}


_NAMESPACE_END