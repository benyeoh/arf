//==============================================================================
//
//		CRResource.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, March 20, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

template <typename _INHERITED>
class CRResource : public CRefCounted< _INHERITED >
{	
protected:
	CRResourceMgr* m_pResourceMgr;
	wstring m_Name;
	CRCDataKey m_ID;
	eRResourceType m_Type;
	
public:
	CRResource()
		: m_pResourceMgr(NULL)
		, m_Type(RT_UNKNOWN)
	{
		m_Name = _W("");
	}

	virtual ~CRResource()
	{
		if(m_pResourceMgr)
			m_pResourceMgr->OnDestroyResource(m_ID, m_Type, this);
	}
	
public:	
	void SetResourceParams(CRResourceMgr* pMgr, const wchar* pName, const CRCDataKey* pID, eRResourceType type)
	{
		m_pResourceMgr = pMgr;	
		if(pID)
			m_ID = *pID;
		if(pName)
			m_Name = pName;
		else
			m_Name.clear();
		m_Type = type;
	}
	
	const wchar* GetResourceName() const
	{
		return m_Name.c_str();
	}
	
	const CRCDataKey& GetResourceID() const
	{
		return m_ID;
	}
	
	//------------------------------------------------------------------------------
	//	OnLostDevice()
	//		Handles the case when the render device is lost (when the app is 
	//		minimized or when a screen-saver kicks in etc)
	//------------------------------------------------------------------------------
	virtual void OnLostDevice()
	{
	}

	//------------------------------------------------------------------------------
	//	OnResetDevice()
	//		Handles the case when the render device is (re)initialized	
	//------------------------------------------------------------------------------
	virtual void OnResetDevice()
	{
	}

	//------------------------------------------------------------------------------
	//	OnRestoreDevice()
	//		Handles the case when the render device context is regained after
	//		losing it
	//------------------------------------------------------------------------------
	virtual void OnRestoreDevice()
	{
	}
};

_NAMESPACE_END