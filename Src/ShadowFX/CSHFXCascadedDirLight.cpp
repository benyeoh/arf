//==============================================================================
//
//		CSHFXCascadedShadowUtils.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/11/2008
//
//==============================================================================

#include "ShadowFXFramework.h"

_NAMESPACE_BEGIN

int DirLightCasterSortLess(const void* pL, const void* pR)
{
	const RRenderOp* pLhs = *((const RRenderOp**) pL);
	const RRenderOp* pRhs = *((const RRenderOp**) pR);

	// Sort by constant params
	if( pLhs->pEffect != pRhs->pEffect )
		return (pLhs->pEffect < pRhs->pEffect) ? 1 : -1;

	//if( pLhs->pVBGroup != pRhs->pVBGroup )
	//	return (pLhs->pVBGroup < pRhs->pVBGroup) ? 1 : -1;
	//		
	//if( pLhs->pIB != pRhs->pIB )
	//	return (pLhs->pIB < pRhs->pIB) ? 1 : -1; 

	//if( pLhs->opHash != pRhs->opHash )
	//	return (pLhs->opHash < pRhs->opHash) ? 1 : -1;
	
	// Front to back
    if(pLhs->userData.fVal != pRhs->userData.fVal )
	    return (pLhs->userData.fVal > pRhs->userData.fVal) ? 1 : -1;

    return 0;
}

IRRenderGroup* CSHFXCascadedDirLight::GetCasterGroup(uint cascade)
{
	return m_pCasterGroups[cascade];
}

void 
CSHFXCascadedDirLight::UpdateZSplits(float nearBias, float nearViewZ, float farViewZ, float farNearRatio)
{
	float lastFarDist = 0.0f;
	float lastFarDistView = nearViewZ;
	
	// This value changes the shape of the exponential curve
	// The larger the farNearRatio, the more steep the curve
	float nearStart = farViewZ / farNearRatio;

	_LOOPi(4)
	{
		float ratio = (i+1.0f) / 4;	

		//float diff = farViewZ - nearViewZ;
		//float expDist = nearViewZ + diff * gmtl::Math::pow(ratio, nearBias);
		float expDist = nearStart * gmtl::Math::pow(farNearRatio, ratio);
		float linearDist = nearViewZ + ratio * (farViewZ - nearViewZ);
		
		//float zDist = expDist;
		float zDist = (nearBias) * expDist + (1.0f - nearBias) * linearDist;
		
		// 0 to 1
		m_Split.splitFar[i] = (zDist - nearViewZ) / (farViewZ - nearViewZ);
		m_Split.splitNear[i] = lastFarDist;
		lastFarDist = m_Split.splitFar[i];

		// nearZ to farZ
		m_SplitView.splitFar[i] = abs(zDist);
		m_SplitView.splitNear[i] = abs(lastFarDistView);
		lastFarDistView = m_SplitView.splitFar[i];
	}	
}

void CSHFXCascadedDirLight::ComputeMinimalBoundingSphere(gmtl::VecA4f& dest, const gmtl::VecA4f* pPoints, uint numPoints)
{
	// Use simple Badouiu-Clarkson algo
	const static float ERROR_EPSILON = 0.001f;
	const static uint NUM_ITERS = (uint)(1.0f / ERROR_EPSILON) + 1;

	gmtl::VecA4f center = pPoints[0];
	float curDiv = 1.0f;

	_LOOPi(NUM_ITERS)
	{
		// Find Xt
		gmtl::VecA4f furthestDiff;
		float furthestLength = -1.0f;
		_LOOPj(numPoints)
		{
			gmtl::VecA4f diff;
			float length;

			VecVecSub(&diff, &pPoints[j], &center);
			VecLength(&length, (gmtl::VecA3f*) &diff);
			if(length > furthestLength)
			{
				furthestLength = length;
				furthestDiff = diff;
			}
		}

		float mult = 1.0f / (1.0f + curDiv);
		gmtl::VecA4f offset;
		VecScalarMult(&offset, &furthestDiff, mult);
		VecVecAdd(&center, &center, &offset);

		curDiv += 1.0f;
	}

	// Find the radius
	float radius = -1.0f;
	_LOOPj(numPoints)
	{
		gmtl::VecA4f diff;
		float length;

		VecVecSub(&diff, &pPoints[j], &center);
		VecLength(&length, (gmtl::VecA3f*) &diff);
		if(length > radius)
		{
			radius = length;
		}
	}

	dest = center;
	dest[3] = radius;
}

