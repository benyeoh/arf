//==============================================================================
//
//		CE_NM_PL1Spec.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/23/2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\CubeEnvMapping\CE_PL1Spec.fxh"
#include "[shd]\LambertianDiffuseLighting\CubeEnvMapping\NormalMapping\CE_NM_PL1Spec.fxh"
				
_DECL_EFFECT_TYPE = RFX_TYPE_PL1;						  
_DECL_EFFECT_DESC = "1 Point Light, Normal Mapping, Diffuse, Specular, Env Mapping";				
_DECL_VERTEX_DESC(1) = VDU_TANGENT + 0;
_DECL_VERTEX_DESC(2) = VDU_BINORMAL + 0;

technique t0
{
	pass p1
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 CE_PL1SpecVS();
		PixelShader		=	compile ps_2_0 CE_PL1SpecPS();
	}
};

technique t1
{
	pass p1
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 CE_NM_PL1SpecVS();
		PixelShader		=	compile ps_2_0 CE_NM_PL1SpecPS();
	}
};