//==============================================================================
//
//		CIMouse.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#pragma once 


_NAMESPACE_BEGIN

class CIMouse : public IIMouse
{
private: 
	gmtl::Vec3f m_Pos;
	gmtl::Vec3f m_PosDelta;
	byte m_ButtonStates[IMC_END];
	std::vector<eIMouseCode> m_ActiveButtons;
	
public: 
	CIMouse()
	{
		ClearMouseStates();
	}

	virtual ~CIMouse()
	{
	}

protected:
	_PURE( void DoUpdate() )
	void SetPos(const gmtl::Vec3f& pos); 	
	void SetPosDelta(const gmtl::Vec3f& posDelta);
	void SetButtonState(eIMouseCode mouseButtonCode, byte state);

public: 
	const gmtl::Vec3f& GetPosDelta() { return m_PosDelta; }
	const gmtl::Vec3f& GetPos() { return m_Pos; }
	
	void Update();
	void ClearMouseStates();
	
	uint GetNumOfActiveButtons();
	const eIMouseCode* GetActiveButtons();
};


_NAMESPACE_END