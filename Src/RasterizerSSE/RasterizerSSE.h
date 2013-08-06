//================================================================================
//
//		RasterizerSSE.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/12/2011
//
//================================================================================

#ifndef _RASTERIZERSSE_H
#define _RASTERIZERSSE_H

#include <Core\Core.h>

#include <xmmintrin.h>
#include <emmintrin.h>

//#include "Defines.h"
#include "RasterInfo.h"

//#include <Rasterizer\Rasterizer.h>

//#include "TriangleSetup.h"
//#include "RasterizeTiles.h"
//#pragma optimize("", off)

#define _RASTERIZER_SSE_INLINE	__forceinline

//#undef _mm_rcp_ss
//#undef _mm_rcp_ps
//
//#define _mm_rcp_ss(v) _mm_div_ss( _mm_set_ss(1.0f), v )
//#define _mm_rcp_ps(v) _mm_div_ps( _mm_set_ps1(1.0f), v )

#include "TriangleSetup.h"
#include "TilesRasterize.h"
#include "TilesEdgeCheck.h"
#include "TriangleBoundingBox.h"

#endif
