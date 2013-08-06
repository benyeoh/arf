//==============================================================================
//
//		ShaderDefs.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Friday, January 05, 2007
//
//==============================================================================

#ifndef _SHADERDEFS
#define _SHADERDEFS

#define _DECL_EFFECT_TYPE int EffectType \
	< int semantic = RFX_EFFECT_TYPE; int usage = 1; bool always_used = true; >

#define _DECL_EFFECT_DESC string EffectDesc \
	< int semantic = RFX_EFFECT_TEXT_DESC; int usage = 1; bool always_used = true; >
	
#define _DECL_VERTEX_DESC(i) int EffectVertexDesc##i \
	< int semantic = RFX_EFFECT_VERTEX_DESC##i; int usage = 1; bool always_used = true; >				

// Vertex data desc
const static int VDU_POSITION		= 0x00001000;
const static int VDU_NORMAL			= 0x00002000;
const static int VDU_TEXCOORDF		= 0x00004000;
const static int VDU_BINORMAL		= 0x00008000;
const static int VDU_TANGENT		= 0x00010000;
const static int VDU_BLENDINDICES	= 0x00020000;
const static int VDU_BLENDWEIGHT	= 0x00040000;
const static int VDU_COLOR			= 0x00080000;

const static int VDU_TEXCOORDF2		= 0x00004000;
const static int VDU_TEXCOORDF1		= 0x00004100;
const static int VDU_TEXCOORDF3		= 0x00004200;
const static int VDU_TEXCOORDF4		= 0x00004400;

const static int VDU_END			= 0x80000000;

// Parameters
const static int RFX_MAX_FILTER_KERNEL_SIZE = 15;

#endif