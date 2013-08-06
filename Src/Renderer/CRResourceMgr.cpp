//==============================================================================
//
//		CRResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, March 21, 2006
//
//==============================================================================

#include "RendererFramework.h"

_NAMESPACE_BEGIN

struct EffectGroupCompare
{
	CREffectTemplate* pTemplate;
	const REffectParam* pParams;
	uint numParams;
	
	EffectGroupCompare(CREffectTemplate* pTemplate, const REffectParam* pParams, uint numParams)
	{
		this->pTemplate = pTemplate;
		this->pParams	= pParams;
		this->numParams	= numParams;
	}
	
	boolean operator==(CREffect& effect)
	{
		if(pTemplate != effect.GetTemplate())
			return FALSE;
		
		if(numParams != effect.GetNumOfParams())
			return FALSE;
			
		_LOOPi(numParams)
		{
			const REffectParamStore* pCurStore = effect.GetParamStore(i);
			if(*pCurStore != pParams[i])
				return FALSE;
		}
		
		return TRUE;
	}	
};

#define __PROCESS_RESOURCES( Fn ) \
{ \
	EffectTemplateMap::iterator eitr = m_EffectTemplateStash.begin(); \
	while(eitr != m_EffectTemplateStash.end()) \
	{ \
		eitr->second->Fn(); \
		++eitr; \
	} \
 \
	Texture2DMap::iterator titr = m_Texture2DStash.begin(); \
	while(titr != m_Texture2DStash.end()) \
	{ \
		titr->second->Fn(); \
		++titr; \
	} \
\
	{ \
		TextureCubeMap::iterator iitr = m_CubeStash.begin(); \
		while(iitr != m_CubeStash.end()) \
		{ \
			iitr->second->Fn(); \
			++iitr; \
		} \
	} \
\
	VertexBufferMap::iterator vbitr = m_VBStash.begin(); \
	while(vbitr != m_VBStash.end()) \
	{ \
		vbitr->second->Fn(); \
		++vbitr; \
	} \
\
	VertexBufferGroupMap::iterator vbgitr = m_VBGroupStash.begin(); \
	while(vbgitr != m_VBGroupStash.end()) \
	{ \
		vbgitr->second->Fn(); \
		++vbgitr; \
	} \
\
	IndexBufferMap::iterator ibitr = m_IBStash.begin(); \
	while(ibitr != m_IBStash.end()) \
	{ \
		ibitr->second->Fn(); \
		++ibitr; \
	} \
\
	TextureRTList::iterator rtitr = m_TextureRTStash.begin(); \
	while(rtitr != m_TextureRTStash.end()) \
	{ \
		(*rtitr)->Fn(); \
		++rtitr; \
	} \
	\
\
	DepthBufferList::iterator ditr = m_DepthBufferStash.begin(); \
	while(ditr != m_DepthBufferStash.end()) \
	{ \
		(*ditr)->Fn(); \
		++ditr; \
	} \
	\
\
	VertexBufferList::iterator vbnoiditr = m_VBNoIDStash.begin(); \
	while(vbnoiditr != m_VBNoIDStash.end()) \
	{ \
		(*vbnoiditr)->Fn(); \
		++vbnoiditr; \
	} \
\
	VertexBufferGroupList::iterator vbgnoiditr = m_VBGroupNoIDStash.begin(); \
	while(vbgnoiditr != m_VBGroupNoIDStash.end()) \
	{ \
		(*vbgnoiditr)->Fn(); \
		++vbgnoiditr; \
	} \
\
	IndexBufferList::iterator idnoiditr = m_IBNoIDStash.begin(); \
	while(idnoiditr != m_IBNoIDStash.end()) \
	{ \
		(*idnoiditr)->Fn(); \
		++idnoiditr; \
	} \
\
	EffectList::iterator effnoiditr = m_EffectNoIDStash.begin(); \
	while(effnoiditr != m_EffectNoIDStash.end()) \
	{ \
		(*effnoiditr)->Fn(); \
		++effnoiditr; \
	} \
\
	EffectTemplateList::iterator etnoiditr = m_EffectTemplateNoIDStash.begin(); \
	while(etnoiditr != m_EffectTemplateNoIDStash.end()) \
	{ \
		(*etnoiditr)->Fn(); \
		++etnoiditr; \
	} \
\
	Texture2DList::iterator texnoiditr = m_Texture2DNoIDStash.begin(); \
	while(texnoiditr != m_Texture2DNoIDStash.end()) \
	{ \
		(*texnoiditr)->Fn(); \
		++texnoiditr; \
	} \
\
	{ \
		TextureCubeList::iterator iitr = m_CubeNoIDStash.begin(); \
		while(iitr != m_CubeNoIDStash.end()) \
		{ \
			(*iitr)->Fn(); \
			++iitr; \
		} \
	} \
}

