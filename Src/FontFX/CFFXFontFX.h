//==============================================================================
//
//		CFFXFontFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/8/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CFFXFontFX : public CRefCounted<IFFXFontFX>
{
private: 
	FT_Library m_Library;
	
	CFFXResourceMgr m_ResMgr;
	CFFXParams m_Params;
	
	IRRendererPtr m_pRenderer;
	//IRFXRenderFXPtr m_pRenderFX;
	//std::vector<FFXGlyphData*> m_GlyphData;
	
	IRVertexBufferGroupPtr m_pVBGroup;
	IRVertexBufferPtr m_pVB;
	IRIndexBufferPtr m_pIB;
	IREffectPtr m_pFontEffect;
	IAppCallback* m_pAppCallback;
	
	IRRenderGroupPtr	m_pRenderGroup;

	//std::vector<FontQuad> m_BatchedQuads[FFX_MAX_NUM_PAGES];
	//int m_NumOfQuads[FFX_MAX_NUM_PAGES];
	
	//CFFXFontFace* m_pCurFace;
	
public: 
	CFFXFontFX()
		: m_pAppCallback(NULL)
		//, m_pCurFace(NULL)
	{
		m_ResMgr.SetFontFX(this);
		//_LOOPi(FFX_MAX_NUM_PAGES)
		//	m_NumOfQuads[i] = 0;
	}

	virtual ~CFFXFontFX()
	{
		FT_Done_FreeType(m_Library);
	}
	
private:
	//void AddQuadToVB(float* pToWrite, const gmtl::Vec4f& pos, const gmtl::Vec4f& uv, uint color);
	//void RenderBatch();

public: 
	boolean Initialize(IRRenderer& renderer, IAppCallback& callback);

	//void BeginRender(IFFXFontFace* pFontFace);
	//void Render(const wchar* pStr, const gmtl::Vec2i& pos, uint color);
	//void EndRender();
	
	FT_Library* GetFTLibrary();
	
	IRRenderer& GetRenderer()	{ return *m_pRenderer; }
	
	CFFXResourceMgr& GetResourceMgr() { return m_ResMgr; }
	CFFXParams& GetParams() { return m_Params; }

	void SetViewport(uint left, uint top, uint width, uint height);
	void AddRenderTarget(IRTextureRT* pTexRT);
	void ClearRenderTargets();
	
	void RenderImmediate();	
	
	IRVertexBuffer* GetVB() { return m_pVB; }
	IRIndexBuffer* GetIB() { return m_pIB; }	
};

_NAMESPACE_END
