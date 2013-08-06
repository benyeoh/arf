//==============================================================================
//
//		OpaqueAdditivePipeSetup.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Thursday, March 01, 2007
//
//==============================================================================

technique t0
{
	pass p0
	{
		AlphaBlendEnable = TRUE;
		ZWriteEnable = FALSE;
		SrcBlend = SRCALPHA;
		DestBlend = ONE;
	}
};


