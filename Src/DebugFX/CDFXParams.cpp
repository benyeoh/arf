//==============================================================================
//
//		CDFXParams.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/27/2008
//
//==============================================================================

#include "DebugFXFramework.h"

_NAMESPACE_BEGIN

void 
CDFXParams::ResetParams()
{
	m_BatchedColorsWireBox.clear();
	m_BatchedWorldWireBox.clear();
	
	m_BatchedWorldWireSphere.clear();
	m_BatchedColorsWireSphere.clear();

	m_BatchedPosPoint.clear();
	m_BatchedColorsPoint.clear();
	
	m_NumOfBatch = 0;
	
	uint numBatchedQuad = (uint) m_BatchedQuadTextures.size();
	_LOOPi(numBatchedQuad)
	{
		BatchedQuadParams* pBatchedQuadParam = m_BatchedQuads[ (size_t) m_BatchedQuadTextures[i].pTex ];
		pBatchedQuadParam->quadPos.clear();
		pBatchedQuadParam->quadSize.clear();
		pBatchedQuadParam->quadTexPos.clear();
		pBatchedQuadParam->quadTexSize.clear();
	}
	
	m_BatchedQuads.clear_no_resize();
	m_BatchedQuadTextures.clear();
	m_BatchedQuadPool.Reset();
}

void 
CDFXParams::SetViewMatrix(const gmtl::Matrix44f& view)
{
	m_pView = &view;
	m_IsViewProjDirty = TRUE;
}

const gmtl::Matrix44f& 
CDFXParams::GetViewMatrix()
{
	return *m_pView;
}

void 
CDFXParams::SetProjMatrix(const gmtl::Matrix44f& proj)
{
	m_pProj = &proj;
	m_IsViewProjDirty = TRUE;
}

const gmtl::Matrix44f& 
CDFXParams::GetProjMatrix()
{
	return *m_pProj;
}

const gmtl::Matrix44f& 
CDFXParams::GetViewProjMatrix()
{
	if(m_IsViewProjDirty)
	{
		MatMatMult(&m_ViewProj, m_pProj, m_pView);
		m_IsViewProjDirty = FALSE;
	}	
	
	return m_ViewProj;
}

void 
CDFXParams::SetWorldBatchMatrix(const gmtl::Matrix44f* pWorld)
{
	m_pWorldBatch = pWorld;
}

const gmtl::Matrix44f*
CDFXParams::GetWorldBatchMatrix()
{
	return m_pWorldBatch;
}

void 
CDFXParams::SetColorBatch(const gmtl::Vec4f* pColor)
{
	m_pColorBatch = pColor;
}

const gmtl::Vec4f* 
CDFXParams::GetColorBatch()
{
	return m_pColorBatch;
}

void 
CDFXParams::SetPosBatch(const gmtl::Vec3f* pPos)
{
	m_pPosBatch = pPos;
}

const gmtl::Vec3f*
CDFXParams::GetPosBatch()
{
	return m_pPosBatch;
}

void 
CDFXParams::SetNumOfBatch(uint numOfBatch)
{
	m_NumOfBatch = numOfBatch;
}

uint 
CDFXParams::GetNumOfBatch()
{
	return m_NumOfBatch;
}

void 
CDFXParams::AddBatchedWireBoxParams(const gmtl::Matrix44f& world, const gmtl::Vec4f& color)
{	
	m_BatchedWorldWireBox.push_back(world);
	m_BatchedColorsWireBox.push_back(color);

	// Ensure that the parameter array when passed to the GPU is always available
	uint spaceToReserve = (uint) ((m_BatchedWorldWireBox.size() / DFX_MAX_NUM_OF_BATCHES) + 1) * DFX_MAX_NUM_OF_BATCHES;
	if(m_BatchedWorldWireBox.capacity() < spaceToReserve)
	{
		m_BatchedWorldWireBox.reserve(spaceToReserve);
		m_BatchedColorsWireBox.reserve(spaceToReserve);
	}
}

const gmtl::Matrix44f* 
CDFXParams::GetBatchedWorldMatricesWireBox()
{
	if(m_BatchedWorldWireBox.size() > 0)
		return &(m_BatchedWorldWireBox[0]);	
		
	return NULL;
}

const gmtl::Vec4f* 
CDFXParams::GetBatchedColorsWireBox()
{
	if(m_BatchedColorsWireBox.size() > 0)
		return &(m_BatchedColorsWireBox[0]);
	
	return NULL;
}

uint 
CDFXParams::GetNumOfBatchedWireBox()
{
	return (uint) m_BatchedWorldWireBox.size();
}

