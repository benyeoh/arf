//==============================================================================
//
//		RendererFramework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
//
//==============================================================================

#ifndef _RENDERERFRAMEWORK_H
#define _RENDERERFRAMEWORK_H

#include "Renderer.h"

#ifndef _NO_RENDERER_METRICS
	#define _INC_METRIC_NUM(renderer, metric, num) renderer->GetMetricsInternal().metric+=num
#else
	#define _INC_METRIC_NUM(renderer, metric, num) (void*)0;
#endif
	
#define _INC_METRIC(renderer, metric) _INC_METRIC_NUM(renderer, metric, 1)

// Additional implementation includes
_NAMESPACE_BEGIN

class CRRenderer;
class CREffect;
class CRTexture2D;
class CRTexture3D;
class CRTextureRT;
class CRTextureCube;
class CRDepthBuffer;
class CRVertexBuffer;
class CRVertexBufferGroup;
class CRIndexBuffer;
class CRResourceMgr;

const static uint MAX_VERTEX_DESC = 64;

inline uint 
GetUsageSize(eRVDUsage usage)
{
	switch( usage )
	{
	case VDU_POSITION:		return sizeof(float)*3;
	case VDU_NORMAL:		return sizeof(float)*3;
	case VDU_BINORMAL:		return sizeof(float)*3;	
	case VDU_TANGENT:		return sizeof(float)*3;
	case VDU_BLENDINDICES:	return sizeof(byte)*4;	
	case VDU_BLENDWEIGHT:	return sizeof(float)*4;					
	case VDU_COLOR:			return sizeof(byte)*4;
	case VDU_TEXCOORDF1:		return sizeof(float);
	case VDU_TEXCOORDF2:		return sizeof(float)*2;
	case VDU_TEXCOORDF3:		return sizeof(float)*3;				
	case VDU_TEXCOORDF4:		return sizeof(float)*4;				

	default: _DEBUG_ASSERT( FALSE );
	}

	return 0;
}

inline uint 
GetVertexDataSize(const RVertexDesc* pDesc)
{
	uint i = 0;
	uint size = 0;
	while(pDesc[i].usage != VDU_END)
	{
		size += GetUsageSize(pDesc[i].usage);		
		++i;
	}
	
	return size;
}

_NAMESPACE_END

#include "RenderOpPool.h"

#include "CRResource.h"
#include "CREffectTemplate.h"
#include "CREffect.h"
#include "CRTexture.h"
#include "CRTexture2D.h"
#include "CRTexture3D.h"
#include "CRTextureRT.h"
#include "CRTextureCube.h"
#include "CRDepthBuffer.h"
#include "CRVertexBuffer.h"
#include "CRVertexBufferGroup.h"
#include "CRIndexBuffer.h"
#include "CRResourceMgr.h"
//#include "CRPipeline.h"
#include "CRRenderer.h"
#include "CRRenderGroup.h"

#endif
