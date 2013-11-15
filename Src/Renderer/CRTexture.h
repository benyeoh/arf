//==============================================================================
//
//		CRTexture2D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, March 25, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

template <typename _INHERITED>
class CRTexture : public CRResource< _INHERITED >
{
protected:
	eRTexUsage m_Usage;
	eRTexFormat m_Format;
	wstring m_FileName;
	
protected:
	CRRenderer* m_pRenderer;
	
public:
	CRTexture(CRRenderer* pRenderer)
		: m_pRenderer(pRenderer)
		, m_Usage(TEXU_END)
		, m_Format(TEXF_END)
	{
		m_FileName = _W("");
	}
	
	virtual ~CRTexture()
	{
	}

protected:
	_PURE( uint DoGetMipMapLevels() const )
		
public:
	uint GetSizePerElem() const 
	{
		switch(m_Format)
		{
			case TEXF_R8: return 1;

			case TEXF_A8R8G8B8:
			case TEXF_R32F:		
			case TEXF_G16R16F:
			case TEXF_G16R16:
			case TEXF_X8R8G8B8: return 4;

			// Compressed textures effectively per block rather than per pixel
			case TEXF_DXT1:	return 8;		
			case TEXF_DXT2:
			case TEXF_DXT3:
			case TEXF_DXT4:
			case TEXF_DXT5:	return 16;
			
			case TEXF_A32B32G32R32F:	return 16;
			case TEXF_G32R32F:			return 8;
			case TEXF_A16B16G16R16F:	return 8;
            case TEXF_A16B16G16R16:	    return 8;
		}
		
		return 0;
	}
	
	eRTexUsage GetUsage() const { return m_Usage; }
	eRTexFormat GetFormat() const { return m_Format; }
	
	void SetFormat(eRTexFormat format) { m_Format = format; }
	void SetUsage(eRTexUsage usage) { m_Usage = usage; }		
	
	void SetFileName(const wchar* pName) { m_FileName = pName; }
	const wchar* GetFileName() const { return m_FileName.c_str(); }

	uint GetMipMapLevels() const
	{
		return DoGetMipMapLevels();
	}
};

_NAMESPACE_END