void CSHFXCascadedDirLight::UpdateCascadedFrustumBounds(SHFXCascadedFrustumBounds& dest, float nearBias, const gmtl::Matrix44f& proj, float expSteepNess)
{
	// Find view frustum points
	gmtl::MatrixA44f invProj;
	gmtl::invert(invProj, proj);

	gmtl::VecA4f screenSpaceBounds[8];
	screenSpaceBounds[0][0] = -1.0f; screenSpaceBounds[0][1] = -1.0f; screenSpaceBounds[0][2] = 0.0f; screenSpaceBounds[0][3] = 1.0f; // near bottom left
	screenSpaceBounds[1][0] = -1.0f; screenSpaceBounds[1][1] = 1.0f; screenSpaceBounds[1][2] = 0.0f; screenSpaceBounds[1][3] = 1.0f; // near top left
	screenSpaceBounds[2][0] = 1.0f; screenSpaceBounds[2][1] = -1.0f; screenSpaceBounds[2][2] = 0.0f; screenSpaceBounds[2][3] = 1.0f; // near bottom right
	screenSpaceBounds[3][0] = 1.0f; screenSpaceBounds[3][1] = 1.0f; screenSpaceBounds[3][2] = 0.0f; screenSpaceBounds[3][3] = 1.0f; // near top right

	screenSpaceBounds[4][0] = -1.0f; screenSpaceBounds[4][1] = -1.0f; screenSpaceBounds[4][2] = 1.0f; screenSpaceBounds[4][3] = 1.0f; // far bottom left
	screenSpaceBounds[5][0] = -1.0f; screenSpaceBounds[5][1] = 1.0f; screenSpaceBounds[5][2] = 1.0f; screenSpaceBounds[5][3] = 1.0f; // far top left
	screenSpaceBounds[6][0] = 1.0f; screenSpaceBounds[6][1] = -1.0f; screenSpaceBounds[6][2] = 1.0f; screenSpaceBounds[6][3] = 1.0f; // far bottom right
	screenSpaceBounds[7][0] = 1.0f; screenSpaceBounds[7][1] = 1.0f; screenSpaceBounds[7][2] = 1.0f; screenSpaceBounds[7][3] = 1.0f; // far top right

	gmtl::VecA4f frustumPtsViewSpace[8];
	BatchTransformAndProjectVecW1(&invProj, screenSpaceBounds, frustumPtsViewSpace, 8);

	float farNearRatio = expSteepNess * frustumPtsViewSpace[4][2] / frustumPtsViewSpace[0][2];
	UpdateZSplits(nearBias, frustumPtsViewSpace[0][2], frustumPtsViewSpace[4][2], farNearRatio);

	// Alloc stack space for temp vectors (bypasses a compiler bug in VS 2010)
	gmtl::VecA4f vData[4];

	// Get the split frustum points in view space
	//gmtl::VecA4f nearToFarCorner[4];
	gmtl::VecA4f* nearToFarCorner = vData;
	VecVecSub(&nearToFarCorner[0], &frustumPtsViewSpace[4], &frustumPtsViewSpace[0]);
	VecVecSub(&nearToFarCorner[1], &frustumPtsViewSpace[5], &frustumPtsViewSpace[1]);
	VecVecSub(&nearToFarCorner[2], &frustumPtsViewSpace[6], &frustumPtsViewSpace[2]);
	VecVecSub(&nearToFarCorner[3], &frustumPtsViewSpace[7], &frustumPtsViewSpace[3]);

	//gmtl::VecA4f frustumPtsSplit[4*4];
	gmtl::VecA4f* frustumPtsSplit = &dest.frustumSplits[0]; //(vData + 4);
	frustumPtsSplit[0] = frustumPtsViewSpace[0];
	frustumPtsSplit[1] = frustumPtsViewSpace[1];
	frustumPtsSplit[2] = frustumPtsViewSpace[2];
	frustumPtsSplit[3] = frustumPtsViewSpace[3];
	frustumPtsSplit = frustumPtsSplit + 4;

	_LOOPi(4)
	{
		gmtl::VecA4f nearToFarSplit[4];
		VecScalarMult(&nearToFarSplit[0], &nearToFarCorner[0], m_Split.splitFar[i]);
		VecScalarMult(&nearToFarSplit[1], &nearToFarCorner[1], m_Split.splitFar[i]);
		VecScalarMult(&nearToFarSplit[2], &nearToFarCorner[2], m_Split.splitFar[i]);
		VecScalarMult(&nearToFarSplit[3], &nearToFarCorner[3], m_Split.splitFar[i]);

		VecVecAdd(&frustumPtsSplit[i*4], &frustumPtsViewSpace[0], &nearToFarSplit[0]);
		VecVecAdd(&frustumPtsSplit[i*4+1], &frustumPtsViewSpace[1], &nearToFarSplit[1]);
		VecVecAdd(&frustumPtsSplit[i*4+2], &frustumPtsViewSpace[2], &nearToFarSplit[2]);
		VecVecAdd(&frustumPtsSplit[i*4+3], &frustumPtsViewSpace[3], &nearToFarSplit[3]);
	}

	frustumPtsSplit =  &dest.frustumSplits[0];//(vData + 4);
	_LOOPi(4)
	{
		ComputeMinimalBoundingSphere(dest.bounds[i], &frustumPtsSplit[i*4], 8);
	}
}

