//==============================================================================
//
//		CBFXMaterial.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/22/2008
//
//==============================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN

CBFXMaterial::CBFXMaterial(CBFXBaseFX* pBase, const CRCDataKey* pResID, IMGMaterialTemplate* pTemplate)
	: m_pBase(pBase)
	, m_pTemplate(pTemplate)
{
	if(pResID)
		m_ID = *pResID;
		
	m_EffectTemplates.Initialize(128, 0.7f);
	
	//m_Effects.set_empty_key(0x80000000);
	//m_Effects.set_deleted_key(0xC0000000);
	
	uint numGlobals = m_pTemplate->GetNumOfGlobals();
	_LOOPi(numGlobals)
	{
		IMGGlobal* pGlobal = m_pTemplate->GetGlobal(i);
		if(pGlobal->GetIsVolatile())
		{
			uint semantic = pGlobal->GetSemantic();
			eMGUsage usage = pGlobal->GetUsage();
			switch(usage)
			{
				case MGU_CONSTANT:
					m_ConstantVolatileParamIndices.push_back(i);
					m_ConstantVolatileParamSemantics.push_back(semantic);
					break;
					
				case MGU_DYNAMIC:
					m_DynamicVolatileParamIndices.push_back(i);
					m_DynamicVolatileParamSemantics.push_back(semantic);	
					break;
					
				case MGU_INSTANCED:
					_DEBUG_ASSERT(FALSE);
					break; 
			}
		}
	}
	
	_DEBUG_ASSERT((uint)(m_ConstantVolatileParamSemantics.size() + m_DynamicVolatileParamSemantics.size())
		 <= BFX_MAX_VOLATILE_PARAMS);	
}

CBFXMaterial::~CBFXMaterial()
{
	((CBFXResourceMgr*)&m_pBase->GetResourceMgr())->NotifyMaterialDestroyed(m_ID, this);
}


IREffectTemplate* 
CBFXMaterial::GetEffectTemplate(uint paramAvailability)
{
	IREffectTemplatePtr* ppEffectTemplate = m_EffectTemplates.Get(paramAvailability);
	if(ppEffectTemplate)
		return (*ppEffectTemplate);
		
	//dense_hash_map<uint, IREffectPtr>::iterator itr = m_Effects.find(paramAvailability);
	//if(itr != m_Effects.end())
	//	return itr->second->GetTemplate();
	
	// Get the ID from the current material ID and availability flags
	CRCDataKey templateID;
	templateID.pHashData = _NEW CByteBuffer(m_ID.pHashData->GetDataLength() + sizeof(uint));
	AppendData(templateID.pHashData, m_ID.pHashData->GetData(), m_ID.pHashData->GetDataLength());
	AppendData(templateID.pHashData, paramAvailability);
	templateID.hashVal = CRCCalcCharLength((const char*) templateID.pHashData->GetData(), 
		templateID.pHashData->GetDataLength());
		
	IREffectTemplate* pEffectTemplate = 
		m_pBase->GetRenderer().GetRResourceMgr().LoadEffectTemplate(&templateID, NULL);
	if(!pEffectTemplate)
	{
		// Compute availibility of volatile parameters to construct effect text shader
		// TODO: We should do away with this sometime
		const static uint MAX_NUM_OF_GLOBALS = 64;
		_DEBUG_ASSERT(m_pTemplate->GetNumOfGlobals() < MAX_NUM_OF_GLOBALS);

		boolean availability[MAX_NUM_OF_GLOBALS];

		uint dynamicSize = (uint) m_DynamicVolatileParamIndices.size();
		_LOOPi(dynamicSize)
		{
			availability[m_DynamicVolatileParamIndices[i]] = (paramAvailability & (1 << i)) > 0 ? TRUE : FALSE;
		}

		uint constantSize = (uint) m_ConstantVolatileParamIndices.size();
		_LOOPi(constantSize)
		{
			availability[m_ConstantVolatileParamIndices[i]] = (paramAvailability & (1 << (i + dynamicSize))) > 0 ? TRUE : FALSE;
		}

		IByteBufferPtr pBuffer = _NEW CByteBuffer(1024);
		boolean isSuccessful = m_pTemplate->Compile(pBuffer, availability);
		_DEBUG_ASSERT(isSuccessful);	
		pEffectTemplate = m_pBase->GetRenderer().GetRResourceMgr().LoadEffectTemplate(&templateID, pBuffer);
	}
	
	//if(!pEffect)
	//{
	//	IByteBufferPtr pBuffer = _NEW CByteBuffer(1024);
	//	boolean isSuccessful = m_pTemplate->Compile(pBuffer, availability);
	//	_DEBUG_ASSERT(isSuccessful);	
	//	pEffect = m_pBase->GetRenderer().GetRResourceMgr().LoadEffect(templateID, pBuffer, NULL, NULL, 0);
	//	_DEBUG_ASSERT(pEffect);
	//}
	
	m_EffectTemplates.Add(paramAvailability, pEffectTemplate);
	
	return pEffectTemplate;
}
	
_NAMESPACE_END