void 
CRResourceMgr::OnLostDevice()
{
	__PROCESS_RESOURCES( OnLostDevice );
	DoOnLostDevice();
}

void 
CRResourceMgr::OnResetDevice()
{
	//_TRACE(_W("CRResouceMgr::OnResetDEvice()"));
	__PROCESS_RESOURCES( OnResetDevice );
	DoOnResetDevice();
}

void 
CRResourceMgr::OnRestoreDevice()
{
	__PROCESS_RESOURCES( OnRestoreDevice );
	DoOnRestoreDevice();
}

void
CRResourceMgr::OnDestroyResource(const CRCDataKey& id, eRResourceType type, IRResource* pRes)
{
	_DEBUG_ASSERT(pRes);
	
	switch(type)
	{
		case RT_EFFECT_TEMPLATE:
		{
			if(id.IsValid())
			{			
				EffectTemplateMap::iterator eitr = 
						m_EffectTemplateStash.find(id);
				if(eitr != m_EffectTemplateStash.end())
				{
					_DEBUG_ASSERT(eitr->second == pRes);
					m_EffectTemplateStash.erase(eitr);
					return;
				}
			}
			else
			{
				_ITER_STLi(EffectTemplateList, m_EffectTemplateNoIDStash)
				{
					if(*iitr == pRes)
					{
						m_EffectTemplateNoIDStash.erase(iitr);
						return;
					}
				}
			}	
		}break;
				
		case RT_EFFECT:
		{
			if(id.IsValid())
			{
				EffectMultiMap::iterator emitr = m_EffectStash.find(id);
				if(emitr != m_EffectStash.end())
				{
					std::vector<CREffect*>& effects = emitr->second;
					uint numEffects = (uint) effects.size();
					_LOOPi(numEffects)
					{
						if(effects[i] == pRes)
						{
							effects.erase(effects.begin()+i);
							if(effects.empty())
								m_EffectStash.erase(emitr);
					
							return;
						}
					}
				}
			}	
			else
			{
				_ITER_STLi(EffectList, m_EffectNoIDStash)
				{
					if(*iitr == pRes)
					{
						m_EffectNoIDStash.erase(iitr);
						return;
					}
				}
			}		
		}break;
			
		case RT_TEXTURE_2D:
		{
			if(id.IsValid())
			{			
				Texture2DMap::iterator iitr = 
					m_Texture2DStash.find(id);
				if(iitr != m_Texture2DStash.end())
				{
					_DEBUG_ASSERT(iitr->second ==  pRes);
					m_Texture2DStash.erase(iitr);
					return;
				}
			}
			else
			{
				_ITER_STLi(Texture2DList, m_Texture2DNoIDStash)
				{
					if(*iitr == pRes)
					{
						m_Texture2DNoIDStash.erase(iitr);
						return;
					}
				}
			}	
		}break;
		
		case RT_TEXTURE_CUBE:
		{
			if(id.IsValid())
			{			
				TextureCubeMap::iterator iitr = 
					m_CubeStash.find(id);
				if(iitr != m_CubeStash.end())
				{
					_DEBUG_ASSERT(iitr->second == pRes);
					m_CubeStash.erase(iitr);
					return;
				}
			}
			else
			{
				_ITER_STLi(TextureCubeList, m_CubeNoIDStash)
				{
					if(*iitr == pRes)
					{
						m_CubeNoIDStash.erase(iitr);
						return;
					}
				}
			}	
		}break;

		case RT_INDEX_BUFFER:
		{
			if(id.IsValid())
			{			
				IndexBufferMap::iterator iitr = 
					m_IBStash.find(id);
				if(iitr != m_IBStash.end())
				{
					_DEBUG_ASSERT(iitr->second ==  pRes);
					m_IBStash.erase(iitr);
					return;
				}
			}
			else
			{
				_ITER_STLi(IndexBufferList, m_IBNoIDStash)
				{
					if(*iitr == pRes)
					{
						m_IBNoIDStash.erase(iitr);
						return;
					}
				}
			}	
		}break;
		
		case RT_VERTEX_BUFFER:
		{
			if(id.IsValid())
			{			
				VertexBufferMap::iterator iitr = 
					m_VBStash.find(id);
				if(iitr != m_VBStash.end())
				{
					_DEBUG_ASSERT(iitr->second == pRes);
					m_VBStash.erase(iitr);
					return;
				}
			}
			else
			{
				_ITER_STLi(VertexBufferList, m_VBNoIDStash)
				{
					if(*iitr == pRes)
					{
						m_VBNoIDStash.erase(iitr);
						return;
					}
				}
			}	
		}break; 
		
		case RT_VERTEX_BUFFER_GROUP:
		{
			if(id.IsValid())
			{			
				VertexBufferGroupMap::iterator iitr = 
					m_VBGroupStash.find(id);
				if(iitr != m_VBGroupStash.end())
				{
					_DEBUG_ASSERT(iitr->second == pRes);
					m_VBGroupStash.erase(iitr);
					return;
				}
			}
			else
			{
				_ITER_STLi(VertexBufferGroupList, m_VBGroupNoIDStash)
				{
					if(*iitr == pRes)
					{
						m_VBGroupNoIDStash.erase(iitr);
						return;
					}
				}
			}	
		}break; 
		
		case RT_DEPTH_BUFFER:
		{
			_ITER_STLi(DepthBufferList, m_DepthBufferStash)
			{
				if(*iitr == pRes)
				{
					m_DepthBufferStash.erase(iitr);
					return;
				}
			}
		}break;
		
		case RT_TEXTURE_RT:
		{
			_ITER_STLi(TextureRTList, m_TextureRTStash)
			{
				if(*iitr == pRes)
				{
					m_TextureRTStash.erase(iitr);
					return;
				}
			}
		}break;
	}
	
	//_DEBUG_ASSERT(FALSE);
}

