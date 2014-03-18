//==============================================================================
//
//		CRDepthBufferD3D11.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 9 March, 2014
//
//==============================================================================

#include "RendererD3D11Framework.h"

_NAMESPACE_BEGIN

void CRTexture2DD3D11::SetD3D11Texture(CRTexture2DD3D11* pTex) 
{ 
	m_pDepthTexture = pTex; 
}

CRTexture2DD3D11* CRTexture2DD3D11::GetD3D11Texture() 
{ 
	return m_pDepthTexture; 
}

_NAMESPACE_END