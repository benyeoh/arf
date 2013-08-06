//==============================================================================
//
//		CFFXResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/8/2007
//
//==============================================================================

#include "FontFXFramework.h"

_NAMESPACE_BEGIN

IFFXFontFace* 
CFFXResourceMgr::CreateFontFace(IByteBuffer* pFontData, uint pointSize, uint faceIndex)
{
	FT_Face face;

	int error = FT_New_Memory_Face( 
		*(m_pFontFX->GetFTLibrary()), 
		pFontData->GetData(), 
		pFontData->GetDataLength(),
		faceIndex,
		&face);

	// int error = FT_New_Face( *(m_pFontFX->GetFTLibrary()), "E:\\Dev\\X\\Data\\Font\\arial.ttf", faceIndex, &face );
		
	if(error)		
		return NULL;
	
	uint pixelSizeX = (uint) ((pointSize / 72.0f) * 96.0f);	
	uint pixelSizeY = (uint) ((pointSize / 72.0f) * 96.0f);
	error = FT_Set_Pixel_Sizes(
		face,		
		pixelSizeX,
		pixelSizeY);
	
	if(error)
		return NULL;

	error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	_DEBUG_ASSERT(!error);
		
	//FT_CharMap unicodeMap = 0;
	//_LOOPi(face->num_charmaps)
	//{
	//	FT_CharMap curMap = face->charmaps[i];
	//	if(curMap->platform_id == TT_PLATFORM_MICROSOFT &&
	//		curMap->encoding_id == TT_MS_ID_UNICODE_CS)
	//	{
	//		unicodeMap = curMap;
	//		break;
	//	}
	//}
	//
	//_DEBUG_ASSERT(unicodeMap);
	//if(unicodeMap)
	//{
	//	error = FT_Set_Charmap(face, unicodeMap);
	//	_DEBUG_ASSERT(!error);
	//}
	//
	
	// Create face object
	// The extra 1 pixel size is to make sure bilinear filtering doesn't sample across boundaries in the cache
	CFFXFontFace* pFontFace = _NEW CFFXFontFace(m_pFontFX, face, pFontData, pixelSizeX+1, pixelSizeY+1);
	return pFontFace;
}



_NAMESPACE_END