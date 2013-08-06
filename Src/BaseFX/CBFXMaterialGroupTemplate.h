//==============================================================================
//
//		CBFXMaterialGroupTemplate.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/22/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXMaterialGroupTemplate : public CRefCounted<IBFXMaterialGroupTemplate>
{
private: 
	std::vector<uint> m_MatIndices;
	std::vector<CBFXMaterialPtr> m_Mats;
	wstring m_Desc;
	
	CBFXMaterial* m_MatType[BFX_MAX_MATERIAL_TYPES];
	
	CRCDataKey m_ID;
	CBFXBaseFX* m_pBase;
	
	wstring m_FileName;

public: 
	CBFXMaterialGroupTemplate(CBFXBaseFX* pBase, const CRCDataKey* pResID, const wchar* pFileName);
	virtual ~CBFXMaterialGroupTemplate();
	
public: 
	const CRCDataKey& GetID() { return m_ID; }
	uint GetNumOfMaterials() { return (uint) m_Mats.size(); }
	IBFXMaterial* GetMaterialByIndex(uint index) { return m_Mats[index]; }
	
	const wchar* GetDesc() { return m_Desc.c_str(); }
	
	IBFXMaterial* GetMaterialByType(uint type) { return m_MatType[type]; }
	uint GetMaterialTypeByIndex(uint index) { return m_MatIndices[index]; }

	void AddMaterial(uint matType, CBFXMaterial* pMat);
	void SetDesc(const wchar* pDesc) { m_Desc = pDesc; }		
	
	const wchar* GetFileName() { return m_FileName.size() > 0 ? m_FileName.c_str() : NULL; }
};

_DECLARE_SMARTPTR(CBFXMaterialGroupTemplate)

_NAMESPACE_END
