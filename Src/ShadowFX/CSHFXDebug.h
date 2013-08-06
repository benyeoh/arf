//==============================================================================
//
//		CSHFXDebug.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/23/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSHFXDebug : public ISHFXDebug
{	
private:
	CSHFXShadowFX*			m_pShadowFX;
	
	IREffectPtr				m_p2DPipeSetupEffect;
	IREffectPtr				m_pSMDebugEffect;
	IRVertexBufferGroupPtr	m_pQuadVBGroup;
	IRIndexBufferPtr		m_pQuadIB;

	//gmtl::Vec4f				m_Offsets[4];
	gmtl::Vec4f				m_Offsets;

	IRRenderGroupPtr		m_pDebugGroup;

public: 
	CSHFXDebug()
		: m_pShadowFX(NULL)
	{
	}

	virtual ~CSHFXDebug()
	{
	}

public: 
	void SetShadowFX(CSHFXShadowFX* pShadowFX) { m_pShadowFX = pShadowFX; }
	void Initialize();

	void SetViewport(uint left, uint top, uint width, uint height);
	void AddRenderTarget(IRTextureRT* pTexRT);
	void ClearRenderTargets();
	void SetDepthBuffer(IRDepthBuffer* pDepth);

	void RenderCascSM(const gmtl::Vec4f& screenSpacePos, const IRTexture2D* pCascSM);
	void RenderPLSM(const gmtl::Vec4f& screenSpacePos, const IRTexture2D* pPLSM);
};

_NAMESPACE_END
