//==============================================================================
//
//		NM_PL1.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/23/2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\PL1.fxh"
#include "[shd]\LambertianDiffuseLighting\NormalMapping\NM_PL1.fxh"
				
_DECL_EFFECT_TYPE = RFX_TYPE_PL1;
_DECL_EFFECT_DESC = "1 Point Light,  Normal Mapping, Diffuse";
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
	pass p1
	{
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 NM_PL1VS();
		PixelShader		=	compile ps_2_0 NM_PL1PS();
	}
};