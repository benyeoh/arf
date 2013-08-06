#ifndef _TESTSCREENSPACEOCCL_PARAMSEMANTICDEFS
#define _TESTSCREENSPACEOCCL_PARAMSEMANTICDEFS

// Dynamic params
const static int NUM_SAMPLES = 0xFFFF0000;
const static int ALCHEMY_AO_PARAMS	= NUM_SAMPLES + 1;
const static int DISTRIBUTED_SAMPLE_PARAMS = ALCHEMY_AO_PARAMS + 1;
const static int CAMERA_POS_WITH_VIEW_FAR = DISTRIBUTED_SAMPLE_PARAMS + 1;
const static int FAR_PLANE_OFFSETS = CAMERA_POS_WITH_VIEW_FAR + 1;

// Constant params
const static int NOISE_TEX = 0xFFFFAAAA;
const static int Z_POS_TEX = NOISE_TEX + 1;
const static int NORMALS_TEX = Z_POS_TEX + 1;
const static int PROJ_PARAMS = NORMALS_TEX + 1;
const static int SAMPLE_LOD = PROJ_PARAMS + 1;
const static int HALF_PIXEL_OFFSET = SAMPLE_LOD + 1;
const static int AO_TEX	= HALF_PIXEL_OFFSET + 1;

const static int NUM_Z_MIPS = 4;

#endif