void CSHFXCascadedDirLight::Update(const SHFXCascadedFrustumBounds& frustum, const gmtl::Vec3f& lightDir, const gmtl::Vec3f& lightPos, const gmtl::Matrix44f& view)
{
	gmtl::MatrixA44f lightView;
	gmtl::VecA3f lightPosViewSpace;
	GetLightViewParams(lightDir, lightPos, lightView, lightPosViewSpace);

	gmtl::MatrixA44f invCamView;
	_CAST_MAT44(invCamView) = view;
	gmtl::invert(_CAST_MAT44(invCamView));	

	gmtl::MatrixA44f lightViewInvCamView;
	MatMatMult(&lightViewInvCamView, &lightView, &invCamView); 

	gmtl::MatrixA44f texOffset;	
	texOffset[0][0] = 0.25f; texOffset[0][1] = 0.0f; texOffset[0][2] = 0.0f; texOffset[0][3] = 0.25f;// - 0.5f / width;
	texOffset[1][0] = 0.0f; texOffset[1][1] = -0.25f; texOffset[1][2] = 0.0f; texOffset[1][3] = 0.25f;// - 0.5f / height;
	texOffset[2][0] = 0.0f; texOffset[2][1] = 0.0f; texOffset[2][2] = 1.0f; texOffset[2][3] = 0.0f;
	texOffset[3][0] = 0.0f; texOffset[3][1] = 0.0f; texOffset[3][2] = 0.0f; texOffset[3][3] = 1.0f;

	gmtl::VecA3f lightViewSpaceBoundsPos[4];

	// The range is -1 to 1 and each cascade width and height is half of the resolution of the final shadow map
	float pixelXRange = GetShadowMap()->GetWidth(0) * 0.25f;
	float pixelYRange = GetShadowMap()->GetHeight(0) * 0.25f;

	// NOTE: Applicable to DX9/X360
	float pixelOffset = m_pShadowFX->GetRenderer().GetRendererInfo().pixelOffset;
	
	gmtl::VecA4f frustumSplitsLightView[4 + (4*4)];
	BatchTransformVecW1(&lightViewInvCamView, &frustum.frustumSplits[0], &frustumSplitsLightView[0], 4);	
	_LOOPi(4)
	{
		// Transfrom splits
		BatchTransformVecW1(&lightViewInvCamView, &frustum.frustumSplits[4 + i*4], &frustumSplitsLightView[4 + i*4], 4);	
		
		// Set the max z to be the z-pos of the light
		AABox lightSpaceBounds;
		PointsToAABox((gmtl::VecA3f*) &frustumSplitsLightView[i*4], 8, &lightSpaceBounds);
		if(lightSpaceBounds.max[2] < lightPosViewSpace[2])
			lightSpaceBounds.max[2] = lightPosViewSpace[2];

		gmtl::MatrixA44f proj;
		gmtl::setOrthoLH(proj, lightSpaceBounds.min[0], lightSpaceBounds.max[1],
			lightSpaceBounds.max[0], lightSpaceBounds.min[1], lightSpaceBounds.max[2], lightSpaceBounds.min[2]);
		MatMatMult(&m_ProjAABB.cascadedViewProj[i], &proj, &lightView);

		// Transform bounds center to view and light space
		TransformVecW1(&lightViewInvCamView, &_CAST_VECA3(frustum.bounds[i]), &lightViewSpaceBoundsPos[i]);

		// Truncate to remove sub-pixel aliasing causing flickering artifacts
		float xProj = (lightViewSpaceBoundsPos[i][0] / (frustum.bounds[i][3] * 2.0f)) * pixelXRange;
		float yProj = (lightViewSpaceBoundsPos[i][1] / (frustum.bounds[i][3] * 2.0f)) * pixelYRange;
		float xTrunc = (FastTruncate(xProj) / pixelXRange) * (frustum.bounds[i][3] * 2.0f);
		float yTrunc = (FastTruncate(yProj) / pixelYRange) * (frustum.bounds[i][3] * 2.0f);
		lightViewSpaceBoundsPos[i][0] = xTrunc;
		lightViewSpaceBoundsPos[i][1] = yTrunc;

		// Set the max z to be the z-pos of the light
		float nearZ = lightViewSpaceBoundsPos[i][2] + frustum.bounds[i][3];
		if(nearZ < lightPosViewSpace[2])
			nearZ = lightPosViewSpace[2];

		gmtl::setOrthoLH(proj, lightViewSpaceBoundsPos[i][0] - frustum.bounds[i][3], lightViewSpaceBoundsPos[i][1] + frustum.bounds[i][3],
			lightViewSpaceBoundsPos[i][0] + frustum.bounds[i][3], lightViewSpaceBoundsPos[i][1] - frustum.bounds[i][3], nearZ, lightViewSpaceBoundsPos[i][2] - frustum.bounds[i][3]);

		MatMatMult(&m_Proj.cascadedViewProj[i], &proj, &lightView);
		texOffset[0][3] = ((i & 1) * 0.5f) + 0.25f + (pixelOffset / GetShadowMap()->GetWidth(0)) ;// - (0.5f / width);
		texOffset[1][3] = ((i >> 1) * 0.5f) + 0.25f + (pixelOffset / GetShadowMap()->GetHeight(0));// - (0.5f / height);
		MatMatMult(&m_ProjWithTexOffset.cascadedViewProj[i], &texOffset, &m_Proj.cascadedViewProj[i]);
		
		//static char buf[1024];
		//sprintf(buf, "%f, %f, %f, %f,\n%f, %f, %f, %f, \n%f, %f, %f, %f, \n%f, %f, %f, %f\n\n\n", m_ProjWithTexOffset.cascadedViewProj[i][0][0], m_ProjWithTexOffset.cascadedViewProj[i][0][1], m_ProjWithTexOffset.cascadedViewProj[i][0][2], m_ProjWithTexOffset.cascadedViewProj[i][0][3],
		//	m_ProjWithTexOffset.cascadedViewProj[i][1][0], m_ProjWithTexOffset.cascadedViewProj[i][1][1], m_ProjWithTexOffset.cascadedViewProj[i][1][2], m_ProjWithTexOffset.cascadedViewProj[i][1][3],
		//	m_ProjWithTexOffset.cascadedViewProj[i][2][0], m_ProjWithTexOffset.cascadedViewProj[i][2][1], m_ProjWithTexOffset.cascadedViewProj[i][2][2], m_ProjWithTexOffset.cascadedViewProj[i][2][3],
		//	m_ProjWithTexOffset.cascadedViewProj[i][3][0], m_ProjWithTexOffset.cascadedViewProj[i][3][1], m_ProjWithTexOffset.cascadedViewProj[i][3][2], m_ProjWithTexOffset.cascadedViewProj[i][3][3]);

	}
}

