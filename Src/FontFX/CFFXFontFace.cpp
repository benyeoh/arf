//==============================================================================
//
//		CFFXFontFace.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/22/2007
//
//==============================================================================

#include "FontFXFramework.h"

_NAMESPACE_BEGIN

//uint 
//CFFXFontFace::CacheText(const wchar* pStr, std::vector<FFXGlyphData*>& glyphDataStore)//FFXGlyphData** pGlyphDataStore)
//{
//	uint i = 0;
//	while(pStr[i])
//	{
//		CachedSlot* pSlot = m_pSlots[pStr[i]];
//		if(!pSlot)
//		{
//			pSlot = m_CacheTracker.Get();
//			
//			m_pSlots[pSlot->charType] = NULL;
//			pSlot->charType = pStr[i];
//			m_pSlots[pStr[i]] = pSlot;
//			
//			// Overwrite bitmap
//			WriteToSlot(pSlot);
//		}
//
//		m_CacheTracker.Preserve(pSlot);
//		glyphDataStore.push_back( &(pSlot->data));
////		pGlyphDataStore[i] = &(pSlot->data);		
//		++i;
//	}
//	
//	return i;
//}

void
CFFXFontFace::WriteToSlot(CachedSlot* pSlot)
{
	_DEBUG_ASSERT(pSlot->pageNum <= (byte) m_TextureCache.size());
	
	// Create a new texture page if the texture is not yet created
	if(pSlot->pageNum == (byte) m_TextureCache.size())
	{
		IRTexture2D* pTex = m_pFontFX->GetRenderer().GetRResourceMgr().CreateTexture2D(
			NULL, MAX_NUM_FONTS_PER_SIDE * m_Width, MAX_NUM_FONTS_PER_SIDE * m_Height, 1, TEXF_R8, TEXU_DEFAULT);

		_DEBUG_ASSERT(pTex);
		
		uint pitch = 0;
		byte* pData = pTex->Lock(0, pitch, NULL);
		memset(pData, 0, pTex->GetHeight(0) * pTex->GetWidth(0));
		pTex->Unlock(0);
		
		m_TextureCache.push_back(pTex);		
	}
	
	int glyphIndex = FT_Get_Char_Index(m_Face, pSlot->charType);
	
	int error = FT_Load_Glyph(m_Face, glyphIndex, FT_LOAD_DEFAULT);	
	_DEBUG_ASSERT(error == 0);
	
	error = FT_Render_Glyph(m_Face->glyph, FT_RENDER_MODE_NORMAL);
	_DEBUG_ASSERT(error == 0);
	
	IRTexture2D* pTex = m_TextureCache[pSlot->pageNum];
	
	uint rows = m_Face->glyph->bitmap.rows;
	uint cols = m_Face->glyph->bitmap.width;
	_DEBUG_ASSERT(rows <= m_Height);
	_DEBUG_ASSERT(cols <= m_Width);
	
	// Lock the required area
	uint pitch = 0;
	gmtl::Vec4i rect(pSlot->x, pSlot->y, m_Width, m_Height);
	byte* pData = pTex->Lock(0, pitch, &rect);

	// Write bitmap to the texture
	int bitmapPitch = gmtl::Math::abs(m_Face->glyph->bitmap.pitch);
	_LOOPi(rows)
	{
		_LOOPj(cols)
		{
			byte val = *(m_Face->glyph->bitmap.buffer + i * bitmapPitch + j);
			byte* pDest = (pData + (pSlot->y + i) * pitch + (pSlot->x + j));
			*pDest = val;
		}
	}
	
	pTex->Unlock(0);
	
	pSlot->data.uv[2] = pSlot->data.uv[0] + ((float)cols) / (MAX_NUM_FONTS_PER_SIDE * m_Width);
	pSlot->data.uv[3] = pSlot->data.uv[1] + ((float)rows) / (MAX_NUM_FONTS_PER_SIDE * m_Height);
	pSlot->data.size[0] = cols;
	pSlot->data.size[1] = rows;

	//pSlot->data.pageNum = pSlot->pageNum;
	pSlot->data.kerningOffsetX = m_Face->glyph->bitmap_left;
	pSlot->data.kerningOffsetY = -m_Face->glyph->bitmap_top;
	pSlot->data.advance = m_Face->glyph->advance.x >> 6;
}


