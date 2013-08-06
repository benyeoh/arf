//==============================================================================
//
//		CPPlatform.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, March 19, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CPPlatform : public CRefCounted<IPPlatform>
{
private:
	CPTimer*		m_pTimer;
	CPResourceMgr*	m_pResMgr;

public:
	CPPlatform(CPTimer* pTimer, CPResourceMgr* pResMgr)
		: m_pTimer(pTimer)
		, m_pResMgr(pResMgr)
	{
	}

	virtual ~CPPlatform()
	{
		_DELETE(m_pTimer);
		_DELETE(m_pResMgr);

		//g_pPlatform = NULL;
	}

protected:
	virtual boolean DoInitialize() = 0;

public:
	boolean Initialize();
	
	IPResourceMgr& GetResourceMgr() { return *m_pResMgr; }
	IPTimer& GetTimer() { return *m_pTimer; }
};

_NAMESPACE_END