//void 
//CSHFXCascadedDirLight::UpdateCascadedProj(const gmtl::Vec3f& lightDir, 
//											const gmtl::Vec3f& lightPos, 
//											const gmtl::Matrix44f& view, 
//											const gmtl::Matrix44f& proj)
//{
//	gmtl::MatrixA44f lightView;
//	gmtl::VecA3f lightPosViewSpace;
//	GetLightViewParams(lightDir, lightPos, lightView, lightPosViewSpace);
//	
//	// Find view frustum points
//	gmtl::MatrixA44f invViewProj;
//	gmtl::mult(_CAST_MAT44(invViewProj), proj, view);
//	gmtl::invert(_CAST_MAT44(invViewProj));	
//	
//	gmtl::VecA4f screenSpaceBounds[8];
//	screenSpaceBounds[0][0] = -1.0f; screenSpaceBounds[0][1] = -1.0f; screenSpaceBounds[0][2] = 0.0f; screenSpaceBounds[0][3] = 1.0f; // near bottom left
//	screenSpaceBounds[1][0] = -1.0f; screenSpaceBounds[1][1] = 1.0f; screenSpaceBounds[1][2] = 0.0f; screenSpaceBounds[1][3] = 1.0f; // near top left
//	screenSpaceBounds[2][0] = 1.0f; screenSpaceBounds[2][1] = -1.0f; screenSpaceBounds[2][2] = 0.0f; screenSpaceBounds[2][3] = 1.0f; // near bottom right
//	screenSpaceBounds[3][0] = 1.0f; screenSpaceBounds[3][1] = 1.0f; screenSpaceBounds[3][2] = 0.0f; screenSpaceBounds[3][3] = 1.0f; // near top right
//	
//	screenSpaceBounds[4][0] = -1.0f; screenSpaceBounds[4][1] = -1.0f; screenSpaceBounds[4][2] = 1.0f; screenSpaceBounds[4][3] = 1.0f; // far bottom left
//	screenSpaceBounds[5][0] = -1.0f; screenSpaceBounds[5][1] = 1.0f; screenSpaceBounds[5][2] = 1.0f; screenSpaceBounds[5][3] = 1.0f; // far top left
//	screenSpaceBounds[6][0] = 1.0f; screenSpaceBounds[6][1] = -1.0f; screenSpaceBounds[6][2] = 1.0f; screenSpaceBounds[6][3] = 1.0f; // far bottom right
//	screenSpaceBounds[7][0] = 1.0f; screenSpaceBounds[7][1] = 1.0f; screenSpaceBounds[7][2] = 1.0f; screenSpaceBounds[7][3] = 1.0f; // far top right
//	
//	// Transform view frustum to world space
//	gmtl::VecA4f frustumPtsWorldSpace[8];
//	BatchTransformAndProjectVecW1(&invViewProj, screenSpaceBounds, frustumPtsWorldSpace, 8);
//
//	SetCascadedProj(frustumPtsWorldSpace, lightView, lightPosViewSpace);
//}
	
//void
//CSHFXCascadedDirLight::SetCascadedProj(gmtl::VecA4f* frustumPtsWorldSpace, 
//									gmtl::MatrixA44f& lightView,
//									gmtl::VecA3f& lightPosViewSpace)
//{
//	// Alloc stack space for temp vectors (bypasses a compiler bug in VS 2010)
//	gmtl::VecA4f vData[4 + (4*4) + (8+4*3)];
//
//	// Get the split frustum points in world space
//	//gmtl::VecA4f nearToFarCorner[4];
//	gmtl::VecA4f* nearToFarCorner = vData;
//	VecVecSub(&nearToFarCorner[0], &frustumPtsWorldSpace[4], &frustumPtsWorldSpace[0]);
//	VecVecSub(&nearToFarCorner[1], &frustumPtsWorldSpace[5], &frustumPtsWorldSpace[1]);
//	VecVecSub(&nearToFarCorner[2], &frustumPtsWorldSpace[6], &frustumPtsWorldSpace[2]);
//	VecVecSub(&nearToFarCorner[3], &frustumPtsWorldSpace[7], &frustumPtsWorldSpace[3]);
//
//	//gmtl::VecA4f frustumPtsSplit[4*4];
//	gmtl::VecA4f* frustumPtsSplit = (vData + 4);
//	_LOOPi(4)
//	{
//		gmtl::VecA4f nearToFarSplit[4];
//		VecScalarMult(&nearToFarSplit[0], &nearToFarCorner[0], m_Split.splitFar[i]);
//		VecScalarMult(&nearToFarSplit[1], &nearToFarCorner[1], m_Split.splitFar[i]);
//		VecScalarMult(&nearToFarSplit[2], &nearToFarCorner[2], m_Split.splitFar[i]);
//		VecScalarMult(&nearToFarSplit[3], &nearToFarCorner[3], m_Split.splitFar[i]);
//
//		VecVecAdd(&frustumPtsSplit[i*4], &frustumPtsWorldSpace[0], &nearToFarSplit[0]);
//		VecVecAdd(&frustumPtsSplit[i*4+1], &frustumPtsWorldSpace[1], &nearToFarSplit[1]);
//		VecVecAdd(&frustumPtsSplit[i*4+2], &frustumPtsWorldSpace[2], &nearToFarSplit[2]);
//		VecVecAdd(&frustumPtsSplit[i*4+3], &frustumPtsWorldSpace[3], &nearToFarSplit[3]);
//	}
//
//	uint width = m_pCasterGroups[0]->GetRenderTarget(0)->GetWidth();
//	uint height = m_pCasterGroups[0]->GetRenderTarget(0)->GetHeight();
//	
//	gmtl::MatrixA44f texOffset;	
//	texOffset[0][0] = 0.25f; texOffset[0][1] = 0.0f; texOffset[0][2] = 0.0f; texOffset[0][3] = 0.25f;// - 0.5f / width;
//	texOffset[1][0] = 0.0f; texOffset[1][1] = -0.25f; texOffset[1][2] = 0.0f; texOffset[1][3] = 0.25f;// - 0.5f / height;
//	texOffset[2][0] = 0.0f; texOffset[2][1] = 0.0f; texOffset[2][2] = 1.0f; texOffset[2][3] = 0.0f;
//	texOffset[3][0] = 0.0f; texOffset[3][1] = 0.0f; texOffset[3][2] = 0.0f; texOffset[3][3] = 1.0f;
//
//	// Compute each light space projection for each split frustum
//	//gmtl::VecA4f frustumPtsLightSpace[8+4*3];
//	gmtl::VecA4f* frustumPtsLightSpace = (vData + 4 + (4*4));
//	BatchTransformVecW1(&lightView, &frustumPtsWorldSpace[0], &frustumPtsLightSpace[0], 4);	
//	_LOOPi(4)
//	{
//		// Transform split far plane
//		BatchTransformVecW1(&lightView, &frustumPtsSplit[i*4], &frustumPtsLightSpace[i*4+4], 4);
//		
//		// Set the max z to be the z-pos of the light
//		AABox lightSpaceBounds;
//		PointsToAABox((gmtl::VecA3f*) &frustumPtsLightSpace[i*4], 8, &lightSpaceBounds);
//		if(lightSpaceBounds.max[2] < lightPosViewSpace[2])
//			lightSpaceBounds.max[2] = lightPosViewSpace[2];
//		
//		gmtl::MatrixA44f proj;
//		gmtl::setOrthoLH(proj, lightSpaceBounds.min[0], lightSpaceBounds.max[1],
//			lightSpaceBounds.max[0], lightSpaceBounds.min[1], lightSpaceBounds.max[2], lightSpaceBounds.min[2]);
//			
//		MatMatMult(&m_Proj.cascadedViewProj[i], &proj, &lightView);
//		texOffset[0][3] = ((i & 1) * 0.5f) + 0.25f;// - (0.5f / width);
//		texOffset[1][3] = ((i >> 1) * 0.5f) + 0.25f;// - (0.5f / height);
//		MatMatMult(&m_ProjWithTexOffset.cascadedViewProj[i], &texOffset, &m_Proj.cascadedViewProj[i]);
//		
//		//memcpy(frustumPtsLightSpace, &frustumPtsLightSpace[4], sizeof(gmtl::VecA4f) * 4);
//	}
//
//}