void 
CDFXParams::AddBatchedWireSphereParams(const gmtl::Matrix44f& world, const gmtl::Vec4f& color)
{	
	m_BatchedWorldWireSphere.push_back(world);
	m_BatchedColorsWireSphere.push_back(color);

	// Ensure that the parameter array when passed to the GPU is always available
	uint spaceToReserve = (uint) ((m_BatchedWorldWireSphere.size() / DFX_MAX_NUM_OF_BATCHES) + 1) * DFX_MAX_NUM_OF_BATCHES;
	if(m_BatchedWorldWireSphere.capacity() < spaceToReserve)
	{
		m_BatchedWorldWireSphere.reserve(spaceToReserve);
		m_BatchedColorsWireSphere.reserve(spaceToReserve);
	}
}

const gmtl::Matrix44f* 
CDFXParams::GetBatchedWorldMatricesWireSphere()
{
	if(m_BatchedWorldWireSphere.size() > 0)
		return &(m_BatchedWorldWireSphere[0]);	
		
	return NULL;
}

const gmtl::Vec4f* 
CDFXParams::GetBatchedColorsWireSphere()
{
	if(m_BatchedColorsWireSphere.size() > 0)
		return &(m_BatchedColorsWireSphere[0]);
		
	return NULL;
}

uint 
CDFXParams::GetNumOfBatchedWireSphere()
{
	return (uint) m_BatchedWorldWireSphere.size();
}

void 
CDFXParams::AddBatchedPointParams(const gmtl::Vec3f& pos, const gmtl::Vec4f& color)
{
	m_BatchedPosPoint.push_back(pos);
	m_BatchedColorsPoint.push_back(color);
	
	// Ensure that the parameter array when passed to the GPU is always available
	uint spaceToReserve = (uint) ((m_BatchedPosPoint.size() / DFX_MAX_NUM_OF_BATCHES) + 1) * DFX_MAX_NUM_OF_BATCHES;
	if(m_BatchedPosPoint.capacity() < spaceToReserve)
	{
		m_BatchedPosPoint.reserve(spaceToReserve);
		m_BatchedColorsPoint.reserve(spaceToReserve);
	}
}

const gmtl::Vec3f* 
CDFXParams::GetBatchedPosPoint()
{
	if(m_BatchedPosPoint.size() > 0)
		return &(m_BatchedPosPoint[0]);	

	return NULL;

}

const gmtl::Vec4f* 
CDFXParams::GetBatchedColorsPoint()
{
	if(m_BatchedColorsPoint.size() > 0)
		return &(m_BatchedColorsPoint[0]);	

	return NULL;
}

uint 
CDFXParams::GetNumOfBatchedPoint()
{
	return (uint) m_BatchedPosPoint.size();
}

void
CDFXParams::AddBatchedQuadVertexParams(size_t index, const gmtl::Vec3f& pos, const gmtl::Vec2f& size,
								const gmtl::Vec2f& texPos, const gmtl::Vec2f& texSize)
{
	BatchedQuadParams* pBatchedQuad = NULL;
	
	BatchedQuadMap::iterator itr = m_BatchedQuads.find(index);
	if(itr == m_BatchedQuads.end())
	{
		pBatchedQuad = m_BatchedQuadPool.Get();
		m_BatchedQuads[index] = pBatchedQuad;
	}
	else
	{
		pBatchedQuad = (BatchedQuadParams*) itr->second;
	}		
	
	// Ensure that the parameter array when passed to the GPU is always available
	uint spaceToReserve = (uint) ((pBatchedQuad->quadPos.size() / DFX_MAX_NUM_OF_BATCHES) + 1) * DFX_MAX_NUM_OF_BATCHES;
	if(m_BatchedPosPoint.capacity() < spaceToReserve)
	{
		pBatchedQuad->quadPos.reserve(spaceToReserve);
		pBatchedQuad->quadSize.reserve(spaceToReserve);
		pBatchedQuad->quadTexPos.reserve(spaceToReserve);
		pBatchedQuad->quadTexSize.reserve(spaceToReserve);
	}
	
	pBatchedQuad->quadPos.push_back(pos);
	pBatchedQuad->quadSize.push_back(size);
	pBatchedQuad->quadTexPos.push_back(texPos);
	pBatchedQuad->quadTexSize.push_back(texSize);	
}
							
void 
CDFXParams::AddBatchedQuadParams(const gmtl::Vec3f& pos, const gmtl::Vec2f& size,
								const gmtl::Vec2f& texPos, const gmtl::Vec2f& texSize,
								IRTexture2D* pTex)
{
	size_t index = (size_t) pTex;
	REffectParam param;
	param.SetTexture2D(pTex);
	m_BatchedQuadTextures.push_back(param);
	
	AddBatchedQuadVertexParams(index, pos, size, texPos, texSize);
}
	
