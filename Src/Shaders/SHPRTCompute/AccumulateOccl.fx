//================================================================================
//
//		AccumulateOccl.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/4/2012
//
//================================================================================

#include "[shd]\SHPRTCompute\AllSHPRTCompute.fxh"
#include "[shd]\SHPRTCompute\AccumulateOccl.fxh"	

technique t0
{
	pass p1
	{
		VertexShader	=	compile vs_3_0 AccumulateOcclVS();
		PixelShader		=	compile ps_3_0 AccumulateOcclPS();
	}
};