//MD5Digest
//CRResourceMgr::GenerateID(const wchar* pName)
//{	
//	MD5Digest id;
//
//	if(pName)
//	{
//		// CRC the file name
//		wstring name = pName;
//		std::transform( name.begin(), name.end(), name.begin(), towlower );
//
//		id.Set(name.c_str(), (uint)(name.size()) * sizeof(wchar));
//	}
//		
//	return id;
//}

IREffectTemplate* 
CRResourceMgr::LoadEffectTemplate(const CRCDataKey* pKey, 
								   IByteBuffer* pData)
{
	//_DEBUG_ASSERT(pData);
	
	CREffectTemplate* pTemplate = NULL;
	
	//MD5Digest id = GenerateID(pKey);
	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_EffectTemplateStash.find(*pKey) == m_EffectTemplateStash.end())
		{
			if(pData)
				pTemplate = DoLoadEffectTemplate(pData);
				
			if(pTemplate)
			{
				m_EffectTemplateStash.insert(std::make_pair( *pKey, pTemplate ));
				pTemplate->SetResourceParams(this, _W("effect_template"), pKey, RT_EFFECT_TEMPLATE);
			}
		}
		else
		{
			pTemplate = m_EffectTemplateStash[*pKey];
		}
	}
	else
	{
		if(pData)
			pTemplate = DoLoadEffectTemplate(pData);
	
		if(pTemplate)
		{
			m_EffectTemplateNoIDStash.push_back(pTemplate);
			pTemplate->SetResourceParams(this, _W("effect_template_no_id"), NULL, RT_EFFECT_TEMPLATE);
		}
	}
	
	return pTemplate;
}

