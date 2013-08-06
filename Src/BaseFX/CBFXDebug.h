//==============================================================================
//
//		CBFXPipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXDebug : public IBFXDebug
{
private:	
	CBFXBaseFX* m_pBase;
	
public: 
	CBFXDebug()
	{
	}

	virtual ~CBFXDebug()
	{
	}

public: 
	void SetBaseFX(CBFXBaseFX* pBase) { m_pBase = pBase; }
	void Initialize();

	void SetWireFrame(boolean isEnabled);
};

_NAMESPACE_END