void
CSHFXCascadedDirLight::GetLightViewParams(const gmtl::Vec3f& lightDir, 
										const gmtl::Vec3f& lightPos, 
										gmtl::MatrixA44f& outLightView,
										gmtl::VecA3f& outLightViewPos)
{
	//gmtl::Vec3f lightDirInv = -lightDir;
	//gmtl::Vec3f upDir(-view[2][0], -view[2][1], -view[2][2]);
	//float viewZDot = 1.0f - gmtl::Math::abs(gmtl::dot(upDir, lightDir));
	//if(viewZDot < gmtl::GMTL_VEC_EQUAL_EPSILON)
	//	upDir = gmtl::Vec3f(view[1][0], view[1][1], view[1][2]);
	
	gmtl::setView(outLightView, gmtl::Vec3f(0, 0, 0), lightDir , gmtl::Vec3f(0.0f, 1.0f, 0.0f));	

	// Rotate position to light view space
	gmtl::VecA3f pos(lightPos[0], lightPos[1], lightPos[2]);
	TransformVec(&outLightView, &pos, &outLightViewPos);
}	

									
//void 
//CSHFXCascadedDirLight::Update(float nearBiasExponent,
//							const gmtl::Vec3f& lightDir, 
//							const gmtl::Vec3f& lightPos, 
//							const gmtl::Matrix44f& view, 
//							const gmtl::Matrix44f& proj)
//{
//	gmtl::MatrixA44f lightView;
//	gmtl::VecA3f lightPosViewSpace;
//	GetLightViewParams(lightDir, lightPos, view, lightView, lightPosViewSpace);
//	
//	// Find view frustum points
//	gmtl::MatrixA44f invProj;
//	gmtl::invert(invProj, proj);
//	gmtl::MatrixA44f invView;
//	gmtl::invert(invView, view);
//
//	gmtl::VecA4f screenSpaceBounds[8];
//	screenSpaceBounds[0][0] = -1.0f; screenSpaceBounds[0][1] = -1.0f; screenSpaceBounds[0][2] = 0.0f; screenSpaceBounds[0][3] = 1.0f; // near bottom left
//	screenSpaceBounds[1][0] = -1.0f; screenSpaceBounds[1][1] = 1.0f; screenSpaceBounds[1][2] = 0.0f; screenSpaceBounds[1][3] = 1.0f; // near top left
//	screenSpaceBounds[2][0] = 1.0f; screenSpaceBounds[2][1] = -1.0f; screenSpaceBounds[2][2] = 0.0f; screenSpaceBounds[2][3] = 1.0f; // near bottom right
//	screenSpaceBounds[3][0] = 1.0f; screenSpaceBounds[3][1] = 1.0f; screenSpaceBounds[3][2] = 0.0f; screenSpaceBounds[3][3] = 1.0f; // near top right
//	
//	screenSpaceBounds[4][0] = -1.0f; screenSpaceBounds[4][1] = -1.0f; screenSpaceBounds[4][2] = 1.0f; screenSpaceBounds[4][3] = 1.0f; // far bottom left
//	screenSpaceBounds[5][0] = -1.0f; screenSpaceBounds[5][1] = 1.0f; screenSpaceBounds[5][2] = 1.0f; screenSpaceBounds[5][3] = 1.0f; // far top left
//	screenSpaceBounds[6][0] = 1.0f; screenSpaceBounds[6][1] = -1.0f; screenSpaceBounds[6][2] = 1.0f; screenSpaceBounds[6][3] = 1.0f; // far bottom right
//	screenSpaceBounds[7][0] = 1.0f; screenSpaceBounds[7][1] = 1.0f; screenSpaceBounds[7][2] = 1.0f; screenSpaceBounds[7][3] = 1.0f; // far top right
//
//	gmtl::VecA4f frustumPtsViewSpace[8];
//	BatchTransformAndProjectVecW1(&invProj, screenSpaceBounds, frustumPtsViewSpace, 8);	
//	UpdateZSplits(nearBiasExponent, frustumPtsViewSpace[0][2], frustumPtsViewSpace[4][2]);
//	//float splitRatio[4];
//	//_LOOPi(4)
//	//{
//	//	splitRatio[i] = (light.split.splitFar[i] - light.split.splitNear[0]) / 
//	//		(light.split.splitFar[3] - light.split.splitNear[0]);
//	//}
//		
//	// Transform view frustum to world space
//	gmtl::VecA4f frustumPtsWorldSpace[8];
//	BatchTransformAndProjectVecW1(&invView, frustumPtsViewSpace, frustumPtsWorldSpace, 8);
//	
//	SetCascadedProj(frustumPtsWorldSpace, lightView, lightPosViewSpace);
//}
//	
//void 
//CSHFXCascadedDirLight::SetSMSize(uint smSize)
//{
//	if(m_pCascadedSM && 
//		m_pCascadedSM->GetWidth(0) == smSize && 
//		m_pCascadedSM->GetHeight(0) == smSize)
//	{
//		return;
//	}
//	
//	m_pCascadedSM		= m_pShadowFX->GetRenderer().GetRResourceMgr().CreateTexture2DRT(smSize, smSize, TEXF_A32B32G32R32F);
//	m_pCascadedSMBackup = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateTexture2DRT(smSize, smSize, TEXF_A32B32G32R32F);
//	m_pSMDepthBuffer	= m_pShadowFX->GetRenderer().GetRResourceMgr().CreateDepthBuffer(smSize, smSize, DF_D24S8, 0);
//	
//	m_CascadedScreenMesh.Initialize(smSize, smSize, &m_pShadowFX->GetRenderer());
//}

