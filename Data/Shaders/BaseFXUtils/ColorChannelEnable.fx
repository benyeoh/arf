//==============================================================================
//
//		ColorWriteAlpha.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/12/2008
//
//==============================================================================


technique Red
{
	pass p0
	{
		ColorWriteEnable = RED;
	}
};

technique Green
{
	pass p0
	{
		ColorWriteEnable = GREEN;
	}
};

technique Blue
{
	pass p0
	{
		ColorWriteEnable = BLUE;
	}
};

technique Alpha
{
	pass p0
	{
		ColorWriteEnable = ALPHA;
	}	
};