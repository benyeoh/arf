//==============================================================================
//
//		CE_HLDL1Spec.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/16/2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\HLDL1.fxh"
#include "[shd]\LambertianDiffuseLighting\HLDL1Spec.fxh"
#include "[shd]\LambertianDiffuseLighting\CubeEnvMapping\CE_HLDL1Spec.fxh"
				
_DECL_EFFECT_TYPE = RFX_TYPE_HLDL1;						  
_DECL_EFFECT_DESC = "Hemi Light, 1 Dir Light, Diffuse, Specular, Env Mapping";				

technique t0
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 CE_HLDL1SpecVS();
		PixelShader		=	compile ps_2_0 CE_HLDL1SpecPS();
	}
};