IREffect* 
CRResourceMgr::LoadEffect(IREffectTemplate* pTemplate,
					 const REffectParam* pConstantParams,
					 const uint* pSemantics,
					 uint numParams)
{
	_DEBUG_ASSERT(pTemplate);
	
	EffectGroupCompare toCompare((CREffectTemplate*) pTemplate, pConstantParams, numParams);
	
	CREffect* pToReturn = NULL;

	const CRCDataKey& templateID = pTemplate->GetResourceID();
	if(templateID.pHashData)
	{
		EffectMultiMap::iterator itr = m_EffectStash.find(templateID);
		if(itr != m_EffectStash.end())
		{
			std::vector<CREffect*>& effects = itr->second;
			uint numEffects = (uint) effects.size();
			_LOOPi(numEffects)
			{
				if(toCompare == *(effects[i]))
				{
					return effects[i];
				}			
			}
			
			pToReturn = _NEW CREffect();
			pToReturn->SetTemplate((CREffectTemplate*) pTemplate);
			_LOOPi(numParams)
			{
				int index = pTemplate->GetConstantParamIndex(pSemantics[i]);
				if(index >= 0)
					pToReturn->SetParam(pConstantParams[i], index);
			}
			
			effects.push_back(pToReturn);
			pToReturn->SetResourceParams(this, _W("effect"), &templateID, RT_EFFECT);
		}
		else
		{
			pToReturn = _NEW CREffect();
			pToReturn->SetTemplate((CREffectTemplate*) pTemplate);
			_LOOPi(numParams)
			{
				int index = pTemplate->GetConstantParamIndex(pSemantics[i]);
				if(index >= 0)
					pToReturn->SetParam(pConstantParams[i], index);
			}
			
			m_EffectStash[templateID].push_back(pToReturn);
			pToReturn->SetResourceParams(this, _W("effect"), &templateID, RT_EFFECT);
		}
	}
	else
	{
		pToReturn = _NEW CREffect();
		pToReturn->SetTemplate((CREffectTemplate*) pTemplate);
		_LOOPi(numParams)
		{
			int index = pTemplate->GetConstantParamIndex(pSemantics[i]);
			if(index >= 0)
				pToReturn->SetParam(pConstantParams[i], index);
		}
		
		m_EffectNoIDStash.push_back(pToReturn);
		pToReturn->SetResourceParams(this, _W("effect_no_id"), NULL, RT_EFFECT);

	}

	return pToReturn;
}


IREffect* 
CRResourceMgr::LoadEffect(const CRCDataKey* pTemplateKey, 
						   IByteBuffer* pTemplateData,
						   const REffectParam* pConstantParams,
						   const uint* pSemantics,
						   uint numParams)
{
	//_DEBUG_ASSERT(pTemplateData);
	
	CREffectTemplate* pTemplate = (CREffectTemplate*) LoadEffectTemplate(pTemplateKey, pTemplateData);
	if(pTemplate == NULL)
		return NULL;

	return LoadEffect(pTemplate, pConstantParams, pSemantics, numParams);
}

void CRResourceMgr::SaveTexture2D(IRTexture2D* pTex, IByteBuffer* pData)
{
	DoSaveTexture2D(pTex, pData);
}

