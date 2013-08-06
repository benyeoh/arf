//==============================================================================
//
//		PL1.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Wednesday, February 07, 2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\PL1.fxh"

_DECL_EFFECT_TYPE = RFX_TYPE_PL1;
_DECL_EFFECT_DESC = "1 Point Light, Diffuse";

technique t0
{
	pass p0
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 PL1VS();
		PixelShader		=	compile ps_2_0 PL1PS();
	}
};
