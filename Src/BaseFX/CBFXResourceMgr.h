//==============================================================================
//
//		CBFXResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/21/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXResourceMgr : public IBFXResourceMgr
{
	typedef dense_hash_map<CRCDataKey, CBFXMaterialGroup*, HashCRCDataKey>			MatGroupMap;
	typedef dense_hash_map<CRCDataKey, CBFXMaterialGroupTemplate*, HashCRCDataKey>	MatGroupTemplateMap;
	typedef dense_hash_map<CRCDataKey, CBFXMaterial*, HashCRCDataKey>				MatMap;
	
private: 
	CBFXBaseFX* m_pBase;
	BFXMaterialGroupParser m_Parser;
	
	MatGroupMap			m_AllMatGroups;
	MatGroupTemplateMap m_AllMatGroupTemplates;
	MatMap				m_AllMats;
	
public: 
	CBFXResourceMgr()
		: m_pBase(NULL)
	{
		CRCDataKey emptyKey;
		emptyKey.hashVal = 0x00000000;
		emptyKey.pHashData = _NEW CByteBuffer(4);
		AppendData(emptyKey.pHashData, (uint) 0xDEADBEEF);
		
		CRCDataKey deleteKey;
		deleteKey.hashVal = 0xFFFFFFFF;
		deleteKey.pHashData = emptyKey.pHashData;
	
		m_AllMatGroups.set_deleted_key(deleteKey);
		m_AllMatGroups.set_empty_key(emptyKey);
		m_AllMatGroupTemplates.set_deleted_key(deleteKey);
		m_AllMatGroupTemplates.set_empty_key(emptyKey);
		m_AllMats.set_deleted_key(deleteKey);
		m_AllMats.set_empty_key(emptyKey);
	}

	virtual ~CBFXResourceMgr()
	{
	}

protected:
	CBFXMaterial* CreateMaterial(const CRCDataKey* pResKey, IByteBuffer* pBuffer);
	
public: 
	void SetBaseFX(CBFXBaseFX* pBase) { m_pBase = pBase; m_Parser.SetBaseFX(pBase); }
	
	IBFXMaterialGroupTemplate* CreateMaterialGroupTemplate(const CRCDataKey* pKey, IByteBuffer* pBuffer, const wchar* pFileName);
	IBFXMaterialGroup* CreateMaterialGroup(const CRCDataKey* pKey, 
		IBFXMaterialGroupTemplate* pTemplate,
		const REffectParam* pConstantParams,
		const uint* pSemantics,
		uint numParams);

	IBFXSharedParamData* CreateSharedParamData();
	IBFXPipeline* CreatePipeline();
	IBFXParamPool* CreateParamPool(IBFXSharedParamData* pSharedParam);

	void NotifyMaterialGroupTemplateDestroyed(const CRCDataKey& resKey, CBFXMaterialGroupTemplate* pTemplate);
	void NotifyMaterialGroupDestroyed(const CRCDataKey& resKey, CBFXMaterialGroup* pGroup);
	void NotifyMaterialDestroyed(const CRCDataKey& resKey, CBFXMaterial* pMat);
};

_NAMESPACE_END