IRTexture2D* 
CRResourceMgr::LoadTexture2D(const CRCDataKey* pKey, IByteBuffer* pData)
{
	CRTexture2D* pTexture = NULL;
	
	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_Texture2DStash.find(*pKey) == m_Texture2DStash.end())
		{
			if(pData)
				pTexture = DoLoadTexture2D(pData);

			if(pTexture)
			{
				m_Texture2DStash.insert( std::make_pair( *pKey, pTexture ) );
				pTexture->SetResourceParams(this, _W("texture2D"), pKey, RT_TEXTURE_2D);
			}
		}
		else
		{
			pTexture = (CRTexture2D*) m_Texture2DStash[*pKey];
		}
	}
	else
	{
		if(pData)
			pTexture = DoLoadTexture2D(pData);

		if(pTexture)
		{
			m_Texture2DNoIDStash.push_back( pTexture );
			pTexture->SetResourceParams(this, _W("texture2D_no_id"), pKey, RT_TEXTURE_2D);
		}		
	}
		
	return pTexture;
}

IRTexture3D* CRResourceMgr::LoadTexture3D(const CRCDataKey* pKey, IByteBuffer* pData)
{
	CRTexture3D* pTexture = NULL;

	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_Texture3DStash.find(*pKey) == m_Texture3DStash.end())
		{
			if(pData)
				pTexture = DoLoadTexture3D(pData);

			if(pTexture)
			{
				m_Texture3DStash.insert( std::make_pair( *pKey, pTexture ) );
				pTexture->SetResourceParams(this, _W("texture3D"), pKey, RT_TEXTURE_3D);
			}
		}
		else
		{
			pTexture = (CRTexture3D*) m_Texture3DStash[*pKey];
		}
	}
	else
	{
		if(pData)
			pTexture = DoLoadTexture3D(pData);

		if(pTexture)
		{
			m_Texture3DNoIDStash.push_back( pTexture );
			pTexture->SetResourceParams(this, _W("texture3D_no_id"), pKey, RT_TEXTURE_3D);
		}		
	}

	return pTexture;
}

IRTextureCube* 
CRResourceMgr::LoadTextureCube(const CRCDataKey* pKey, IByteBuffer* pData)
{
	CRTextureCube* pTexture = NULL;

	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_CubeStash.find(*pKey) == m_CubeStash.end())
		{
			if(pData)
				pTexture = DoLoadTextureCube(pData);

			if(pTexture)
			{
				m_CubeStash.insert( std::make_pair( *pKey, pTexture ) );
				pTexture->SetResourceParams(this, _W("textureCube"), pKey, RT_TEXTURE_CUBE);
			}
		}
		else
		{
			pTexture = (CRTextureCube*) m_CubeStash[*pKey];
		}
	}
	else
	{
		if(pData)
			pTexture = DoLoadTextureCube(pData);

		if(pTexture)
		{
			m_CubeNoIDStash.push_back( pTexture );
			pTexture->SetResourceParams(this, _W("textureCube_no_id"), pKey, RT_TEXTURE_CUBE);
		}		
	}

	return pTexture;
}

IRDepthBuffer* 
CRResourceMgr::CreateDepthBuffer(uint width, 
								uint height, 
								eRDepthFormat format,
								uint multiSampleQuality)
{
	CRDepthBuffer* pDepthBuffer = NULL;
	
	pDepthBuffer = DoCreateDepthBuffer(width, height, format, multiSampleQuality);
	
	if(pDepthBuffer)
	{
		m_DepthBufferStash.push_back(pDepthBuffer);
		pDepthBuffer->SetResourceParams(this, _W("depthBuffer"), NULL, RT_DEPTH_BUFFER);
	}

	return pDepthBuffer;	
}

IRTextureCube* CRResourceMgr::CreateTextureCubeRT(uint size, eRTexFormat format)
{
	CRTextureCube* pRT = NULL;

	// If it is not in the stash, add it to the stash
	pRT = DoCreateTextureCubeRT(size, format);
	if(pRT)
	{
		m_CubeNoIDStash.push_back(pRT);
		pRT->SetResourceParams(this, _W("textureCubeRT"), NULL, RT_TEXTURE_CUBE);
	}

	return pRT;
}

