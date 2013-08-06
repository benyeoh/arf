//==============================================================================
//
//		CE_PL1Spec.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/16/2007
//
//==============================================================================


#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\PL1.fxh"
#include "[shd]\LambertianDiffuseLighting\PL1Spec.fxh"
#include "[shd]\LambertianDiffuseLighting\CubeEnvMapping\CE_PL1Spec.fxh"
	
_DECL_EFFECT_TYPE = RFX_TYPE_PL1;
_DECL_EFFECT_DESC = "1 Point Light, Diffuse, Specular, Env Mapping";

technique t0
{
	pass p0
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 CE_PL1SpecVS();
		PixelShader		=	compile ps_2_0 CE_PL1SpecPS();
	}
};
