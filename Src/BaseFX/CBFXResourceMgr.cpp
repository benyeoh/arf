//==============================================================================
//
//		CBFXResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/22/2008
//
//==============================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN

CBFXMaterial* 
CBFXResourceMgr::CreateMaterial(const CRCDataKey* pKey, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pKey);
	_DEBUG_ASSERT(pKey->IsValid());
	
	MatMap::iterator itr = m_AllMats.find(*pKey);
	if(itr != m_AllMats.end())
	{
		return itr->second;
	}
		
	if(pBuffer)
	{
		IMGMaterialTemplate* pTemplate = NULL;
		LoadMaterialTemplate(&m_pBase->GetMaterialGen(), pBuffer, 0, &pTemplate);
		CBFXMaterial* pMat = _NEW CBFXMaterial(m_pBase, pKey, pTemplate);
		m_AllMats[*pKey] = pMat;
		
		return pMat;
	}
	
	return NULL;
}


IBFXMaterialGroupTemplate* 
CBFXResourceMgr::CreateMaterialGroupTemplate(const CRCDataKey* pKey, IByteBuffer* pBuffer, const wchar* pFileName)
{
	if(pKey)
	{
		MatGroupTemplateMap::iterator itr = m_AllMatGroupTemplates.find(*pKey);
		if(itr != m_AllMatGroupTemplates.end())
		{
	#ifdef _DEBUG
			wstring fileName = pFileName;
			_DEBUG_ASSERT(fileName == itr->second->GetFileName());
	#endif 
			return itr->second;
		}
	}
	
	CBFXMaterialGroupTemplate* pToReturn = NULL;
			
	if(pBuffer)
	{
		if(m_Parser.Parse(pBuffer))
		{
			pToReturn = _NEW CBFXMaterialGroupTemplate(m_pBase, pKey, pFileName);
			uint numMats = m_Parser.GetNumOfMats();
			_LOOPi(numMats)
			{
				wstring matName = m_Parser.GetMatName(i);
				uint matIndex = m_Parser.GetMatIndex(i);

				CRCDataKey matID;
				matID.Set(matName.c_str(), (uint) matName.size() * sizeof(wchar));
				CBFXMaterial* pMat = CreateMaterial(&matID, NULL);
				if(!pMat)
				{
					IByteBufferPtr pFileData = m_pBase->GetAppCallback().GetFileData(matName.c_str());
					if(pFileData)
					{
						pMat = CreateMaterial(&matID, pFileData);
						_DEBUG_ASSERT(pMat);
					}
					else
					{
						wstring errMsg = _W("ERROR: Missing material ");
						errMsg = errMsg + _W("\'") + matName + _W("\'") + _W(" while parsing material group template ") + _W("\'") + pFileName + _W("\'!");

						m_pBase->GetAppCallback().Log(errMsg.c_str());
					}
				}				
				
				if(pMat)
					pToReturn->AddMaterial(matIndex, pMat);
			}
			
			if(m_Parser.GetDescription())
			{
				pToReturn->SetDesc(m_Parser.GetDescription());
			}	
			
			if(pKey && pKey->IsValid())
				m_AllMatGroupTemplates[*pKey] = pToReturn;		
		}
	}

	return pToReturn;
}

IBFXMaterialGroup* 
CBFXResourceMgr::CreateMaterialGroup(const CRCDataKey* pKey, 
									IBFXMaterialGroupTemplate* pTemplate,
									const REffectParam* pConstantParams,
									const uint* pSemantics,
									uint numParams)
{
	CBFXMaterialGroup* pToReturn = NULL;
	if(pKey)
	{
		MatGroupMap::iterator itr = m_AllMatGroups.find(*pKey);
		if(itr != m_AllMatGroups.end())
			return itr->second;

		if(pTemplate)
		{
			pToReturn = _NEW CBFXMaterialGroup(m_pBase, pKey, (CBFXMaterialGroupTemplate*) pTemplate);
			_LOOPi(numParams)
			{
				pToReturn->AddParam(&pConstantParams[i], pSemantics[i]);
			}

			if(pKey->IsValid())
				m_AllMatGroups.insert( std::make_pair(*pKey, pToReturn) );
		}
	}
	else
	{
		if(pTemplate)
		{
			pToReturn = _NEW CBFXMaterialGroup(m_pBase, pKey, (CBFXMaterialGroupTemplate*) pTemplate);
			_LOOPi(numParams)
			{
				pToReturn->AddParam(&pConstantParams[i], pSemantics[i]);
			}
		}
	}
	
	return pToReturn;
}

IBFXPipeline* 
CBFXResourceMgr::CreatePipeline()
{
	CBFXPipeline* pPipeline = _NEW CBFXPipeline();
	pPipeline->SetBaseFX(m_pBase);
	pPipeline->Initialize();
	return pPipeline;
}

IBFXParamPool* 
CBFXResourceMgr::CreateParamPool(IBFXSharedParamData* pSharedParam)
{
	if(!pSharedParam)
		pSharedParam = CreateSharedParamData();

	CBFXParamPool* pParamPool = _NEW CBFXParamPool( (CBFXSharedParamData*) pSharedParam );
	pParamPool->SetBaseFX(m_pBase);
	return pParamPool;
}

IBFXSharedParamData* CBFXResourceMgr::CreateSharedParamData()
{
	CBFXSharedParamData* pSharedParam = _NEW CBFXSharedParamData;
	return pSharedParam;
}

void 
CBFXResourceMgr::NotifyMaterialGroupTemplateDestroyed(const CRCDataKey& resID, CBFXMaterialGroupTemplate* pTemplate)
{
	if(resID.IsValid())
	{
		MatGroupTemplateMap::iterator itr = m_AllMatGroupTemplates.find(resID);
		_DEBUG_ASSERT(itr != m_AllMatGroupTemplates.end());
		_DEBUG_ASSERT(itr->second == pTemplate);
		m_AllMatGroupTemplates.erase(itr);
	}
}


void 
CBFXResourceMgr::NotifyMaterialGroupDestroyed(const CRCDataKey& resID, CBFXMaterialGroup* pGroup)
{
	if(resID.IsValid())
	{
		MatGroupMap::iterator itr = m_AllMatGroups.find(resID);	
		_DEBUG_ASSERT(itr != m_AllMatGroups.end());
		_DEBUG_ASSERT(itr->second == pGroup);
		m_AllMatGroups.erase(itr);
		
		//std::pair<MatGroupMultiMap::iterator, MatGroupMultiMap::iterator> range = m_AllMatGroups.equal_range(templateID);
		//MatGroupMultiMap::iterator itr; 
		//for(itr = range.first; itr != range.second; ++itr)
		//{
		//	if(itr->second == pGroup)
		//	{
		//		m_AllMatGroups.erase(itr);
		//		return;
		//	}
		//}
		//
		//_DEBUG_ASSERT(FALSE);
	}	
}	

void 
CBFXResourceMgr::NotifyMaterialDestroyed(const CRCDataKey& resID, CBFXMaterial* pMat)
{
	// The id is always valid
	_DEBUG_ASSERT(resID.IsValid());
	
	MatMap::iterator itr = m_AllMats.find(resID);
	_DEBUG_ASSERT(itr != m_AllMats.end());
	_DEBUG_ASSERT(itr->second == pMat);
	m_AllMats.erase(itr);
}
	
_NAMESPACE_END