IRTexture2D* 
CRResourceMgr::CreateTexture2DRT(uint width, uint height, uint mipLevels, eRTexFormat format)
{
	CRTexture2D* pRT = NULL;

	// If it is not in the stash, add it to the stash
	pRT = DoCreateTexture2DRT(width, height, mipLevels, format);
	if(pRT)
	{
		m_Texture2DNoIDStash.push_back(pRT);
		pRT->SetResourceParams(this, _W("texture2DRT"), NULL, RT_TEXTURE_2D);
	}
	
	return pRT;
}

IRTextureRT* CRResourceMgr::CreateBackBufferColor()
{
	CRTextureRT* pRT = NULL;

	// If it is not in the stash, add it to the stash
	pRT = DoCreateBackBufferColor();
	if(pRT)
	{
		m_TextureRTStash.push_back(pRT);
		pRT->SetResourceParams(this, _W("backbufferRT"), NULL, RT_TEXTURE_RT);
	}

	return pRT;
}

IRDepthBuffer* CRResourceMgr::CreateBackBufferDepth()
{
	CRDepthBuffer* pDepthBuffer = NULL;

	pDepthBuffer = DoCreateBackBufferDepth();

	if(pDepthBuffer)
	{
		m_DepthBufferStash.push_back(pDepthBuffer);
		pDepthBuffer->SetResourceParams(this, _W("backBufferDepth"), NULL, RT_DEPTH_BUFFER);
	}

	return pDepthBuffer;	
}


IRTextureCube* 
CRResourceMgr::CreateTextureCube(const CRCDataKey* pKey, 
								uint size, 
								uint mipMapLevel, 
								eRTexFormat format,
								eRTexUsage usage)
{
	CRTextureCube* pTexture = NULL;

	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_CubeStash.find(*pKey) == m_CubeStash.end())
		{
			pTexture = DoCreateTextureCube(size, mipMapLevel, format, usage);
			if(pTexture)
			{
				m_CubeStash.insert( std::make_pair( *pKey, pTexture ) );
				pTexture->SetResourceParams(this, _W("textureCube_create"), pKey, RT_TEXTURE_CUBE);
			}
		}
		else
		{
			pTexture = (CRTextureCube*) m_CubeStash[*pKey];
		}
	}
	else
	{
		pTexture = DoCreateTextureCube(size, mipMapLevel, format, usage);
		if(pTexture)
		{
			m_CubeNoIDStash.push_back( pTexture );
			pTexture->SetResourceParams(this, _W("textureCube_create_no_id"), NULL, RT_TEXTURE_CUBE);
		}
	}

	return pTexture;
}

IRTexture3D* CRResourceMgr::CreateTexture3D(const CRCDataKey* pKey, 
											uint width, 
											uint height,
											uint depth,
											uint mipMapLevel, 
											eRTexFormat format,
											eRTexUsage usage)
{
	//MD5Digest id = GenerateID(pKey);

	CRTexture3D* pTexture = NULL;

	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_Texture3DStash.find(*pKey) == m_Texture3DStash.end())
		{
			pTexture = DoCreateTexture3D(width, height, depth, mipMapLevel, format, usage);
			if(pTexture)
			{
				m_Texture3DStash.insert( std::make_pair( *pKey, pTexture ) );
				pTexture->SetResourceParams(this, _W("texture3D_create"), pKey, RT_TEXTURE_3D);
			}
		}
		else
		{
			pTexture = (CRTexture3D*) m_Texture2DStash[*pKey];
		}
	}
	else
	{
		pTexture = DoCreateTexture3D(width, height, depth, mipMapLevel, format, usage);
		if(pTexture)
		{
			m_Texture3DNoIDStash.push_back( pTexture );
			pTexture->SetResourceParams(this, _W("texture3D_create_no_id"), NULL, RT_TEXTURE_3D);
		}
	}

	return pTexture;
}

