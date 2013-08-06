//==============================================================================
//
//		PermAlphaMode.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/17/2007
//
//==============================================================================

#ifndef _PERMALPHAMODE
#define _PERMALPHAMODE

int g_AlphaMode < int semantic = RFX_ALPHA_MODE; 
				int usage = 1; 
				bool always_used = true;
				string name = "Alpha Mode";
				string combo = "Opaque, Transparency, Translucency"; >;
				
// Alpha Testing
void DoAlphaTest(float alpha)
{
	clip(alpha - 2.0f / 256.0f);
}

void ConditionalAlphaTest(float alpha)
{
	if(g_AlphaMode > 0)
	{
		DoAlphaTest(alpha);
	}
}

#endif