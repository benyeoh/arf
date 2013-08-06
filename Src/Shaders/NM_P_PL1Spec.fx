//==============================================================================
//
//		NM_P_PL1Spec.fx
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
#include "[shd]\LambertianDiffuseLighting\NormalMapping\NM_PL1Spec.fxh"
#include "[shd]\LambertianDiffuseLighting\NormalMappingAndParallax\NM_P_PL1Spec.fxh"

_DECL_EFFECT_TYPE = RFX_TYPE_PL1;
_DECL_EFFECT_DESC = "Point Light, Normal Mapping, Parallax, Diffuse, Specular";
_DECL_VERTEX_DESC(1) = VDU_TANGENT + 0;
_DECL_VERTEX_DESC(2) = VDU_BINORMAL + 0;

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
	pass p0
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 PL1SpecVS();
		PixelShader		=	compile ps_2_0 PL1SpecPS();
	}
};

technique t2
{
	pass p0
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 NM_PL1SpecVS();
		PixelShader		=	compile ps_2_0 NM_PL1SpecPS();
	}
};

technique t3
{
	pass p0
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 NM_P_PL1SpecVS();
		PixelShader		=	compile ps_2_0 NM_P_PL1SpecPS();
	}
};
