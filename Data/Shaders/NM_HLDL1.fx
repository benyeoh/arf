//==============================================================================
//
//		NM_HLDL1.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/22/2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\Globals\PermAlphaMode.fxh"
#include "[shd]\LambertianDiffuseLighting\HLDL1.fxh"
#include "[shd]\LambertianDiffuseLighting\NormalMapping\NM_HLDL1.fxh"
	
_DECL_EFFECT_TYPE = RFX_TYPE_HLDL1;						  
_DECL_EFFECT_DESC = "Hemi Light, 1 Dir Light, Normal Mapping, Diffuse";				
_DECL_VERTEX_DESC(1) = VDU_TANGENT + 0;
_DECL_VERTEX_DESC(2) = VDU_BINORMAL + 0;

technique t0
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 HLDL1VS();
		PixelShader		=	compile ps_2_0 HLDL1PS();
	}
};

technique t1
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 NM_HLDL1VS();
		PixelShader		=	compile ps_2_0 NM_HLDL1PS();
	}
};
