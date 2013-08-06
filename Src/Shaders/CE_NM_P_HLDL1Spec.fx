//==============================================================================
//
//		CE_NM_P_HLDL1Spec.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/25/2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\HLDL1.fxh"
#include "[shd]\LambertianDiffuseLighting\HLDL1Spec.fxh"
#include "[shd]\LambertianDiffuseLighting\CubeEnvMapping\CE_HLDL1Spec.fxh"
#include "[shd]\LambertianDiffuseLighting\CubeEnvMapping\NormalMapping\CE_NM_HLDL1Spec.fxh"
#include "[shd]\LambertianDiffuseLighting\CubeEnvMapping\NormalMappingAndParallax\CE_NM_P_HLDL1Spec.fxh"
				
_DECL_EFFECT_TYPE = RFX_TYPE_HLDL1;						  
_DECL_EFFECT_DESC = "Hemi Light, 1 Dir Light, Normal Mapping, Parallax, Diffuse, Specular, Env Mapping";				
_DECL_VERTEX_DESC(1) = VDU_TANGENT + 0;
_DECL_VERTEX_DESC(2) = VDU_BINORMAL + 0;

technique t0
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 CE_HLDL1SpecVS();
		PixelShader		=	compile ps_2_0 CE_HLDL1SpecPS();
	}
};

technique t1
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 CE_NM_HLDL1SpecVS();
		PixelShader		=	compile ps_2_0 CE_NM_HLDL1SpecPS();
	}
};

technique t2
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 CE_NM_P_HLDL1SpecVS();
		PixelShader		=	compile ps_2_0 CE_NM_P_HLDL1SpecPS();
	}
};