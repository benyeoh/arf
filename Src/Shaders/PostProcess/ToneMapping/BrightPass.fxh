//==============================================================================
//
//		BrightPass.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/2/2007
//
//==============================================================================

#ifndef BRIGHTPASS
#define BRIGHTPASS

PS_OUT_Color
BrightPassPS(VS_OUT_ScreenSpaceBasic input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
    float4 color = tex2D(g_SampRT1, input.texUV);

	// Perform tone mapping based on the middle gray color and average scene luminance
	// to find out the pixel color if tone mapping is performed
    color.rgb *= g_MiddleGrayTone / ( g_AverageLuminance + 0.001f );	
    
    // Range [0..2 * g_WhiteCutoff] if color is [0..g_WhiteCutoff]
    color.rgb *= ( 1.0f + ( color.rgb / ( g_WhiteCutoff * g_WhiteCutoff ) ) );
    
    // Remove dark pixels
    color.rgb -= g_BrightThreshold;

    color.rgb = max( color.rgb, 0.0f );

	// Map from 0..1
    color.rgb /= ( g_BrightOffset + color.rgb );

    output.color = color;
    
	return output;
}

#endif