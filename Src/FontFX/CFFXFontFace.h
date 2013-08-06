//==============================================================================
//
//		CFFXFontFace.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/11/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CFFXFontFace : public CRefCounted<IFFXFontFace>
{
	const static int MAX_NUM_FONTS_PER_SIDE = 32;
	const static int MAX_NUM_FONTS_PER_PAGE = MAX_NUM_FONTS_PER_SIDE * MAX_NUM_FONTS_PER_SIDE;
	const static int CACHE_SIZE = FFX_MAX_NUM_PAGES * MAX_NUM_FONTS_PER_PAGE;
	
	struct CachedSlot
	{
		// Cache data
		wchar	charType;
		byte	pageNum;
		uint	x;
		uint	y;
		
		// Glyph data
		FFXGlyphData data;
	};
		
	struct FontQuad
	{
		struct FontVertex
		{
			float x, y, z;
			uint color;
			float u, v; 
		};

		FontVertex vertices[4];		
	};
	
	typedef ObjectPool<CachedSlot, CACHE_SIZE, FALSE> CacheTracker;
	typedef std::vector<IRTexture2DPtr> TextureList;

private: 
	FT_Face			m_Face;
	CachedSlot*		m_pSlots[65536];
	CFFXFontFX*		m_pFontFX;
	CacheTracker	m_CacheTracker;
	TextureList		m_TextureCache;
	IByteBufferPtr	m_pFontData;	// Cannot destroy until all face data using this is destroyed
	
	uint m_Width;
	uint m_Height;	
	
	std::vector<FontQuad> m_BatchedQuads[FFX_MAX_NUM_PAGES];
	//int m_NumOfQuads[FFX_MAX_NUM_PAGES];
	
public: 
	CFFXFontFace(CFFXFontFX* pFontFX,
				 FT_Face& face,
				 IByteBuffer* pFontData,
				 uint width, 
				 uint height)
		: m_Face(face)
		, m_pFontFX(pFontFX)
		, m_Height(height)
		, m_Width(width)
		, m_pFontData(pFontData)
	{
		Initialize();
	}

	~CFFXFontFace()
	{
		FT_Done_Face(m_Face);
	}

private:
	void Initialize();
	void WriteToSlot(CachedSlot* pSlot);
	void AddQuadToVB(float* pToWrite, const gmtl::Vec4f& pos, const gmtl::Vec4f& uv, uint color);
	void RenderBatch();
	
public:
	//uint CacheText(const wchar* pStr, std::vector<FFXGlyphData*>& glyphDataStore); //FFXGlyphData** pGlyphDataStore);
	
	//IRTexture2D* GetCachedTexture(int pageNum) { return m_TextureCache[pageNum]; }
	
	void RenderString(const wchar* pStr, const gmtl::Vec2i& pos, uint color);
	void Flush();
	
	const FT_Face& GetFace() { return m_Face; }
	uint GetWidth() { return m_Width; }
	uint GetHeight() { return m_Height; }
};

_NAMESPACE_END
