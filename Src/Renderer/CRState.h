//==============================================================================
//
//		CRState.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
//
//==============================================================================

#pragma once

class CRState : public IRState
{
public:
	CRState()
	{
	}

	virtual ~CRState()
	{
	}

protected:
	//virtual void DoSetAlphaEnable() = 0;
	
	_PURE( void DoSetAlphaBlendEnable(boolean val) )
	_PURE( void DoSetAlphaBlendType(eAlphaBlendOp srcBlend, eAlphaBlendOp destBlend) )
	_PURE( void DoSetZEnable(boolean val) )
	_PURE( void DoSetZWriteEnable(boolean val) )
	_PURE( void DoSetAlphaTestEnable(boolean val) )
	_PURE( void DoSetColorWriteEnable(eColorWrite val) )
	
	virtual void DoSetVertexBufferGroup(IRVertexBufferGroup* pVBGroup) = 0;
	virtual void DoSetIndexBuffer(IRIndexBuffer* pIB) = 0;

	virtual void DoOnLostDevice() = 0;
	virtual void DoOnRestoreDevice() = 0;
	virtual void DoOnResetDevice() = 0;

public:
	void SetAlphaBlendEnable(boolean val);
	void SetAlphaBlendType(eAlphaBlendOp srcBlend, eAlphaBlendOp destBlend);
	void SetZEnable(boolean val);
	void SetZWriteEnable(boolean val);
	void SetAlphaTestEnable(boolean val);
	void SetColorWriteEnable(eColorWrite val);
	
	void SetVertexBufferGroup(IRVertexBufferGroup* pVBGroup);
	void SetIndexBuffer(IRIndexBuffer* pIB);
		
	void OnLostDevice();	
	void OnResetDevice();
	void OnRestoreDevice();
};