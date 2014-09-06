//==============================================================================
//
//		CDFXDebugFX.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/23/2008
//
//==============================================================================

#include "DebugFXFramework.h"

_NAMESPACE_BEGIN

int  SortLess(const void* pL, const void* pR)
{
	const RRenderOp* pLhs = *((const RRenderOp**) pL);
	const RRenderOp* pRhs = *((const RRenderOp**) pR);

	// Sort by shaders
	if( pLhs->pEffectTemplate != pRhs->pEffectTemplate )
		return (pLhs->pEffectTemplate < pRhs->pEffectTemplate) ? 1 : -1;
	//if( pLhs->technique != pRhs->technique ) 
	//	return (pLhs->technique < pRhs->technique) ? 1 : -1;
	//if( pLhs->pass != pRhs->pass ) 
	//	return pLhs->pass < pRhs->pass;

	// Sort by constant params
	if( pLhs->pEffect != pRhs->pEffect )
		return (pLhs->pEffect < pRhs->pEffect) ? 1 : -1;

	//if( pLhs->pVBGroup != pRhs->pVBGroup )
	//	return pLhs->pVBGroup < pRhs->pVBGroup;

	//if( pLhs->pIB != pRhs->pIB )
	//	return pLhs->pIB < pRhs->pIB; 

	if( pLhs->opHash != pRhs->opHash )
		return (pLhs->opHash < pRhs->opHash) ? 1 : -1;

    // Front to back
    if(pLhs->userData.fVal != pRhs->userData.fVal )
        return (pLhs->userData.fVal > pRhs->userData.fVal) ? 1 : -1;

    return 0;
}

void 
CDFXDebugFX::BeginWireFrame()
{
	m_pWireFrame->GetTemplate()->BeginTechnique(0);
	m_pWireFrame->GetTemplate()->BeginPass(0);
}

void 
CDFXDebugFX::EndWireFrame()
{
	m_pWireFrame->GetTemplate()->EndPass();
	m_pWireFrame->GetTemplate()->EndTechnique();
}


boolean
CDFXDebugFX::Initialize(IRRenderer& renderer, IAppCallback& callback)
{
	m_pRenderer = &renderer;
	m_pAppCallback = &callback;
	
	m_ResMgr.Initialize();
	
	IRResourceMgr& resMgr = m_pRenderer->GetRResourceMgr();

	wstring str = _W("[shd]\\DebugFX\\WireFrameSetup.fx");
	CRCDataKey id;
	id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	IByteBufferPtr pFileBuffer = m_pAppCallback->GetFileData(str.c_str());
	if(pFileBuffer)
	{
		m_pWireFrame = resMgr.LoadEffect(&id, 
			pFileBuffer,
			NULL,
			NULL,
			0);
	}

	m_pRenderGroup = resMgr.CreateRenderGroup(SortLess);
	m_Params.ResetParams();

	return true;
}

void 
CDFXDebugFX::RenderWireBox(const AABox& box,
						const gmtl::Vec4f& color)
{	
	gmtl::Vec3f extents = _CAST_VEC3(box.max) - _CAST_VEC3(box.min);
	extents *= 0.5f;
	gmtl::Vec3f pos = _CAST_VEC3(box.min) + extents;
	
	gmtl::Matrix44f scale;
	gmtl::identity(scale);
	//gmtl::Matrix44f world;
	
	scale[0][0] = extents[0];
	scale[1][1] = extents[1];
	scale[2][2] = extents[2];
	scale[0][3] = pos[0];
	scale[1][3] = pos[1];
	scale[2][3] = pos[2];

	//m_Params.AddBatchedWireBoxParams(*(MatMatMult(&scale, &transform, &scale)), color);
	m_Params.AddBatchedWireBoxParams(scale, color);
}		
		
