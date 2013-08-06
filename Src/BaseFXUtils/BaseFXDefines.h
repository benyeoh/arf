//==============================================================================
//
//		BaseFXDefines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/10/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

// Load / Save
const static uint BFXU_MAT_GROUP_CHUNK_H			= 0x00010000;
const static uint BFXU_MAT_GROUP_TEMPLATE_CHUNK_H	= 0x00020000;
const static uint BFXU_MESH_CHUNK_H					= 0x00030000;
const static uint BFXU_MESH_LIST_CHUNK_H			= _MAKE_ID('b', 'm', 'e', 'l');

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

// Color Channel
enum
{
	BFXU_COLOR_CHANNEL_RED		= 0,
	BFXU_COLOR_CHANNEL_GREEN	= 1,
	BFXU_COLOR_CHANNEL_BLUE		= 2,
	BFXU_COLOR_CHANNEL_ALPHA	= 3,
};

const static int BFXU_MAX_FILTER_KERNEL_SIZE = 15;
const static int BFXU_MIN_FILTER_KERNEL_SIZE = 2;

enum eBFXUPoissonBlurSamples
{
	POISSON_4_SAMPLE,
	POISSON_8_SAMPLE,
	POISSON_12_SAMPLE,
	POISSON_16_SAMPLE,

	MAX_NUM_POISSON_SAMPLES
};

_NAMESPACE_END