void CSHFXCascadedDirLight::SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTexture2D* pFinal, IRDepthBuffer* pDepth, float* pStdDevs, uint* pSpreadKernelSizes)
{
	// Initialize all render groups
	_DEBUG_ASSERT(pSM->GetWidth(0) == pSM->GetHeight(0));
	_DEBUG_ASSERT(pBackup->GetWidth(0) == pBackup->GetHeight(0));
	_DEBUG_ASSERT(pDepth->GetWidth() >= pSM->GetHeight(0));

	uint shadowMapSize = pSM->GetWidth(0);

	if(m_GaussianParamsH)
		_DELETE_ARRAY(m_GaussianParamsH);

	if(m_GaussianParamsV)
		_DELETE_ARRAY(m_GaussianParamsV);

	m_GaussianParamsH = _NEW BFXUGaussianBlurParams[4];
	m_GaussianParamsV = _NEW BFXUGaussianBlurParams[4];

	_LOOPi(4)
	{
		IRTextureRT* pRT = pSM->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == shadowMapSize);

		m_pCasterGroups[i]->ClearRenderTargets();
		m_pCasterGroups[i]->AddRenderTarget(pRT);
		m_pCasterGroups[i]->SetDepthBuffer(pDepth);

		m_pCasterGroups[i]->SetClearDepth(1.0f);
		m_pCasterGroups[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);
		m_pCasterGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		//m_pCasterGroups[i]->SetViewport((pFinal->GetWidth() >> 1) * (i & 1), (pFinal->GetHeight() >> 1) * (i >> 1), pFinal->GetWidth() >> 1, pFinal->GetHeight() >> 1);

		pRT = pBackup->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == shadowMapSize);

		m_pBlur1Group[i]->ClearRenderTargets();
		m_pBlur1Group[i]->AddRenderTarget(pRT);
		m_pBlur1Group[i]->SetViewport(0, 0, pRT->GetWidth(), pRT->GetHeight());

		pRT = pFinal->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT((pRT->GetWidth() >> 1) == shadowMapSize);

		m_pBlur2Group[i]->ClearRenderTargets();
		m_pBlur2Group[i]->AddRenderTarget(pRT);
		m_pBlur2Group[i]->SetViewport((pRT->GetWidth() >> 1) * (i & 1), (pRT->GetHeight() >> 1) * (i >> 1), pRT->GetWidth() >> 1, pRT->GetHeight() >> 1);
	}

	m_pClearFinal->ClearRenderTargets();
	m_pClearFinal->AddRenderTarget(pFinal->GetTextureRT(0));
	m_pClearFinal->SetViewport(0, 0, pFinal->GetWidth(0), pFinal->GetHeight(0));
	m_pClearFinal->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

	m_CascadedScreenMesh.Initialize(pSM->GetWidth(0), pSM->GetHeight(0), &m_pShadowFX->GetRenderer());

	wstring fileName = _W("[shd]\\basefxutils\\gaussianblur1d.fx");
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));

	_LOOPi(4)
	{
		// Set gaussian blur params	
		gmtl::Vec2f spreadH(1.0f, 0.0f);
		gmtl::Vec2f spreadV(0.0f, 1.0f);
		gmtl::Vec2f offsetH(0.0f, 0.0f);
		gmtl::Vec2f offsetV(0.0f, 0.0f);

		//float sigma = (filterSize) * pow(0.32f, i);
		//if(sigma < 0.6f)
		//	sigma = 0.6f;

		m_GaussianParamsH[i].InitializeGaussianFilter(
			pStdDevs[i], pSpreadKernelSizes[i]*2+1, &spreadH, &offsetH, 1.0f, (float)pSM->GetWidth(0), (float)pSM->GetHeight(0), TRUE);

		m_GaussianParamsV[i].InitializeGaussianFilter(
			pStdDevs[i], pSpreadKernelSizes[i]*2+1, &spreadV, &offsetV, 1.0f, (float)pBackup->GetWidth(0), (float)pBackup->GetHeight(0), TRUE);

		m_pBlur1Effect[i] = LoadGaussianEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), &m_GaussianParamsH[i], pSM);
		m_pBlur2Effect[i] = LoadGaussianEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), &m_GaussianParamsV[i], pBackup);

		// Add gaussian blur draw ops
		m_pBlur1Group[i]->Reset();
		m_pBlur1Group[i]->AddDrawOp(m_CascadedScreenMesh.GetQuadVertices(), m_CascadedScreenMesh.GetQuadIndices(),
			m_pBlur1Effect[i], NULL, GetGaussianBlurEffectTech(m_pBlur1Effect[i], m_GaussianParamsH[i].GetGaussianSize()), RVarData());

		m_pBlur2Group[i]->Reset();
		m_pBlur2Group[i]->AddDrawOp(m_CascadedScreenMesh.GetQuadVertices(), m_CascadedScreenMesh.GetQuadIndices(),
			m_pBlur2Effect[i], NULL, GetGaussianBlurEffectTech(m_pBlur2Effect[i], m_GaussianParamsV[i].GetGaussianSize()), RVarData());
	}

	m_pShadowMap = pFinal;

	m_TexOffsets[0] = 0.5f / GetShadowMap()->GetWidth(0);
	m_TexOffsets[1] = 0.5f / GetShadowMap()->GetHeight(0);
}

