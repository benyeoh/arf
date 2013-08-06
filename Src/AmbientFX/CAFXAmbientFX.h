//==============================================================================
//
//		CAFXAmbientFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/7/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CAFXAmbientFX : public CRefCounted<IAFXAmbientFX>
{
private:
	CAFXResourceMgr		m_ResourceMgr;
	CAFXSHLightToolKit	m_SHToolKit;

	IAppCallback*	m_pAppCallback;	

public:
	CAFXAmbientFX()
		: m_pAppCallback(NULL)
	{
	}

	virtual ~CAFXAmbientFX()
	{
	}

public:
	boolean	Initialize(IAppCallback& callback);

	void	LerpAmbientSHCoeff(AFXAmbientCoeffs& dest, const AFXAmbientCoeffs& src1, const AFXAmbientCoeffs& src2, float amount);

	IAFXResourceMgr&	GetResourceMgr()	{ return m_ResourceMgr; }
	IAFXSHLightToolKit& GetSHToolKit()		{ return m_SHToolKit; }

	IAppCallback&		GetAppCallback()	{ return *m_pAppCallback; }
};

_NAMESPACE_END
