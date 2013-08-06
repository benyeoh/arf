//==============================================================================
//
//		CBFXMaterial.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/22/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXMaterial : public CRefCounted<IBFXMaterial>
{
private: 
	IMGMaterialTemplatePtr m_pTemplate;
	DenseChainedHashMap<uint, IREffectTemplatePtr, NullUintHash> m_EffectTemplates;
	
	CBFXBaseFX* m_pBase;
	CRCDataKey m_ID;
	
	std::vector<uint> m_ConstantVolatileParamIndices;
	std::vector<uint> m_ConstantVolatileParamSemantics;
	std::vector<uint> m_DynamicVolatileParamIndices;
	std::vector<uint> m_DynamicVolatileParamSemantics;
	
public: 
	CBFXMaterial(CBFXBaseFX* pBase, const CRCDataKey* pResID, IMGMaterialTemplate* pTemplate);
	virtual ~CBFXMaterial();
	
public: 
	uint GetNumOfConstantVolatileParams() { return (uint) m_ConstantVolatileParamIndices.size(); }
	uint GetNumOfDynamicVolatileParams() { return (uint) m_DynamicVolatileParamIndices.size(); }
	
	uint GetConstantVolatileParamSemantic(uint index) { return m_ConstantVolatileParamSemantics[index]; }
	uint GetDynamicVolatileParamSemantic(uint index) { return m_DynamicVolatileParamSemantics[index]; }
	
	IREffectTemplate* GetEffectTemplate(uint paramAvailability);
};

_DECLARE_SMARTPTR(CBFXMaterial)

_NAMESPACE_END
