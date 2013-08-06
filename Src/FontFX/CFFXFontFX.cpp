//==============================================================================
//
//		CFFXFontFX.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/8/2007
//
//==============================================================================

#include "FontFXFramework.h"

_NAMESPACE_BEGIN

//struct GlyphSortLess
//{
//	inline bool operator()(	const FFXGlyphData* pLhs, const FFXGlyphData* pRhs ) const
//	{
//		return ( pLhs->pTex < pRhs->pTex );
//	}
//};

const static uint MAX_STRING_SIZE = 512;

boolean
CFFXFontFX::Initialize(IRRenderer& renderer, IAppCallback& callback)
{
	if(FT_Init_FreeType(&m_Library) != 0)
	{
		return FALSE;
	}
	
	//m_pRenderFX = &renderFX;
	m_pRenderer = &renderer;
	m_pAppCallback = &callback;
	
	RVertexDesc desc[] = 
	{
		{ VDU_POSITION, 0 },
		{ VDU_COLOR, 0 },
		{ VDU_TEXCOORDF2, 0 },
		{ VDU_END, 0 },
	};

	m_pVB = m_pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, desc, 4 * MAX_STRING_SIZE, BUF_DYNAMIC);
	m_pIB = m_pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, 6 * MAX_STRING_SIZE, BUF_DEFAULT);
	m_pVBGroup = m_pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);

	m_pVBGroup->AddVertexBuffer(m_pVB);
	
	ushort* pIndices = m_pIB->Lock(0, 6 * MAX_STRING_SIZE);
	_DEBUG_ASSERT(pIndices);
	_LOOPi(MAX_STRING_SIZE)
	{
		pIndices[i*6] = i*4+0;
		pIndices[i*6+1] = i*4+1;
		pIndices[i*6+2] = i*4+2;
		pIndices[i*6+3] = i*4+1;
		pIndices[i*6+4] = i*4+3;
		pIndices[i*6+5] = i*4+2;
	}
	m_pIB->Unlock();
	
	CRCDataKey id;
	wstring fileName = _W("[shd]\\FontFX\\ScreenSpaceFont.fx");
	IByteBufferPtr pFontEffectBuffer = m_pAppCallback->GetFileData(fileName.c_str());
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));	
	m_pFontEffect = m_pRenderer->GetRResourceMgr().LoadEffect(&id, pFontEffectBuffer, NULL, NULL, 0);
	
	m_pRenderGroup = m_pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);

	return TRUE;
}

FT_Library* 
CFFXFontFX::GetFTLibrary()
{
	return &m_Library;
}

//void
//CFFXFontFX::AddQuadToVB(float* pToWrite, const gmtl::Vec4f& pos, const gmtl::Vec4f& uv, uint color)
//{
//	// Top Left
//	// Pos
//	pToWrite[0] = pos[0];
//	pToWrite[1] = pos[1];
//	pToWrite[2] = 0.0f;
//
//	// Color
//	((uint*)pToWrite)[3] = color;
//
//	// UV
//	pToWrite[4] = uv[0];
//	pToWrite[5] = uv[1];
//
//	// Top Right
//	// Pos
//	pToWrite[6] = pos[2];
//	pToWrite[7] = pos[1];
//	pToWrite[8] = 0.0f;
//
//	// Color
//	((uint*)pToWrite)[9] = color;
//
//	// UV
//	pToWrite[10] = uv[2];
//	pToWrite[11] = uv[1];			
//
//	// Bottom Left
//	// Pos
//	pToWrite[12] = pos[0];
//	pToWrite[13] = pos[3];
//	pToWrite[14] = 0.0f;
//
//	// Color
//	((uint*)pToWrite)[15] = color;
//
//	// UV
//	pToWrite[16] = uv[0];
//	pToWrite[17] = uv[3];			
//
//	// Bottom Right
//	// Pos
//	pToWrite[18] = pos[2];
//	pToWrite[19] = pos[3];
//	pToWrite[20] = 0.0f;
//
//	// Color
//	((uint*)pToWrite)[21] = color;
//
//	// UV
//	pToWrite[22] = uv[2];
//	pToWrite[23] = uv[3];
//}

