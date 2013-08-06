//================================================================================
//
//		ParamSemanticDefs.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/2/2012
//
//================================================================================

#ifndef _BASEFXUTILS_PARAMSEMANTICDEFS
#define _BASEFXUTILS_PARAMSEMANTICDEFS

const static int BFXU_RT1						= 0xFFFF0000;
const static int BFXU_RT2						= BFXU_RT1 + 1;
const static int BFXU_FILTER_TEXTURE_OFFSETS	= BFXU_RT2 + 1;
const static int BFXU_FILTER_WEIGHTS			= BFXU_FILTER_TEXTURE_OFFSETS + 1;
const static int BFXU_COLOR_FILL				= BFXU_FILTER_WEIGHTS + 1;
const static int BFXU_SCREEN_PIXEL_OFFSET		= BFXU_COLOR_FILL + 1;
const static int BFXU_RT1_SIZE					= BFXU_SCREEN_PIXEL_OFFSET + 1;
const static int BFXU_RT2_SIZE					= BFXU_RT1_SIZE + 1;
const static int BFXU_RT3						= BFXU_RT2_SIZE + 1;
const static int BFXU_RT4						= BFXU_RT3 + 1;
const static int BFXU_RT5						= BFXU_RT4 + 1;
const static int BFXU_POISSON_DISK_SCALE		= BFXU_RT5 + 1;
const static int BFXU_NOISE_SCALE				= BFXU_POISSON_DISK_SCALE + 1;
const static int BFXU_X_CUBE_AXIS				= BFXU_NOISE_SCALE + 1;
const static int BFXU_Y_CUBE_AXIS				= BFXU_X_CUBE_AXIS + 1;
const static int BFXU_Z_CUBE_AXIS				= BFXU_Y_CUBE_AXIS + 1;

#endif