void CSHFXCascadedDirLight::SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTexture2D* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples* pBlurSamples, float* pSpreadScales, boolean isUseNoiseBlur)
{
	// Initialize all render groups
	_DEBUG_ASSERT(pSM->GetWidth(0) == pSM->GetHeight(0));
	_DEBUG_ASSERT(pDepth->GetWidth() >= pSM->GetHeight(0));

	uint shadowMapSize = pSM->GetWidth(0);

	if(m_PoissonParams)
		_DELETE_ARRAY(m_PoissonParams);

	m_PoissonParams = _NEW BFXUPoissonBlurParams[4];

	_LOOPi(4)
	{
		IRTextureRT* pRT = pSM->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == shadowMapSize);

		m_pCasterGroups[i]->ClearRenderTargets();
		m_pCasterGroups[i]->AddRenderTarget(pRT);
		m_pCasterGroups[i]->SetDepthBuffer(pDepth);

		m_pCasterGroups[i]->SetClearDepth(1.0f);
		m_pCasterGroups[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);
		m_pCasterGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

		pRT = pFinal->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT((pRT->GetWidth() >> 1) == shadowMapSize);
		
		m_pBlur1Group[i]->ClearRenderTargets();
		m_pBlur1Group[i]->AddRenderTarget(pRT);
		m_pBlur1Group[i]->SetViewport((pRT->GetWidth() >> 1) * (i & 1), (pRT->GetHeight() >> 1) * (i >> 1), pRT->GetWidth() >> 1, pRT->GetHeight() >> 1);	
	}

	m_pClearFinal->ClearRenderTargets();
	m_pClearFinal->AddRenderTarget(pFinal->GetTextureRT(0));
	m_pClearFinal->SetViewport(0, 0, pFinal->GetWidth(0), pFinal->GetHeight(0));
	m_pClearFinal->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

	m_CascadedScreenMesh.Initialize(shadowMapSize, shadowMapSize, &m_pShadowFX->GetRenderer());

	wstring fileName = _W("[shd]\\basefxutils\\poissondiskblur.fx");
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));

	_LOOPi(4)
	{
		m_PoissonParams[i].InitializePoissonFilter(pBlurSamples[i], &m_pShadowFX->GetRenderer(), pSpreadScales[i], (float)shadowMapSize, (float)shadowMapSize, isUseNoiseBlur);

		m_pBlur1Effect[i] = LoadPoissonEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), &m_PoissonParams[i], pSM); //LoadGaussianEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), &m_GaussianParamsH[i], pSM);
		m_pBlur2Effect[i] = NULL;

		// Add poisson blur draw ops
		m_pBlur1Group[i]->Reset();
		m_pBlur1Group[i]->AddDrawOp(m_CascadedScreenMesh.GetQuadVertices(), m_CascadedScreenMesh.GetQuadIndices(),
			m_pBlur1Effect[i], NULL, GetPoissonBlurEffectTech(m_pBlur1Effect[i], &m_PoissonParams[i]), RVarData());
	}

	m_pShadowMap = pFinal;

	m_TexOffsets[0] = 0.5f / GetShadowMap()->GetWidth(0);
	m_TexOffsets[1] = 0.5f / GetShadowMap()->GetHeight(0);
}


void CSHFXCascadedDirLight::SetShadowMapParams(IRTexture2D* pFinal, IRDepthBuffer* pDepth)
{
	_LOOPi(4)
	{
		IRTextureRT* pRT = pFinal->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);

		m_pCasterGroups[i]->ClearRenderTargets();
		m_pCasterGroups[i]->AddRenderTarget(pRT);
		m_pCasterGroups[i]->SetDepthBuffer(pDepth);
		//m_pCasterGroups[i]->SetClearDepth(1.0f);
		//m_pCasterGroups[i]->SetClearColor(gmtl::Vec4f(0.5f, 0.5f, 1.0f, 1.0f));

		m_pCasterGroups[i]->SetViewport((pRT->GetWidth() >> 1) * (i & 1), (pRT->GetHeight() >> 1) * (i >> 1), pRT->GetWidth() >> 1, pRT->GetHeight() >> 1);

		m_pBlur1Effect[i] = NULL;
		m_pBlur2Effect[i] = NULL;
	}

	m_pClearFinal->ClearRenderTargets();
	m_pClearFinal->AddRenderTarget(pFinal->GetTextureRT(0));
	m_pClearFinal->SetDepthBuffer(pDepth);
	m_pClearFinal->SetViewport(0, 0, pFinal->GetWidth(0), pFinal->GetHeight(0));
	m_pClearFinal->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_pClearFinal->SetClearDepth(1.0f);

	m_pShadowMap = pFinal;

	m_TexOffsets[0] = 0.5f / GetShadowMap()->GetWidth(0);
	m_TexOffsets[1] = 0.5f / GetShadowMap()->GetHeight(0);
}