IRTexture2D* 
CRResourceMgr::CreateTexture2D(const CRCDataKey* pKey, 
							uint width, 
							uint height, 
							uint mipMapLevel, 
							eRTexFormat format,
							eRTexUsage usage)
{
	//MD5Digest id = GenerateID(pKey);

	CRTexture2D* pTexture = NULL;

	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_Texture2DStash.find(*pKey) == m_Texture2DStash.end())
		{
			pTexture = DoCreateTexture2D(width, height, mipMapLevel, format, usage);
			if(pTexture)
			{
				m_Texture2DStash.insert( std::make_pair( *pKey, pTexture ) );
				pTexture->SetResourceParams(this, _W("texture2D_create"), pKey, RT_TEXTURE_2D);
			}
		}
		else
		{
			pTexture = (CRTexture2D*) m_Texture2DStash[*pKey];
		}
	}
	else
	{
		pTexture = DoCreateTexture2D(width, height, mipMapLevel, format, usage);
		if(pTexture)
		{
			m_Texture2DNoIDStash.push_back( pTexture );
			pTexture->SetResourceParams(this, _W("texture2D_create_no_id"), NULL, RT_TEXTURE_2D);
		}
	}
	
	return pTexture;
}

IRVertexBuffer* 
CRResourceMgr::CreateVertexBuffer(const CRCDataKey* pKey, const RVertexDesc* pDesc, uint numVertices, eRBufferUsage usage)
{
	//MD5Digest id = GenerateID(pKey);

	CRVertexBuffer* pVB = NULL; 
	
	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_VBStash.find(*pKey) == m_VBStash.end())
		{
			if(numVertices <= 0 || pDesc == NULL)
			{
				return NULL;
			}
			
			_DEBUG_ASSERT(pDesc);
			_DEBUG_ASSERT(numVertices > 0);
			
			// Determine the size of a vertex
			uint vertexSize = 0;
			uint i = 0;
			while( pDesc[i].usage != VDU_END )
			{
				vertexSize += GetUsageSize(pDesc[i].usage);
				++i;
			}
			
			// Get the required buffer length
			uint bufferLen = numVertices * vertexSize;
				
			pVB = DoCreateVertexBuffer(bufferLen, usage);
			
			if(pVB)
			{
				pVB->Initialize(pDesc, numVertices, usage);		
				m_VBStash.insert( std::make_pair( *pKey, pVB ) );
				pVB->SetResourceParams(this, _W("vertex_buffer"), pKey, RT_VERTEX_BUFFER);
			}
		}
		else
		{
			pVB = m_VBStash[*pKey];
		}
	}
	else
	{
		if(numVertices <= 0 || pDesc == NULL)
		{
			return NULL;
		}
			
		_DEBUG_ASSERT(pDesc);
		_DEBUG_ASSERT(numVertices > 0);

		// Determine the size of a vertex
		uint vertexSize = 0;
		uint i = 0;
		while( pDesc[i].usage != VDU_END )
		{
			vertexSize += GetUsageSize(pDesc[i].usage);
			++i;
		}

		// Get the required buffer length
		uint bufferLen = numVertices * vertexSize;

		pVB = DoCreateVertexBuffer(bufferLen, usage);

		if(pVB)
		{
			pVB->Initialize(pDesc, numVertices, usage);		
			m_VBNoIDStash.push_back( pVB );
			pVB->SetResourceParams(this, _W("vertex_buffer_no_id"), NULL, RT_VERTEX_BUFFER);
		}
	}
	
	return pVB;
}

IRVertexBuffer* CRResourceMgr::CreateVertexBufferShared(const RVertexDesc* pDesc, uint numVertices, IRVertexBuffer* pSource)
{
	if(numVertices <= 0 || pDesc == NULL)
	{
		return NULL;
	}

	_DEBUG_ASSERT(pDesc);
	_DEBUG_ASSERT(numVertices > 0);

	// Determine the size of a vertex
	uint vertexSize = 0;
	uint i = 0;
	while( pDesc[i].usage != VDU_END )
	{
		vertexSize += GetUsageSize(pDesc[i].usage);
		++i;
	}

	// Get the required buffer length
	uint bufferLen = numVertices * vertexSize;
	CRVertexBuffer* pVB = DoCreateVertexBufferShared(bufferLen, (CRVertexBuffer*) pSource);
	if(pVB)
	{
		pVB->Initialize(pDesc, numVertices, pSource->GetUsage());		
		m_VBNoIDStash.push_back( pVB );
		pVB->SetResourceParams(NULL, _W("vertex_buffer_shared"), NULL, RT_VERTEX_BUFFER);
	}

	return pVB;
}

