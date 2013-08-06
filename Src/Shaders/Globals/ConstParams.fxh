//==============================================================================
//
//		ConstParams.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#ifndef _CONSTPARAMS
#define _CONSTPARAMS

float g_ParallaxThickness < int semantic = RFX_PARALLAX_THICKNESS; 
							int usage = 1; 
							float min = 0.0f;
							float max = 0.1f;
							string name = "Parallax Thickness"; > = 0.02f;
							
float g_FresnelExp < int semantic = RFX_FRESNEL_EXP;
					int usage = 1;
					float min = 0.0f;
					float max = 16.0f;
					string name = "Fresnel Exponent"; > = 3.0f;

float g_FresnelMin < int semantic = RFX_FRESNEL_MIN;
					int usage = 1;
					float min = 0.0f;
					float max = 1.0f;
					string name = "Fresnel Min"; > = 0.86f;

float3 g_EnvMapTint < int semantic = RFX_ENV_MAP_TINT;
						int usage = 1;
						float min = 0.0f;
						float max = 5.0f;
						string name = "Environment Map Tint"; > = float3(1.0f, 1.0f, 1.0f);
					
float2 g_FilterTextureOffsets[RFX_MAX_FILTER_KERNEL_SIZE]	< int semantic = RFX_FILTER_TEXTURE_OFFSETS; 
															int usage = 1; 
															string name = "Filter Texture Offsets"; >;
															
float g_FilterWeights[RFX_MAX_FILTER_KERNEL_SIZE]			< int semantic = RFX_FILTER_WEIGHTS; 
															int usage = 1; 
															string name = "Filter Weights"; >;

float g_BrightThreshold		< int semantic = RFX_BRIGHT_THRESHOLD;
							int usage = 1; 
							string name = "Bright Threshold"; >;
							
float g_BrightOffset		< int semantic = RFX_BRIGHT_OFFSET; 
							int usage = 1; 
							string name = "Bright Offset"; >;
							
float g_MiddleGrayTone		< int semantic = RFX_MIDDLE_GRAY_TONE; 
							int usage = 1; 
							string name = "Middle Gray Tone"; >;
							
float g_WhiteCutoff			< int semantic = RFX_WHITE_CUTOFF; 
							int usage = 1; 
							string name = "White Cutoff"; >;

#endif