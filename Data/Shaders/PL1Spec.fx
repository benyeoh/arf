//==============================================================================
//
//		PL1Spec.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\PL1.fxh"
#include "[shd]\LambertianDiffuseLighting\PL1Spec.fxh"

_DECL_EFFECT_TYPE = RFX_TYPE_PL1;
_DECL_EFFECT_DESC = "1 Point Light, Diffuse, Specular";

technique t0
{
	pass p0
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 PL1VS();
		PixelShader		=	compile ps_2_0 PL1PS();
	}
};

technique t1
{
	pass p1
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 PL1SpecVS();
		PixelShader		=	compile ps_2_0 PL1SpecPS();
	}
};