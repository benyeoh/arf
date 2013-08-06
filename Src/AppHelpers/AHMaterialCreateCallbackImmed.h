//================================================================================
//
//		AHMaterialCreateCallbackImmed.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/23/2012
//
//================================================================================

#pragma once

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

_NAMESPACE_BEGIN

class AHMaterialCreateCallbackImmed : public CRefCounted<IRefCounted>, public IBFXMaterialCreateCallback
{
private:
	Mutex						m_MatMutex;

public:
	AHMaterialCreateCallbackImmed()
	{
		m_MatMutex.Initialize();
	}

	~AHMaterialCreateCallbackImmed()
	{
	}

public:
	IREffect* CreateEffect(IBFXMaterialGroup* pGroup, uint type, uint availability);
};

_DECLARE_SMARTPTR(AHMaterialCreateCallbackImmed)

_NAMESPACE_END

#endif