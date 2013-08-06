//==============================================================================
//
//		HLDL1.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, June 06, 2006
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\HLDL1.fxh"
			
_DECL_EFFECT_TYPE = RFX_TYPE_HLDL1;						  
_DECL_EFFECT_DESC = "Hemi Light, 1 Dir Light, Diffuse";

technique t0
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 HLDL1VS();
		PixelShader		=	compile ps_2_0 HLDL1PS();
	}
};