//void 
//CDFXParams::AddBatchedQuadParams(const gmtl::Vec3f& pos, const gmtl::Vec2f& size,
//								const gmtl::Vec2f& texPos, const gmtl::Vec2f& texSize,
//								IRTexture2D* pTex)
//{
//	uint index = (uint) pTex;
//	REffectParam param;
//	param.SetTexture2D(pTex);
//	m_BatchedQuadTextures.push_back(param);
//	
//	AddBatchedQuadVertexParams(index, pos, size, texPos, texSize);
//}

BatchedQuadParams* 
CDFXParams::GetBatchedQuadParams(size_t index)
{
	BatchedQuadMap::iterator itr = m_BatchedQuads.find(index);
	if(itr != m_BatchedQuads.end())
	{
		return itr->second;
	}
	
	return NULL;
}

REffectParam* 
CDFXParams::GetQuadTexture(uint index)
{
	return &(m_BatchedQuadTextures[index]);
}

uint 
CDFXParams::GetNumOfBatchedQuadParams()
{
	return (uint) m_BatchedQuadTextures.size();
}	

void 
CDFXParams::SetQuadSizeBatch(const gmtl::Vec2f* pSize)
{
	m_pQuadSizeBatch = pSize;
}

const gmtl::Vec2f* 
CDFXParams::GetQuadSizeBatch()
{
	return m_pQuadSizeBatch;
}
	
void 
CDFXParams::SetQuadTexPosBatch(const gmtl::Vec2f* pPos)
{
	m_pQuadTexPosBatch = pPos;
}
	
const gmtl::Vec2f* 
CDFXParams::GetQuadTexPosBatch()
{
	return m_pQuadTexPosBatch;
}
	
void 
CDFXParams::SetQuadTexSizeBatch(const gmtl::Vec2f* pSize)
{
	m_pQuadTexSizeBatch = pSize;
}
	
const gmtl::Vec2f* 
CDFXParams::GetQuadTexSizeBatch()
{
	return m_pQuadTexSizeBatch;
}
	
void 
CDFXParams::SetQuadTexture(REffectParam* pTex)
{
	m_pQuadTex = pTex;
}

REffectParam*
CDFXParams::GetQuadTexture()
{
	return m_pQuadTex;
}
			
void 
CDFXParams::GetParameter(REffectParam& param, uint semantic)
{
	switch(semantic)
	{
		case DFX_VIEW:
		{
			param.SetMatrix44(&GetViewMatrix()); 
			break;
		}
		
		case DFX_PROJ:
		{
			param.SetMatrix44(&GetProjMatrix()); 
			break;
		}
		
		case DFX_VIEW_PROJ:
		{
			param.SetMatrix44(&GetViewProjMatrix()); 
			break;
		}
		
		case DFX_WORLD_BATCH:
		{
			param.SetValue((void*) GetWorldBatchMatrix(), sizeof(gmtl::Matrix44f) * GetNumOfBatch());
			break;
		}
		
		case DFX_COLOR_BATCH:
		{
			param.SetValue((void*) GetColorBatch(), sizeof(gmtl::Vec4f) * GetNumOfBatch());
			break;
		}
		
		case DFX_POS_BATCH:
		{
			param.SetValue((void*) GetPosBatch(), sizeof(gmtl::Vec3f) * GetNumOfBatch());
			break;
		}
		
		case DFX_QUAD_SIZE_BATCH:
		{
			param.SetValue((void*) GetQuadSizeBatch(), sizeof(gmtl::Vec2f) * GetNumOfBatch());
			break;
		}
		
		case DFX_QUAD_TEX_POS_BATCH:
		{
			param.SetValue((void*) GetQuadTexPosBatch(), sizeof(gmtl::Vec2f) * GetNumOfBatch());
			break;
		}
		
		case DFX_QUAD_TEX_SIZE_BATCH:
		{
			param.SetValue((void*) GetQuadTexSizeBatch(), sizeof(gmtl::Vec2f) * GetNumOfBatch());
			break;
		}
		
		case DFX_QUAD_TEXTURE:
		{
			REffectParam* pParam = GetQuadTexture();
			param = *pParam;
			break;
		}
		
		default: _DEBUG_ASSERT(FALSE);
	}
}

uint 
CDFXParams::InitializeEffect(IREffect& effect, float detailLevel, REffectParam* params)
{
	uint tech = (uint)((effect.GetNumOfTechs()-1) * detailLevel + 0.5f);
	uint numDynamicParams = effect.GetTemplate()->GetNumOfDynamicParams(tech);

	_LOOPi(numDynamicParams)
	{
		uint semantic = effect.GetTemplate()->GetDynamicParamSemantic(tech, i);
		GetParameter(params[i], semantic);
	}

	return tech;
}


_NAMESPACE_END