void CSHFXCascadedDirLight::Initialize()
{
	wstring fileName = _W("[shd]\\BaseFXUtils\\2DPipeSetup.fx");
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	m_p2DPipeSetupEffect = m_pShadowFX->GetRenderer().GetRResourceMgr().LoadEffect(
		&id, 
		pBuffer,
		NULL,
		NULL,
		0);

	//fileName = _W("[shd]\\BaseFXUtils\\ColorFill.fx");
	//id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	//pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());

	//float expClearColor = gmtl::Math::exp(EXP_SM_K * 1.0f);
	//REffectParam params[1];
	//uint semantics[1];
	//gmtl::Vec4f colorFill;
	//colorFill[0] = colorFill[1] = colorFill[2] = colorFill[3] = expClearColor;
	//params[0].SetVector4(&colorFill);	
	//semantics[0] = BFXU_COLOR_FILL;

	//m_pColorFillEffect = m_pShadowFX->GetRenderer().GetRResourceMgr().LoadEffect(
	//	&id, 
	//	pBuffer,
	//	params,
	//	semantics,
	//	1);

	//fileName = _W("[shd]\\BaseFXUtils\\ColorChannelEnable.fx");
	//id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	//pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	//m_pColorChannelEffect = m_pShadowFX->GetRenderer().GetRResourceMgr().LoadEffect(
	//	&id, 
	//	pBuffer,
	//	NULL,
	//	NULL,
	//	0);

	//m_pClearGroup = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL);

	_LOOPi(4)
	{
		m_pCasterGroups[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(DirLightCasterSortLess);
		m_pBlur1Group[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL, 1);
		m_pBlur2Group[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL, 1);
	
		m_pBlur1Effect[i] = NULL;
		m_pBlur2Effect[i] = NULL;
	}

	m_pClearFinal = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL);

	AddContext();
}	

void CSHFXCascadedDirLight::RenderCascadeDirLightCaster(BFXRenderContext& context)
{	
	CSHFXParamPool* pPool = (CSHFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET) );
	IBFXParamPool* pBFXPool = (IBFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET) );
	
	uint cascadeIndex = pPool->GetCurrentCascadedCasterIndex() - 1;
	_DEBUG_ASSERT(cascadeIndex < 4);

	uint contextID = pBFXPool->GetCurrentContext();

	IRRenderGroup* pCasterGroup = m_pCasterGroups[cascadeIndex];
	RRenderOp* pOp = m_Contexts[contextID].pCasterContext[cascadeIndex]->Alloc();

	RVarData data;
	data.fVal = pPool->GetZDistanceFromCamera();
	
	pCasterGroup->FillDrawOp(pOp, context.pMesh->pVBGroup, context.pMesh->pIB, context.pEffect, 
		context.pDynamicParams, context.tech, data);

	//pCasterGroup->AddDrawOp(context.pMesh->pVBGroup, context.pMesh->pIB, context.pEffect, 
	//	context.pDynamicParams, context.tech, data);
}

uint CSHFXCascadedDirLight::AddContext()
{
	PipelineContext contextToAdd;
	_LOOPi(4)
	{
		contextToAdd.pCasterContext[i]	= _NEW BFXDataContext<RRenderOp>(&m_CasterPool[i]);
	}

	m_Contexts.push_back(contextToAdd);
	return m_Contexts.size() - 1;
}

uint CSHFXCascadedDirLight::GetNumOfContexts()
{
	return m_Contexts.size();
}

void CSHFXCascadedDirLight::ResetContext(uint contextID)
{
	_LOOPi(4)
	{
		m_Contexts[contextID].pCasterContext[i]->Dispose();
	}
}

void CSHFXCascadedDirLight::Flush()
{
	m_pClearFinal->Flush();

	_LOOPi(4)
	{
		_LOOPj(m_Contexts.size())
		{
			m_pCasterGroups[i]->AddToList(m_Contexts[j].pCasterContext[i]->GetDataBuffer(), m_Contexts[j].pCasterContext[i]->GetNumUsedData());
		}
	
		if(m_pCasterGroups[i]->GetNumOfDrawOps() > 0)
		{
			m_pCasterGroups[i]->Flush();
			m_pCasterGroups[i]->Reset();	

			m_CasterPool[i].UpdateSize();
			_LOOPj(m_Contexts.size())
			{
				m_Contexts[j].pCasterContext[i]->UpdateAndReset();
			}

			// Perform blur if needed
			if(m_pBlur1Effect[i] || m_pBlur2Effect[i])
			{
				IREffectTemplate* pTemplate = m_p2DPipeSetupEffect->GetTemplate();
				pTemplate->BeginTechnique(0);
				pTemplate->BeginPass(0);

				if(m_pBlur1Effect[i])
					m_pBlur1Group[i]->Flush();

				if(m_pBlur2Effect[i])
					m_pBlur2Group[i]->Flush();

				pTemplate->EndPass();
				pTemplate->EndTechnique();
			}
		}
	}
}


_NAMESPACE_END