//void 
//CFFXFontFX::RenderBatch()
//{
//	REffectParam effectParams[RRenderOp::MAX_NUM_PARAMS];
//
//	_LOOPi(FFX_MAX_NUM_PAGES)
//	{
//		uint numGlyphs = //m_NumOfQuads[i]; 
//			m_BatchedQuads[i].size();
//		if(numGlyphs > 0)
//		{		
//			IRTexture2D* pCurTex = m_pCurFace->GetCachedTexture(i);
//			
//			byte* pData = NULL;
//			uint curOffset = m_pVB->GetStartVertex() + m_pVB->GetNumVertices();
//			uint sizeLeft = m_pVB->GetSize() - curOffset;
//			if(numGlyphs*4 > sizeLeft)
//			{
//				sizeLeft += curOffset;
//				curOffset = 0;
//			}
//			
//			_DEBUG_ASSERT(curOffset % 4 == 0);
//			_DEBUG_ASSERT(sizeLeft % 4 == 0);
//	
//			uint numGlyphsToDraw = gmtl::Math::Min(numGlyphs, sizeLeft/4);
//			pData = m_pVB->Lock(curOffset, numGlyphsToDraw*4);
//					
//			FontQuad* pQuad = &(m_BatchedQuads[i][numGlyphs-numGlyphsToDraw]);
//			memcpy(pData, pQuad, sizeof(FontQuad)*numGlyphsToDraw);
//			
//			m_pVB->Unlock();
//			m_pIB->SetValidRange(0, numGlyphsToDraw*6);
//			
//			m_Params.SetFontTexture(pCurTex);
//			uint tech = m_Params.InitializeEffect(*m_pFontEffect, 1.0f, effectParams);
//			uint groupIndex = m_pRenderer->GetRPipeline().CreateNewRenderGroup(FFX_GROUP_IMMEDIATE, NULL, *this);
//			m_pRenderer->GetRPipeline().AddToRenderGroup(groupIndex, m_pVBGroup, m_pIB, m_pFontEffect, effectParams, tech, NULL);
//			m_pRenderer->GetRPipeline().RenderImmediate(groupIndex);
//
//			//m_NumOfQuads[i] -= numGlyphsToDraw;
//			m_BatchedQuads[i].erase(m_BatchedQuads[i].begin()+(numGlyphs-numGlyphsToDraw), m_BatchedQuads[i].end());
//			
//			if(m_BatchedQuads[i].size() > 0)
//			//if(m_NumOfQuads[i] > 0)
//				i--;
//		}
//	}
//}

//void 
//CFFXFontFX::BeginRender(IFFXFontFace* pFontFace)
//{
//	_DEBUG_ASSERT(!m_pCurFace && pFontFace);
//	m_pCurFace = (CFFXFontFace*) pFontFace;
//}
//
//void 
//CFFXFontFX::Render(const wchar* pStr, const gmtl::Vec2i& pos, uint color)
//{
//	const RDisplayInfo& info = m_pRenderer->GetDisplayInfo();
//	const RRendererInfo& rendererInfo = m_pRenderer->GetRendererInfo();
//
//	CFFXFontFace* pFFXFontFace = m_pCurFace;
//	pFFXFontFace->CacheText(pStr, m_GlyphData);
//
//	int currentPenX = pos[0];
//	int currentPenY = pos[1];
//
//	gmtl::Vec4f vertexPos;
//	FontQuad quad;
//
//	uint size = m_GlyphData.size();
//	
//	//_LOOPi(FFX_MAX_NUM_PAGES)
//	//{
//	//	if(m_BatchedQuads[i].size() < size)
//	//		m_BatchedQuads[i].resize(size);
//	//}
//	
//	float backBufferWidthOffset = 2.0f / info.backBufferWidth;
//	float backBufferHeightOffset = -2.0f / info.backBufferHeight;
//
//	_LOOPi(size)
//	{
//		uint pageNum = m_GlyphData[i]->pageNum;
//				
//		_DEBUG_ASSERT(pageNum < FFX_MAX_NUM_PAGES);
//		_DEBUG_ASSERT(pageNum >= 0);
//
//		FFXGlyphData* pGlyph = m_GlyphData[i];
//				
//		float offsetX = (currentPenX + pGlyph->kerningOffsetX - rendererInfo.pixelOffset);
//		float offsetY = (currentPenY + pGlyph->kerningOffsetY - rendererInfo.pixelOffset);
//		
//		vertexPos[0] = offsetX * backBufferWidthOffset - 1.0f;
//		vertexPos[1] = offsetY * backBufferHeightOffset + 1.0f;
//		vertexPos[2] = (offsetX + pGlyph->size[0]) * backBufferWidthOffset - 1.0f;
//		vertexPos[3] = (offsetY + pGlyph->size[1]) * backBufferHeightOffset + 1.0f;
//
//		AddQuadToVB((float*)&quad, vertexPos, pGlyph->uv, color);
//		//AddQuadToVB((float*)&(m_BatchedQuads[pageNum][m_NumOfQuads[pageNum]]), vertexPos, pGlyph->uv, color);
//		//m_NumOfQuads[pageNum]++;
//		
//		m_BatchedQuads[pageNum].push_back(quad);
//
//		// Advance pen position
//		currentPenX += pGlyph->advance;		
//	}
//
//	m_GlyphData.clear();
//}

//void 
//CFFXFontFX::EndRender()
//{
//	RenderBatch();
//	m_pCurFace = NULL;
//}

void CFFXFontFX::RenderImmediate()
{
	REffectParam effectParams[RRenderOp::MAX_NUM_PARAMS];
	
	uint tech = m_Params.InitializeEffect(*m_pFontEffect, 1.0f, effectParams);

	RVarData var;
	var.iVal = 0;
	m_pRenderGroup->AddDrawOp(m_pVBGroup, m_pIB, m_pFontEffect, effectParams, tech, var); 
	m_pRenderGroup->Flush();
	m_pRenderGroup->Reset();	
}

void CFFXFontFX::SetViewport(uint left, uint top, uint width, uint height)
{
	m_pRenderGroup->SetViewport(left, top, width, width);
}

void CFFXFontFX::AddRenderTarget(IRTextureRT* pTexRT)
{
	m_pRenderGroup->AddRenderTarget(pTexRT);
}

void CFFXFontFX::ClearRenderTargets()
{
	m_pRenderGroup->ClearRenderTargets();
}


_NAMESPACE_END