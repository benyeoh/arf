//==============================================================================
//
//		CBFXMaterialGroup.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/22/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXMaterialGroup : public CRefCounted<IBFXMaterialGroup>
{
	typedef DenseChainedHashMap<uint, IREffectPtr, NullUintHash> EffectCache;
	
	struct MaterialInstance
	{
		EffectCache cache;
		uint constantParamFlags;
		
		MaterialInstance()
			: constantParamFlags(BFX_INVALID)
		{
			//cache.Initialize(128, 0.7f);
			
			// Something else?
			// The last bit is reserved for this purpose
			//cache.set_empty_key(0x80000000);
			//cache.set_deleted_key(0xC0000000);			
		}
	};


private:
	CBFXMaterialGroupTemplatePtr m_pGroupTemplate;
	
	std::vector<REffectParamStore> m_ParamStores;
	std::vector<uint> m_ParamSemantics;
	DenseChainedHashMap<uint, uint, NullUintHash> m_ParamSemanticIndices;
	
	MaterialInstance m_MaterialInstances[BFX_MAX_MATERIAL_TYPES];
	
	CBFXBaseFX* m_pBase;
	
	CRCDataKey m_ID;
	
public: 
	CBFXMaterialGroup(CBFXBaseFX* pBase, const CRCDataKey* pResID, CBFXMaterialGroupTemplate* pGroupTemplate);
	virtual ~CBFXMaterialGroup();

public:
	IBFXMaterialGroupTemplate* GetTemplate() { return m_pGroupTemplate; }
	
	uint GetNumOfParams();
	void GetParam(uint index, REffectParam& param);
	
	uint GetSemantic(uint index);
	void AddParam(const REffectParam* pParam, uint semantic);
	
	//void SetCurrentMatType(uint type);
	//uint GetCurrentMatType();
	
	boolean IsMatTypeValid(uint type);
	
	IREffect* CreateEffect(uint type, uint availabilityFlag);
	IREffect* GetEffect(uint type, uint availabilityFlag);
	uint GetParamAvailability(uint type, BFXParamContainer& container);

	const CRCDataKey& GetID() { return m_ID; }
};

_DECLARE_SMARTPTR(CBFXMaterialGroup)

_NAMESPACE_END
