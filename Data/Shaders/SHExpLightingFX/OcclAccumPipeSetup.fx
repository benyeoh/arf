//==============================================================================
//
//		OcclAccumPipeSetup.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		02/12/09
//
//==============================================================================

technique t0
{
	pass p0
	{
		StencilEnable = TRUE;
		ZWriteEnable = FALSE;
		AlphaBlendEnable = TRUE;
		SrcBlend = ONE;
		DestBlend = ONE;
	}
};
