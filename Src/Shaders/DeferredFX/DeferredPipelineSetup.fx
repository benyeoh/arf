//==============================================================================
//
//		DeferredStencilSetup.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		14/10/2009
//
//==============================================================================

technique Read
{
	pass p0
	{
		StencilEnable		= TRUE;
		StencilFunc			= EQUAL;
		StencilMask			= 0x1;
		StencilRef			= 1;
	}
};

technique Write
{
	pass p0
	{
		StencilEnable		= TRUE;
		StencilFunc			= ALWAYS;
		StencilWriteMask	= 0x1;
		StencilRef			= 1;
		StencilPass			= REPLACE;		
	}
};

technique Light
{
	pass p0
	{
		AlphaBlendEnable	= TRUE;
		SrcBlend			= ONE;
		DestBlend			= ONE;
		ZWriteEnable		= FALSE;
		StencilEnable		= TRUE;
		StencilRef			= 0x2;
		StencilWriteMask	= 0x2;
		StencilMask			= 0x2;
	}
}