void 
CDFXDebugFX::RenderWireBox(const OOBox& box,
						   const gmtl::Vec4f& color)
{
	gmtl::Vec3f halfXExtents = _CAST_VEC3(box.xExtents) * 0.5f;
	gmtl::Vec3f halfYExtents = _CAST_VEC3(box.yExtents) * 0.5f;
	gmtl::Vec3f halfZExtents = _CAST_VEC3(box.zExtents) * 0.5f;

	gmtl::Vec3f pos = _CAST_VEC3(box.corner) + halfXExtents + halfYExtents + halfZExtents; 
	
	float lengthX = gmtl::normalize(halfXExtents);
	float lengthY = gmtl::normalize(halfYExtents);
	float lengthZ = gmtl::normalize(halfZExtents);

	gmtl::MatrixA44f scale;
	gmtl::identity(_CAST_MAT44(scale));
	scale[0][0] = lengthX;
	scale[1][1] = lengthY;
	scale[2][2] = lengthZ;

	gmtl::MatrixA44f rot;
	gmtl::identity(_CAST_MAT44(rot));
	rot[0][0] = halfXExtents[0]; rot[0][1] = halfXExtents[1]; rot[0][2] = halfXExtents[2];
	rot[1][0] = halfYExtents[0]; rot[1][1] = halfYExtents[1]; rot[1][2] = halfYExtents[2];
	rot[2][0] = halfZExtents[0]; rot[2][1] = halfZExtents[1]; rot[2][2] = halfZExtents[2];
	gmtl::invert(_CAST_MAT44(rot));
	
	MatMatMult(&rot, &rot, &scale);
	rot[0][3] = pos[0];
	rot[1][3] = pos[1];
	rot[2][3] = pos[2];

	m_Params.AddBatchedWireBoxParams(_CAST_MAT44(rot), color);
}						   

void 
CDFXDebugFX::RenderWireSphere(const gmtl::Vec3f& pos,
						  float radius,
						  const gmtl::Vec4f& color)
{
	gmtl::Matrix44f scale;
	//gmtl::Matrix44f world;
	gmtl::identity(scale);
	scale[0][0] = radius;
	scale[1][1] = radius;
	scale[2][2] = radius;
	scale[0][3] = pos[0];
	scale[1][3] = pos[1];
	scale[2][3] = pos[2];

	m_Params.AddBatchedWireSphereParams(scale, color);
}

void 
CDFXDebugFX::RenderQuad(const gmtl::Vec3f& pos, 
						const gmtl::Vec2f& size,
						const gmtl::Vec2f& topLeftTexPos,
						const gmtl::Vec2f& texSize,
						IRTexture2D* pTex)
{
	m_Params.AddBatchedQuadParams(pos, size, topLeftTexPos, texSize, pTex);
}
		
//void 
//CDFXDebugFX::RenderQuad(const gmtl::Vec3f& pos, 
//						const gmtl::Vec2f& size,
//						const gmtl::Vec2f& topLeftTexPos,
//						const gmtl::Vec2f& texSize,
//						IRTextureRT* pTex)
//{
//	m_Params.AddBatchedQuadParams(pos, size, topLeftTexPos, texSize, pTex);
//}
		
void 
CDFXDebugFX::RenderLine(const gmtl::Vec3f& pos1, const gmtl::Vec3f& pos2, const gmtl::Vec4f& color)
{
	m_Params.AddBatchedPointParams(pos1, color);
	m_Params.AddBatchedPointParams(pos2, color);
}
					  	
IRIndexBuffer* CDFXDebugFX::AllocSharedIBFromPool(SharedIndicesPool& pool, IRIndexBuffer* pSourceIB)
{
	if(pool.sharedIndices.size() <= pool.numSharedIndicesUsed)
	{
		IRIndexBuffer* pSharedIB = m_pRenderer->GetRResourceMgr().CreateIndexBufferShared(pSourceIB->GetNumIndices(), pSourceIB);
		pool.sharedIndices.push_back(pSharedIB);
	}

	IRIndexBuffer* pToReturn = pool.sharedIndices[pool.numSharedIndicesUsed];
	pool.numSharedIndicesUsed++;
	return pToReturn;
}

