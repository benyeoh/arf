//==============================================================================
//
//		CSEFXSHExpLightingFX.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#include "SHExpLightingFXFramework.h"

_NAMESPACE_BEGIN

boolean 
CSEFXSHExpLightingFX::Initialize(IDEFXDeferredFX& deferredFX, IBFXBaseFX& baseFX, IRRenderer& renderer, IAppCallback& callback)
{
	m_pDeferredFX	= &deferredFX;
	m_pBaseFX		= &baseFX;
	m_pRenderer		= &renderer;
	m_pAppCallback	= &callback;

	m_ParamPool.SetSHExpLightingFX(this);
	m_ResMgr.SetSHExpLightingFX(this);
	m_Pipeline.SetSHExpLightingFX(this);
	
	m_ParamPool.Initialize();
	m_Pipeline.Initialize();

	m_pBaseFX->SetParamCallback(&m_ParamPool, SEFX_DYNAMIC_SEMANTIC_BEGIN,
		SEFX_DYNAMIC_SEMANTIC_END - SEFX_DYNAMIC_SEMANTIC_BEGIN + 1);
	m_pBaseFX->SetRenderCallback(&m_Pipeline, SEFX_MAT_COLLAPSE_LIGHT);
	m_pBaseFX->SetRenderCallback(&m_Pipeline, SEFX_MAT_OCCLUDER_ACCUM);
	m_pBaseFX->SetRenderCallback(&m_Pipeline, SEFX_MAT_STENCIL_VOLUME);

	return TRUE;
}


_NAMESPACE_END
