//==============================================================================
//
//		CRStateD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
//
//==============================================================================

#pragma once

class CRStateD3D : public CRState
{
private:
	CRIndexBufferD3D* m_pIB;
	CRVertexBufferGroupD3D* m_pVBGroup;
	
public:
	CRStateD3D()
		: m_pIB(0)
		, m_pVBGroup(0)
	{
	}

	virtual ~CRStateD3D()
	{
	}
		
protected:
	void DoSetVertexBufferGroup(IRVertexBufferGroup* pVBGroup);
	void DoSetIndexBuffer(IRIndexBuffer* pIB);
	
	void DoOnRestoreDevice();
	void DoOnResetDevice();
	void DoOnLostDevice();
	
public:
	CRVertexBufferGroupD3D* GetVertexBufferGroup() { return m_pVBGroup; }
	CRIndexBufferD3D* GetIndexBuffer() { return m_pIB; }
};