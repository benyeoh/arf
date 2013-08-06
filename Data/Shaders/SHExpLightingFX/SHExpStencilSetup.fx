//==============================================================================
//
//		SHExpStencilSetup.fx
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

/*
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
*/

