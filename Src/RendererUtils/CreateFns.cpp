//==============================================================================
//
//		CreateFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		22/11/09
//
//==============================================================================

#include "RendererUtils.h"

_NAMESPACE_BEGIN


void 
CreateSphere(IRRenderer* pRenderer,
			 IRVertexBufferGroup** ppVBGroupDest, 
			  IRIndexBuffer** ppIBDest, 
			  float radius, 
			  uint numThetaSegments, 
			  uint numPhiSegments)
{
	struct SphereVert
	{
		float pos[3];
		float normal[3];
		float uv[2];
	};

	RVertexDesc desc[16];
	desc[0].usage		= VDU_POSITION;
	desc[0].usageIndex	= 0;
	desc[1].usage		= VDU_NORMAL;
	desc[1].usageIndex	= 0;
	desc[2].usage		= VDU_TEXCOORDF2;
	desc[2].usageIndex	= 0;
	desc[3].usage		= VDU_END;
	desc[3].usageIndex	= 0;

	uint numThetaVertices = numThetaSegments + 1;
	uint numPhiVertices = numPhiSegments + 1;

	IRVertexBufferGroup* pVBGroup = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
	IRVertexBuffer* pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(
		NULL, 
		desc,
		numThetaVertices * numPhiVertices, 
		BUF_DEFAULT);

	float phiInc		= gmtl::Math::TWO_PI / numPhiSegments;
	float thetaInc		= gmtl::Math::PI / numThetaSegments;
	gmtl::Vec2f polar;
	polar[0]	= 0.0f;
	polar[1]	= 0.0f;

	SphereVert* pVertices = (SphereVert*) pVB->Lock(0, pVB->GetNumVertices());

	_LOOPi(numPhiVertices)
	{
		_LOOPj(numThetaVertices)
		{
			SphericalToCartesian(*(gmtl::Vec3f*)(pVertices->normal), polar);
			pVertices->pos[0]	= pVertices->normal[0] * radius;
			pVertices->pos[1]	= pVertices->normal[1] * radius;
			pVertices->pos[2]	= pVertices->normal[2] * radius;

			pVertices->uv[0]	= ((float)i) / (numPhiVertices - 1.0f);
			pVertices->uv[1]	= ((float)j) / (numThetaVertices - 1.0f);

			pVertices++;

			polar[0] += thetaInc;
		}

		polar[0] = 0.0f;
		polar[1] += phiInc;
	}

	pVB->Unlock();

	pVBGroup->AddVertexBuffer(pVB);

	IRIndexBuffer* pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(
		NULL,
		6 * numThetaSegments * numPhiSegments, 
		BUF_DEFAULT);

	ushort* pIndices = pIB->Lock(0, pIB->GetNumIndices());

	_LOOPi(numPhiSegments)
	{
		_LOOPj(numThetaSegments)
		{
			// Each latitude segment (theta) has got 6 indices
			uint curIndex = (i * numThetaSegments + j) * 6;

			pIndices[curIndex]		= i * numThetaVertices + j;
			pIndices[curIndex+1]	= (i+1) * numThetaVertices + j;
			pIndices[curIndex+2]	= i * numThetaVertices + 1 + j;

			pIndices[curIndex+3]	= i * numThetaVertices + 1 + j;
			pIndices[curIndex+4]	= (i+1) * numThetaVertices + j;
			pIndices[curIndex+5]	= (i+1) * numThetaVertices + 1 + j;
		}
	}

	pIB->Unlock();

	RUPreTransVertCacheOpt preOpt;
	preOpt.Optimize(pVBGroup, pIB);
	RUPostTransVertCacheOpt postOpt;
	postOpt.Optimize(pIB, pVBGroup->GetVertexBuffer(0)->GetNumVertices());

	*ppVBGroupDest	= pVBGroup;
	*ppIBDest		= pIB;

}

_NAMESPACE_END
