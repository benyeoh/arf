//==============================================================================
//
//		CBFXMaterialGroup.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/22/2008
//
//==============================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN


CBFXMaterialGroup::CBFXMaterialGroup(CBFXBaseFX* pBase, const CRCDataKey* pResID, CBFXMaterialGroupTemplate* pGroupTemplate)
	: m_pGroupTemplate(pGroupTemplate)
	, m_pBase(pBase)
{
	if(pResID)
		m_ID = *pResID;
		
	m_ParamSemanticIndices.Initialize(64, 0.7f);
	
	//m_ParamSemanticIndices.set_empty_key(0xFFFFFFFE);
	//m_ParamSemanticIndices.set_deleted_key(0xFFFFFFFF);
}

CBFXMaterialGroup::~CBFXMaterialGroup()
{
	((CBFXResourceMgr*)&m_pBase->GetResourceMgr())->NotifyMaterialGroupDestroyed(m_ID, this);
}

uint 
CBFXMaterialGroup::GetNumOfParams()
{
	return (uint) m_ParamSemantics.size();
}

void
CBFXMaterialGroup::GetParam(uint index, REffectParam& param)
{
	m_ParamStores[index].AssignREffectParam(param);
}

uint 
CBFXMaterialGroup::GetSemantic(uint index)
{
	return m_ParamSemantics[index];
}

void 
CBFXMaterialGroup::AddParam(const REffectParam* pParam, uint semantic)
{
	if(pParam->type != EPT_END)
	{
		//_DEBUG_ASSERT(m_ParamSemanticIndices.find(semantic) == m_ParamSemanticIndices.end());
		m_ParamStores.push_back(REffectParamStore());
		m_ParamSemantics.push_back(semantic);
		m_ParamStores.back() = *pParam;
		
		m_ParamSemanticIndices.Add(semantic, (uint) m_ParamSemantics.size() - 1);
		//m_ParamSemanticIndices[semantic] = (uint) m_ParamSemantics.size() - 1;
	}
}

//void 
//CBFXMaterialGroup::SetCurrentMatType(uint type)
//{
//	_DEBUG_ASSERT(type < BFX_MAX_MATERIAL_TYPES);
//	m_CurrentType = type;
//}

//uint 
//CBFXMaterialGroup::GetCurrentMatType()
//{
//	return m_CurrentType;
//}

boolean 
CBFXMaterialGroup::IsMatTypeValid(uint type)
{
	return m_pGroupTemplate->GetMaterialByType(type) != NULL;
}	
	
IREffect*
CBFXMaterialGroup::GetEffect(uint type, uint availabilityFlag)
{
	_DEBUG_ASSERT(type < BFX_MAX_MATERIAL_TYPES);

	CBFXMaterial* pMat = (CBFXMaterial*) m_pGroupTemplate->GetMaterialByType(type);
	MaterialInstance& instance = m_MaterialInstances[type];

	IREffect* pEffect = NULL;
	if(instance.constantParamFlags != BFX_INVALID)
	{
		IREffectPtr* ppEffect = instance.cache.Get(availabilityFlag);
		if(ppEffect)
			pEffect = ppEffect->GetPtr();
	}	

	return pEffect;
}

uint
CBFXMaterialGroup::GetParamAvailability(uint type, BFXParamContainer& container)
{
	_DEBUG_ASSERT(type < BFX_MAX_MATERIAL_TYPES);

	CBFXMaterial* pMat = (CBFXMaterial*) m_pGroupTemplate->GetMaterialByType(type);
	MaterialInstance& instance = m_MaterialInstances[type];

	uint availabilityFlag = 0;
	uint dynamicSize = pMat->GetNumOfDynamicVolatileParams();
	_LOOPi(dynamicSize)
	{
		uint semantic = pMat->GetDynamicVolatileParamSemantic(i);
		uint libIndex = _GET_LIB_INDEX(semantic);
		IBFXParamCallback* pCallback = container.GetParamCallback(libIndex);
		_DEBUG_ASSERT(pCallback);

		uint val = pCallback->IsParamAvailable(semantic) ? 1 : 0;
		availabilityFlag |= (val << i);
	}

	if(instance.constantParamFlags == BFX_INVALID)
	{
		// Initialize the cache when needed
		//instance.cache.Initialize(128, 0.7f);

		uint constantFlag = 0;
		uint constantSize = pMat->GetNumOfConstantVolatileParams();
		_LOOPi(constantSize)
		{
			uint semantic = pMat->GetConstantVolatileParamSemantic(i);
			uint val = m_ParamSemanticIndices.Get(semantic) ? 1 : 0; 
			//(m_ParamSemanticIndices.find(semantic) != m_ParamSemanticIndices.end()) ? 1 : 0;
			constantFlag |= (val << (i+dynamicSize));
		}

		availabilityFlag |= constantFlag;
	}
	else
		availabilityFlag |= instance.constantParamFlags;

	return availabilityFlag;
}

IREffect*
CBFXMaterialGroup::CreateEffect(uint type, uint availabilityFlag)
{
	_DEBUG_ASSERT(type < BFX_MAX_MATERIAL_TYPES);
	
	CBFXMaterial* pMat = (CBFXMaterial*) m_pGroupTemplate->GetMaterialByType(type);
	MaterialInstance& instance = m_MaterialInstances[type];
	
	uint dynamicSize = pMat->GetNumOfDynamicVolatileParams();
	if(instance.constantParamFlags == BFX_INVALID)
	{
		// Initialize the cache when needed
		instance.cache.Initialize(128, 0.7f);
		instance.constantParamFlags = availabilityFlag & (0xFFFFFFFF << dynamicSize);
	}

	IREffectPtr* ppEffect = instance.cache.Get(availabilityFlag);
	if(!ppEffect)
	{
		IREffectTemplate* pEffectTemplate = pMat->GetEffectTemplate(availabilityFlag);
		
		REffectParam params[RRenderOp::MAX_NUM_PARAMS];
		uint paramSemantics[RRenderOp::MAX_NUM_PARAMS];

		_LOOPi(m_ParamStores.size())
		{
			GetParam(i, params[i]);
			paramSemantics[i] = m_ParamSemantics[i];
		}
		
		// Create a new effect here with the actual constant parameters
		IREffect* pEffect = 
			m_pBase->GetRenderer().GetRResourceMgr().LoadEffect(pEffectTemplate, 
				params, paramSemantics, (uint) m_ParamStores.size());
		
		instance.cache.Add(availabilityFlag, pEffect);
		return pEffect;
	}

	return (*ppEffect);
		
	//EffectCache::iterator itr = instance.cache.find(availabilityFlag);
	//if(itr == instance.cache.end())
	//{
	//	IREffectTemplate* pEffectTemplate = pMat->GetEffectTemplate(availabilityFlag);
	//	
	//	REffectParam params[RRenderOp::MAX_NUM_PARAMS];
	//	_LOOPi(m_ParamStores.size())
	//	{
	//		GetParam(i, params[i]);
	//	}
	//	
	//	// Create a new effect here with the actual constant parameters
	//	IREffect* pEffect = 
	//		m_pBase->GetRenderer().GetRResourceMgr().LoadEffect(pEffectTemplate, params, &m_ParamSemantics[0], (uint) m_ParamStores.size());
	//	
	//	instance.cache[availabilityFlag] = pEffect;
	//	return pEffect;
	//}

	//return itr->second;
}


_NAMESPACE_END