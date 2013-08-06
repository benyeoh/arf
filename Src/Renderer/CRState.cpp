//==============================================================================
//
//		CRState.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
//
//==============================================================================

#include "RendererFramework.h"

void 
CRState::SetAlphaBlendEnable(boolean val)
{
	DoSetAlphaBlendEnable(val);
}

void 
CRState::SetAlphaBlendType(eAlphaBlendOp srcBlend, eAlphaBlendOp destBlend)
{
	DoSetAlphaBlendType(srcBlend, destBlend);
}

void 
CRState::SetZEnable(boolean val)
{
	DoSetZEnable(val);
}

void 
CRState::SetZWriteEnable(boolean val)
{
	DoZWriteEnable(val);
}

void 
CRState::SetAlphaTestEnable(boolean val)
{
	DoSetAlphaTestEnable(val);
}

void 
CRState::SetColorWriteEnable(eColorWrite val)
{
	DoSetColorWriteEnable(val);
}

void 
CRState::SetVertexBufferGroup(IRVertexBufferGroup* pVBGroup)
{
	DoSetVertexBufferGroup(pVBGroup);
}

void 
CRState::SetIndexBuffer(IRIndexBuffer* pIB)
{
	DoSetIndexBuffer(pIB);
}

void 
CRState::OnLostDevice()
{
	DoOnLostDevice();
}

void 
CRState::OnResetDevice()
{
	DoOnResetDevice();
}

void 
CRState::OnRestoreDevice()
{
	DoOnRestoreDevice();
}
