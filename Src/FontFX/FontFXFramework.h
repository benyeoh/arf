//==============================================================================
//
//		FontFXFramework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/30/2007
//
//==============================================================================

#include "FontFX.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include FT_TRUETYPE_IDS_H

_NAMESPACE_BEGIN

class CFFXFontFX;

_NAMESPACE_END

#include <RendererUtils\RendererUtils.h>

#include "FontFXDefines.h"
#include "ParamSemanticDefs.h"
#include "FFXGlyphData.h"
#include "CFFXParams.h"
#include "CFFXFontFace.h"
#include "CFFXResourceMgr.h"
#include "CFFXFontFX.h"