IRIndexBuffer* 
CRResourceMgr::CreateIndexBuffer(const CRCDataKey* pKey, uint numIndices, eRBufferUsage usage)
{
	//MD5Digest id = GenerateID(pKey);

	CRIndexBuffer* pIB = NULL; 

	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_IBStash.find(*pKey) == m_IBStash.end())
		{	
			if(numIndices <= 0)
			{
				return NULL;
			}
			
			uint bufferLen = numIndices * sizeof(ushort);
			pIB = DoCreateIndexBuffer(bufferLen, usage);
			if(pIB)
			{
				pIB->Initialize(numIndices, usage);				
				m_IBStash.insert( std::make_pair( *pKey, pIB ) );
				
				pIB->SetResourceParams(this, _W("index_buffer"), pKey, RT_INDEX_BUFFER);
			}
		}
		else
		{
			pIB = m_IBStash[*pKey];
		}
	}
	else
	{
		if(numIndices <= 0)
		{
			return NULL;
		}
		
		_DEBUG_ASSERT(numIndices > 0);

		uint bufferLen = numIndices * sizeof(ushort);
		pIB = DoCreateIndexBuffer(bufferLen, usage);
		if(pIB)
		{
			pIB->Initialize(numIndices, usage);				
			m_IBNoIDStash.push_back( pIB );
			pIB->SetResourceParams(this, _W("index_buffer_no_id"), NULL, RT_INDEX_BUFFER);
		}
	}
	
	return pIB;
}

IRIndexBuffer* CRResourceMgr::CreateIndexBufferShared(uint numIndices, IRIndexBuffer* pSource)
{
	if(numIndices <= 0)
	{
		return NULL;
	}

	_DEBUG_ASSERT(numIndices > 0);

	uint bufferLen = numIndices * sizeof(ushort);
	CRIndexBuffer* pIB = DoCreateIndexBufferShared(bufferLen, (CRIndexBuffer*) pSource);
	if(pIB)
	{
		pIB->Initialize(numIndices, pSource->GetUsage());	
		m_IBNoIDStash.push_back( pIB );
		pIB->SetResourceParams(NULL, _W("index_buffer_shared"), NULL, RT_INDEX_BUFFER);
	}

	return pIB;
}

IRVertexBufferGroup* 
CRResourceMgr::CreateVertexBufferGroup(const CRCDataKey* pKey)
{
	CRVertexBufferGroup* pVBGroup = NULL; 

	if(pKey && pKey->IsValid())
	{
		// If it is not in the stash, add it to the stash
		if(m_VBGroupStash.find(*pKey) == m_VBGroupStash.end())
		{	
			pVBGroup = DoCreateVertexBufferGroup();
			if(pVBGroup)
			{
				m_VBGroupStash.insert( std::make_pair( *pKey, pVBGroup ) );
				
				pVBGroup->SetResourceParams(this, _W("vb_group"), pKey, RT_VERTEX_BUFFER_GROUP);
			}
		}
		else
		{
			pVBGroup = m_VBGroupStash[*pKey];
		}
	}
	else
	{
		pVBGroup = DoCreateVertexBufferGroup();
		if(pVBGroup)
		{
			m_VBGroupNoIDStash.push_back( pVBGroup );
			pVBGroup->SetResourceParams(this, _W("vb_group_no_id"), NULL, RT_VERTEX_BUFFER_GROUP);
		}
	}
	
	return pVBGroup;
}

IRRenderGroup* 
CRResourceMgr::CreateRenderGroup(RenderOpSort sortFn, uint maxRenderOps)
{
	return DoCreateRenderGroup(sortFn, maxRenderOps);
}

_NAMESPACE_END