void 
CDFXDebugFX::Flush()
{
	REffectParam effectParams[RRenderOp::MAX_NUM_PARAMS];
	RVarData var;
	var.iVal = 0;

	uint numOfBatchesSoFar = 0;

	// Batch all wire boxes
	const gmtl::Matrix44f* pWorldBatch = m_Params.GetBatchedWorldMatricesWireBox();
	const gmtl::Vec4f* pColorBatch = m_Params.GetBatchedColorsWireBox();
	int numOfBatchedWireBox = (int) m_Params.GetNumOfBatchedWireBox();
	
	DFXRenderData& wireData = m_ResMgr.GetWireBoxRenderData();
	
	while(numOfBatchedWireBox > 0)
	{	
		uint curNumOfBatch = DFX_MAX_NUM_OF_BATCHES;
		if(numOfBatchedWireBox < DFX_MAX_NUM_OF_BATCHES)
		{
			curNumOfBatch = (int) numOfBatchedWireBox;
		}		
				
		m_Params.SetColorBatch(pColorBatch + numOfBatchesSoFar);
		m_Params.SetWorldBatchMatrix(pWorldBatch + numOfBatchesSoFar);
		m_Params.SetNumOfBatch(curNumOfBatch);
		
		uint tech = m_Params.InitializeEffect(*wireData.pEffect, 1.0f, effectParams);
		
		uint numIndices = curNumOfBatch * 24;
		IRIndexBuffer* pSharedIB = AllocSharedIBFromPool(m_WireBoxSharedIBPool, wireData.pIB);
		pSharedIB->SetValidRange(0, numIndices);

		m_pRenderGroup->AddDrawOp(wireData.pVBGroup, pSharedIB, wireData.pEffect, effectParams, tech, var, PRIM_LINE);
		
		numOfBatchedWireBox -= curNumOfBatch;
		numOfBatchesSoFar += curNumOfBatch;
	}
	
	numOfBatchesSoFar = 0;
	
	// Batch wire spheres
	pWorldBatch = m_Params.GetBatchedWorldMatricesWireSphere();
	pColorBatch = m_Params.GetBatchedColorsWireSphere();
	int numOfBatchedWireSphere = (int) m_Params.GetNumOfBatchedWireSphere();

	DFXRenderData& wireSphereData = m_ResMgr.GetWireSphereRenderData();

	while(numOfBatchedWireSphere > 0)
	{	
		uint curNumOfBatch = DFX_MAX_NUM_OF_BATCHES;
		if(numOfBatchedWireSphere < DFX_MAX_NUM_OF_BATCHES)
		{
			curNumOfBatch = (int) numOfBatchedWireSphere;
		}		

		m_Params.SetColorBatch(pColorBatch + numOfBatchesSoFar);
		m_Params.SetWorldBatchMatrix(pWorldBatch + numOfBatchesSoFar);
		m_Params.SetNumOfBatch(curNumOfBatch);

		uint tech = m_Params.InitializeEffect(*wireSphereData.pEffect, 1.0f, effectParams);

		uint numIndices = curNumOfBatch * 6 * DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE;
		IRIndexBuffer* pSharedIB = AllocSharedIBFromPool(m_WireSphereSharedIBPool, wireSphereData.pIB);
		pSharedIB->SetValidRange(0, numIndices);

		m_pRenderGroup->AddDrawOp(wireSphereData.pVBGroup, pSharedIB, wireSphereData.pEffect, effectParams, tech, var, PRIM_LINE);

		numOfBatchedWireSphere -= curNumOfBatch;
		numOfBatchesSoFar += curNumOfBatch;
	}
	
	numOfBatchesSoFar = 0;
	
	// Batch lines
	const gmtl::Vec3f* pPosBatch = m_Params.GetBatchedPosPoint();
	pColorBatch = m_Params.GetBatchedColorsPoint();
	int numOfBatchedPoints = (int) m_Params.GetNumOfBatchedPoint();

	DFXRenderData& wireLineData = m_ResMgr.GetLineRenderData();

	while(numOfBatchedPoints > 0)
	{	
		uint curNumOfBatch = DFX_MAX_NUM_OF_BATCHES;
		if(numOfBatchedPoints < DFX_MAX_NUM_OF_BATCHES)
		{
			curNumOfBatch = (int) numOfBatchedPoints;
		}		

		m_Params.SetColorBatch(pColorBatch + numOfBatchesSoFar);
		m_Params.SetPosBatch(pPosBatch + numOfBatchesSoFar);
		m_Params.SetNumOfBatch(curNumOfBatch);

		uint tech = m_Params.InitializeEffect(*wireLineData.pEffect, 1.0f, effectParams);

		uint numIndices = curNumOfBatch * 2;
		IRIndexBuffer* pSharedIB = AllocSharedIBFromPool(m_LineSharedIBPool, wireLineData.pIB);
		pSharedIB->SetValidRange(0, numIndices);

		m_pRenderGroup->AddDrawOp(wireLineData.pVBGroup, pSharedIB, wireLineData.pEffect, effectParams, tech, var, PRIM_LINE);

		numOfBatchedPoints -= curNumOfBatch;
		numOfBatchesSoFar += curNumOfBatch;
	}
	
	// Batch textured quads
	uint numBatches = m_Params.GetNumOfBatchedQuadParams();
	DFXRenderData& quadData = m_ResMgr.GetQuadRenderData();
	_LOOPi(numBatches)
	{
		REffectParam* pTexParam = m_Params.GetQuadTexture(i);
		BatchedQuadParams* pParam = m_Params.GetBatchedQuadParams((size_t) pTexParam->pTex);
		uint numBatchedQuads = (uint) pParam->quadPos.size();
		
		numOfBatchesSoFar = 0;
	
		while(numBatchedQuads > 0)
		{
			uint curNumOfBatch = DFX_MAX_NUM_OF_BATCHES;
			if(numBatchedQuads < DFX_MAX_NUM_OF_BATCHES)
			{
				curNumOfBatch = (int) numBatchedQuads;
			}		

			m_Params.SetPosBatch(&(pParam->quadPos[0])+ numOfBatchesSoFar);
			m_Params.SetQuadSizeBatch(&(pParam->quadSize[0]) + numOfBatchesSoFar);
			m_Params.SetQuadTexPosBatch(&(pParam->quadTexPos[0]) + numOfBatchesSoFar);
			m_Params.SetQuadTexSizeBatch(&(pParam->quadTexSize[0]) + numOfBatchesSoFar);
			m_Params.SetQuadTexture(pTexParam);
			m_Params.SetNumOfBatch(curNumOfBatch);

			uint tech = m_Params.InitializeEffect(*quadData.pEffect, 1.0f, effectParams);

			uint numIndices = curNumOfBatch * 6;
			IRIndexBuffer* pSharedIB = AllocSharedIBFromPool(m_QuadSharedIBPool, quadData.pIB);
			pSharedIB->SetValidRange(0, numIndices);

			m_pRenderGroup->AddDrawOp(quadData.pVBGroup, pSharedIB, quadData.pEffect, effectParams, tech, var, PRIM_TRIANGLE);

			numBatchedQuads -= curNumOfBatch;
			numOfBatchesSoFar += curNumOfBatch;
		}
		
	}
	
	// Draw now
	m_pRenderGroup->Flush();
	m_pRenderGroup->Reset();

	// Reset all shared indices to be reused
	m_WireBoxSharedIBPool.numSharedIndicesUsed = 0;
	m_WireSphereSharedIBPool.numSharedIndicesUsed = 0;
	m_LineSharedIBPool.numSharedIndicesUsed = 0;
	m_QuadSharedIBPool.numSharedIndicesUsed = 0;

	m_Params.ResetParams();
}

void CDFXDebugFX::SetViewport(uint left, uint top, uint width, uint height)
{
	m_pRenderGroup->SetViewport(left, top, width, width);
}

void CDFXDebugFX::AddRenderTarget(IRTextureRT* pTexRT)
{
	m_pRenderGroup->AddRenderTarget(pTexRT);
}

void CDFXDebugFX::ClearRenderTargets()
{
	m_pRenderGroup->ClearRenderTargets();
}

void CDFXDebugFX::SetDepthBuffer(IRDepthBuffer* pDepth)
{
	m_pRenderGroup->SetDepthBuffer(pDepth);
}


_NAMESPACE_END