void 
CFFXFontFace::Initialize()
{
	_LOOPi(65536)
	{
		m_pSlots[i] = NULL;
	}

	_LOOPi(CACHE_SIZE)
	{
		CachedSlot* pSlot = m_CacheTracker.Get();
		pSlot->charType = 0;
		pSlot->pageNum	= (byte) (i / MAX_NUM_FONTS_PER_PAGE);
		uint rowIndex	= ((i - (pSlot->pageNum * MAX_NUM_FONTS_PER_PAGE)) / MAX_NUM_FONTS_PER_SIDE);
		uint colIndex	= (i - (pSlot->pageNum * MAX_NUM_FONTS_PER_PAGE) - (rowIndex * MAX_NUM_FONTS_PER_SIDE));
		pSlot->y		= m_Height * rowIndex;
		pSlot->x		= m_Width * colIndex;
		
		pSlot->data.uv[0] = ((float)pSlot->x) / (MAX_NUM_FONTS_PER_SIDE * m_Width);
		pSlot->data.uv[1] = ((float)pSlot->y) / (MAX_NUM_FONTS_PER_SIDE * m_Height);
		pSlot->data.uv[2] = pSlot->data.uv[0] + ((float)m_Width) / (MAX_NUM_FONTS_PER_SIDE * m_Width);
		pSlot->data.uv[3] = pSlot->data.uv[1] + ((float)m_Height) / (MAX_NUM_FONTS_PER_SIDE * m_Height);
		
		pSlot->data.kerningOffsetX = 0;
		pSlot->data.kerningOffsetY = 0;
		
		pSlot->data.size[0] = m_Width;
		pSlot->data.size[1] = m_Height;
	}
}

void
CFFXFontFace::AddQuadToVB(float* pToWrite, const gmtl::Vec4f& pos, const gmtl::Vec4f& uv, uint color)
{
	// Top Left
	// Pos
	pToWrite[0] = pos[0];
	pToWrite[1] = pos[1];
	pToWrite[2] = 0.0f;

	// Color
	((uint*)pToWrite)[3] = color;

	// UV
	pToWrite[4] = uv[0];
	pToWrite[5] = uv[1];

	// Top Right
	// Pos
	pToWrite[6] = pos[2];
	pToWrite[7] = pos[1];
	pToWrite[8] = 0.0f;

	// Color
	((uint*)pToWrite)[9] = color;

	// UV
	pToWrite[10] = uv[2];
	pToWrite[11] = uv[1];			

	// Bottom Left
	// Pos
	pToWrite[12] = pos[0];
	pToWrite[13] = pos[3];
	pToWrite[14] = 0.0f;

	// Color
	((uint*)pToWrite)[15] = color;

	// UV
	pToWrite[16] = uv[0];
	pToWrite[17] = uv[3];			

	// Bottom Right
	// Pos
	pToWrite[18] = pos[2];
	pToWrite[19] = pos[3];
	pToWrite[20] = 0.0f;

	// Color
	((uint*)pToWrite)[21] = color;

	// UV
	pToWrite[22] = uv[2];
	pToWrite[23] = uv[3];
}

void 
CFFXFontFace::RenderBatch()
{
	IRVertexBuffer* pVB = m_pFontFX->GetVB();
	IRIndexBuffer* pIB = m_pFontFX->GetIB();
	
	int i = 0;
	while(i < FFX_MAX_NUM_PAGES)
	{
		uint numGlyphs = //m_NumOfQuads[i]; 
			(uint) m_BatchedQuads[i].size();
		if(numGlyphs > 0)
		{		
			IRTexture2D* pCurTex = m_TextureCache[i];

			byte* pData = NULL;
			uint curOffset = pVB->GetStartVertex() + pVB->GetNumVertices();
			uint sizeLeft = pVB->GetSize() - curOffset;
			if(numGlyphs*4 > sizeLeft)
			{
				sizeLeft += curOffset;
				curOffset = 0;
			}

			_DEBUG_ASSERT(curOffset % 4 == 0);
			_DEBUG_ASSERT(sizeLeft % 4 == 0);

			uint numGlyphsToDraw = gmtl::Math::Min(numGlyphs, sizeLeft/4);
			pData = pVB->Lock(curOffset, numGlyphsToDraw*4);

			FontQuad* pQuad = &(m_BatchedQuads[i][numGlyphs-numGlyphsToDraw]);
			memcpy(pData, pQuad, sizeof(FontQuad)*numGlyphsToDraw);

			pVB->Unlock();
			pIB->SetValidRange(0, numGlyphsToDraw*6);

			m_pFontFX->GetParams().SetFontTexture(pCurTex);
			m_pFontFX->RenderImmediate();

			//m_NumOfQuads[i] -= numGlyphsToDraw;
			m_BatchedQuads[i].erase(m_BatchedQuads[i].begin()+(numGlyphs-numGlyphsToDraw), m_BatchedQuads[i].end());

			if(m_BatchedQuads[i].size() > 0)
				//if(m_NumOfQuads[i] > 0)
				i--;
		}
		
		i++;
	}
}

void 
CFFXFontFace::RenderString(const wchar* pStr, const gmtl::Vec2i& pos, uint color)
{
	const RDisplayInfo& info = m_pFontFX->GetRenderer().GetDisplayInfo();
	const RRendererInfo& rendererInfo = m_pFontFX->GetRenderer().GetRendererInfo();

	//CFFXFontFace* pFFXFontFace = m_pCurFace;
	//pFFXFontFace->CacheText(pStr, m_GlyphData);

	int currentPenX = pos[0];
	int currentPenY = pos[1];

	gmtl::Vec4f vertexPos;
	FontQuad quad;

//	uint size = m_GlyphData.size();

	//_LOOPi(FFX_MAX_NUM_PAGES)
	//{
	//	if(m_BatchedQuads[i].size() < size)
	//		m_BatchedQuads[i].resize(size);
	//}

	float backBufferWidthOffset = 2.0f / info.backBufferWidth;
	float backBufferHeightOffset = -2.0f / info.backBufferHeight;

	uint i = 0;
	while(pStr[i])
	{
		CachedSlot* pSlot = m_pSlots[pStr[i]];
		if(!pSlot)
		{
			pSlot = m_CacheTracker.Get();

			m_pSlots[pSlot->charType] = NULL;
			pSlot->charType = pStr[i];
			m_pSlots[pStr[i]] = pSlot;

			// Overwrite bitmap
			WriteToSlot(pSlot);
		}

		m_CacheTracker.Preserve(pSlot);
		//glyphDataStore.push_back( &(pSlot->data));
		//		pGlyphDataStore[i] = &(pSlot->data);		
		
		uint pageNum = pSlot->pageNum;

		_DEBUG_ASSERT(pageNum < FFX_MAX_NUM_PAGES);
		_DEBUG_ASSERT(pageNum >= 0);

		FFXGlyphData* pGlyph = &(pSlot->data);

		float offsetX = (currentPenX + pGlyph->kerningOffsetX - rendererInfo.pixelOffset);
		float offsetY = (currentPenY + pGlyph->kerningOffsetY - rendererInfo.pixelOffset);

		vertexPos[0] = offsetX * backBufferWidthOffset - 1.0f;
		vertexPos[1] = offsetY * backBufferHeightOffset + 1.0f;
		vertexPos[2] = (offsetX + pSlot->data.size[0]) * backBufferWidthOffset - 1.0f;
		vertexPos[3] = (offsetY + pSlot->data.size[1]) * backBufferHeightOffset + 1.0f;

		AddQuadToVB((float*)&quad, vertexPos, pSlot->data.uv, color);
		//AddQuadToVB((float*)&(m_BatchedQuads[pageNum][m_NumOfQuads[pageNum]]), vertexPos, pGlyph->uv, color);
		//m_NumOfQuads[pageNum]++;

		m_BatchedQuads[pageNum].push_back(quad);

		// Advance pen position
		currentPenX += pSlot->data.advance;		
		
		++i;
	}

	//_LOOPi(size)
	//{
	//	uint pageNum = m_GlyphData[i]->pageNum;

	//	_DEBUG_ASSERT(pageNum < FFX_MAX_NUM_PAGES);
	//	_DEBUG_ASSERT(pageNum >= 0);

	//	FFXGlyphData* pGlyph = m_GlyphData[i];

	//	float offsetX = (currentPenX + pGlyph->kerningOffsetX - rendererInfo.pixelOffset);
	//	float offsetY = (currentPenY + pGlyph->kerningOffsetY - rendererInfo.pixelOffset);

	//	vertexPos[0] = offsetX * backBufferWidthOffset - 1.0f;
	//	vertexPos[1] = offsetY * backBufferHeightOffset + 1.0f;
	//	vertexPos[2] = (offsetX + pGlyph->size[0]) * backBufferWidthOffset - 1.0f;
	//	vertexPos[3] = (offsetY + pGlyph->size[1]) * backBufferHeightOffset + 1.0f;

	//	AddQuadToVB((float*)&quad, vertexPos, pGlyph->uv, color);
	//	//AddQuadToVB((float*)&(m_BatchedQuads[pageNum][m_NumOfQuads[pageNum]]), vertexPos, pGlyph->uv, color);
	//	//m_NumOfQuads[pageNum]++;

	//	m_BatchedQuads[pageNum].push_back(quad);

	//	// Advance pen position
	//	currentPenX += pGlyph->advance;		
	//}

	//m_GlyphData.clear();
}

void 
CFFXFontFace::Flush()
{
	RenderBatch();
}

_NAMESPACE_END