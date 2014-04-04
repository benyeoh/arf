//================================================================================
//
//		CSHPRTCompute.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/9/2012
//
//================================================================================

#include "SHPRTComputeFramework.h"

_NAMESPACE_BEGIN

boolean CSHPRTCompute::Initialize(IRRenderer& renderer, IAppCallback& callback)
{
	m_pRenderer = &renderer;
	//m_pBaseFX = &baseFX;
	m_pAppCallback = &callback;

	m_Pipeline.Initialize(&renderer, &callback);

	//m_pBFXPipeline = m_pBaseFX->GetResourceMgr().CreatePipeline();
	//m_pBFXPipeline->SetDepthBuffer(m_pOcclusionDB);
	//m_pBFXPipeline->SetViewport(0, 0, SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE);

	//m_pBFXParamPool = m_pBaseFX->GetResourceMgr().CreateParamPool();

	//m_ParamContainer.SetParamCallback(m_pBFXParamPool, _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET));
	//m_RenderContainer.SetRenderCallback(m_pBFXPipeline, _GET_LIB_INDEX_FROM_MAT_TYPE(BFX_MATERIAL_TYPE_OFFSET));

	return TRUE;
}

//uint CSHPRTCompute::AddContext()
//{
//	return m_Pipeline.AddContext();
//}
//
//uint CSHPRTCompute::GetNumOfContexts()
//{
//	return m_Pipeline.GetNumOfContexts();
//}
//
//void CSHPRTCompute::ResetContext(uint contextID)
//{
//	m_Pipeline.ResetContext(contextID);
//}

void CSHPRTCompute::ComputeVertexLocEntries(SHPRTLocationEntry* pLocEntries, const gmtl::MatrixA44f& worldTrans, 
										void* pPos, uint posStride, void* pNormal, uint normalStride,
										uint numVertices, float posBias)
{
	_LOOPi(numVertices)
	{
		gmtl::VecA3f pos;
		pos[0] = ((float*)pPos)[0];
		pos[1] = ((float*)pPos)[1];
		pos[2] = ((float*)pPos)[2];

		TransformVecW1(&worldTrans, &pos, &pos);

		gmtl::VecA3f normal;
		normal[0] = ((float*)pNormal)[0];
		normal[1] = ((float*)pNormal)[1];
		normal[2] = ((float*)pNormal)[2];

		gmtl::MatrixA44f worldTrans3x3;
		worldTrans3x3.set(worldTrans[0][0], worldTrans[0][1], worldTrans[0][2],
						  worldTrans[1][0], worldTrans[1][1], worldTrans[1][2],
						  worldTrans[2][0], worldTrans[2][1], worldTrans[2][2]);
		TransformVec(&worldTrans3x3, &normal, &normal);
		NormalizeVec(normal);

		pLocEntries[i].pos = pos;
		pLocEntries[i].normal = normal;
		pLocEntries[i].texX = -1;
		pLocEntries[i].texY = -1;

		// Add bias to make sure rays get occluded by intersecting geometry
		gmtl::VecA3f posBiasVec = normal;
		posBiasVec[0] *= posBias;
		posBiasVec[1] *= posBias;
		posBiasVec[2] *= posBias;

		pLocEntries[i].pos = pLocEntries[i].pos + posBiasVec;

		pPos = ((byte*) pPos) + posStride;
		pNormal = ((byte*) pNormal) + normalStride;
	}
}

//uint CSHPRTCompute::ComputeUVLocEntries(SHPRTLocationEntry* pLocEntries, const gmtl::MatrixA44f& worldTrans, 
//										void* pPos, uint posStride, void* pNormal, uint normalStride,
//										void* pUV, uint uvStride, ushort* pIndices, uint numTri, uint texWidth, uint texHeight)
//{
//	uint numEntries = 0;
//
//	_LOOPi(numTri)
//	{
//		uint i1 = (uint) pIndices[i*3];
//		uint i2 = (uint) pIndices[i*3+1];
//		uint i3 = (uint) pIndices[i*3+2];
//
//		gmtl::VecA4f pos1;
//		gmtl::VecA4f pos2;
//		gmtl::VecA4f pos3;
//
//		// Get the triangle pos to rasterize (which is the uv)
//		// TODO: Remove determinant checks for back faces
//		gmtl::Vec2f& uv1 = *( (gmtl::Vec2f*) ( ((byte*) pUV) + uvStride * i1 ) );
//		gmtl::Vec2f& uv2 = *( (gmtl::Vec2f*) ( ((byte*) pUV) + uvStride * i2 ) );
//		gmtl::Vec2f& uv3 = *( (gmtl::Vec2f*) ( ((byte*) pUV) + uvStride * i3 ) );
//
//		pos1[0] = uv1[0] * 2.0f - 1.0f;
//		pos1[1] = 1.0f - uv1[1] * 2.0f;
//		pos1[2] = 0.0f;
//		pos1[3] = 1.0f;
//
//		pos2[0] = uv2[0] * 2.0f - 1.0f;
//		pos2[1] = 1.0f - uv2[1] * 2.0f;
//		pos2[2] = 0.0f;
//		pos2[3] = 1.0f;
//
//		pos3[0] = uv3[0] * 2.0f - 1.0f;
//		pos3[1] = 1.0f - uv3[1] * 2.0f;
//		pos3[2] = 0.0f;
//		pos3[3] = 1.0f;
//
//		// Get the interpolants (which is the world space position/normals)
//		gmtl::Vec3f* pPos1 = (gmtl::Vec3f*) ( ((byte*) pPos) + posStride * i1 );
//		gmtl::Vec3f* pPos2 = (gmtl::Vec3f*) ( ((byte*) pPos) + posStride * i2 );
//		gmtl::Vec3f* pPos3 = (gmtl::Vec3f*) ( ((byte*) pPos) + posStride * i3 );
//
//		gmtl::VecA3f worldPosInter[3];
//		worldPosInter[0][0] = (*pPos1)[0];
//		worldPosInter[0][1] = (*pPos1)[1];
//		worldPosInter[0][2] = (*pPos1)[2];
//		worldPosInter[1][0] = (*pPos2)[0];
//		worldPosInter[1][1] = (*pPos2)[1];
//		worldPosInter[1][2] = (*pPos2)[2];
//		worldPosInter[2][0] = (*pPos3)[0];
//		worldPosInter[2][1] = (*pPos3)[1];
//		worldPosInter[2][2] = (*pPos3)[2];
//
//		gmtl::Vec3f* pNormal1 = (gmtl::Vec3f*) ( ((byte*) pNormal) + normalStride * i1 );
//		gmtl::Vec3f* pNormal2 = (gmtl::Vec3f*) ( ((byte*) pNormal) + normalStride * i2 );
//		gmtl::Vec3f* pNormal3 = (gmtl::Vec3f*) ( ((byte*) pNormal) + normalStride * i3 );
//
//		gmtl::VecA3f worldNormalInter[3];
//		worldNormalInter[0][0] = (*pNormal1)[0];
//		worldNormalInter[0][1] = (*pNormal1)[1];
//		worldNormalInter[0][2] = (*pNormal1)[2];
//		worldNormalInter[1][0] = (*pNormal2)[0];
//		worldNormalInter[1][1] = (*pNormal2)[1];
//		worldNormalInter[1][2] = (*pNormal2)[2];
//		worldNormalInter[2][0] = (*pNormal3)[0];
//		worldNormalInter[2][1] = (*pNormal3)[1];
//		worldNormalInter[2][2] = (*pNormal3)[2];
//
//		BatchTransformVecW1(&worldTrans, worldPosInter, worldPosInter, 3);
//		BatchTransformVecA(&(_CAST_MATA33(worldTrans)), worldNormalInter, worldNormalInter, 3);
//
//		float interData1[6];
//		interData1[0] = worldPosInter[0][0];
//		interData1[1] = worldPosInter[0][1];
//		interData1[2] = worldPosInter[0][2];
//		interData1[3] = worldNormalInter[0][0];
//		interData1[4] = worldNormalInter[0][1];
//		interData1[5] = worldNormalInter[0][2];
//
//		float interData2[6];
//		interData2[0] = worldPosInter[1][0];
//		interData2[1] = worldPosInter[1][1];
//		interData2[2] = worldPosInter[1][2];
//		interData2[3] = worldNormalInter[1][0];
//		interData2[4] = worldNormalInter[1][1];
//		interData2[5] = worldNormalInter[1][2];
//
//		float interData3[6];
//		interData3[0] = worldPosInter[2][0];
//		interData3[1] = worldPosInter[2][1];
//		interData3[2] = worldPosInter[2][2];
//		interData3[3] = worldNormalInter[2][0];
//		interData3[4] = worldNormalInter[2][1];
//		interData3[5] = worldNormalInter[2][2];
//
//		numEntries += DoComputeUVLocEntries(&(pLocEntries[numEntries]), pos1, pos2, pos3, interData1, interData2, interData3, texWidth, texHeight);
//	}
//
//	return numEntries;
//} 

void CSHPRTCompute::ComputeBRDFLocationEntries(SHPRTLocationEntry* pLocEntries, uint texWidth, uint texHeight)
{
	float rcpHeight = 1.0f / texHeight;
	float rcpWidth = 1.0f / texWidth;

	_LOOPi(texHeight)
	{
		_LOOPj(texWidth)
		{
			uint texIndex = i*texWidth+j;

			pLocEntries[texIndex].texX = j;
			pLocEntries[texIndex].texY = i;

			// Derive local hemisphere direction from tex coord of the paraboloid projection
			float u = ((j + 0.5f) * rcpWidth * 2.0f) - 1.0f;
			float v = ((1.0f - (i + 0.5f) * rcpHeight) * 2.0f) - 1.0f;

			gmtl::VecA3f dir;
			dir[0] = (2.0f * u) / (v * v + u * u + 1.0f);
			dir[1] = -(v * v + u * u - 1.0f) / (v * v + u * u + 1.0f);
			dir[2] = (2.0f * v) / (v * v + u * u + 1.0f);

			// Clamp y to the hemisphere
			if(dir[1] < 0.0f)
				dir[1] = 0.0f;

			// Then transform the dir with respect to the world space			
			NormalizeVec(dir);
			
			//// Compute from ((-1 to 1) * 0.25 + 0.25) and ((-1 to 1) * 0.25 + 0.75)
			//// If z is < 0, then it's 0.5 to 1 range
			//float y = (i + 0.5f) * rcpHeight;
			//float x = (j + 0.5f) * rcpWidth;
			//float zMult = 1.0f;
			//if(x <= 0.5f)
			//{
			//	x = x * 4.0f - 1.0f;
			//}
			//else
			//{
			//	x = (1.0f - x) * 4.0f - 1.0f;
			//	zMult = -1.0f;
			//}

			//float z = sqrt(1.0f - y * y - x * x) * zMult;

			pLocEntries[texIndex].normal = dir;

			pLocEntries[texIndex].pos[0] = 0.0f;
			pLocEntries[texIndex].pos[1] = 0.0f;
			pLocEntries[texIndex].pos[2] = 0.0f;
		}
	}
}

void CSHPRTCompute::CompressOcclTexDataToSH(uint* pParaTexData, SHPRTDataEntry* pDataEntryRDest, SHPRTDataEntry* pDataEntryGDest, SHPRTDataEntry* pDataEntryBDest, const gmtl::Matrix44f& paraView)
{
	const float BYTE_RCP = 1.0f / 255.0f;
	const float OCCL_RT_SIZE_RCP =  1.0f / SHPRT_COMPUTE_OCCL_RT_SIZE;

	gmtl::MatrixA44f invParaViewRot;
	invParaViewRot.set( paraView[0][0], paraView[1][0], paraView[2][0],
						paraView[0][1], paraView[1][1], paraView[2][1],
						paraView[0][2], paraView[1][2], paraView[2][2] );

	// Zero SH
	SHPRTDataEntry red;
	SHPRTDataEntry green;
	SHPRTDataEntry blue;

	_LOOPi(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
	{
		red.sh[i] = 0.0f;
		green.sh[i] = 0.0f;
		blue.sh[i] = 0.0f;
	}

	//_LOOPi(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
	//{
	//	pDataEntryRDest->sh[i] = 0.0f;
	//	pDataEntryGDest->sh[i] = 0.0f;
	//	pDataEntryBDest->sh[i] = 0.0f;
	//}

	_LOOPi(SHPRT_COMPUTE_OCCL_RT_SIZE)
	{
		_LOOPj(SHPRT_COMPUTE_OCCL_RT_SIZE)
		{
			uint index = i * SHPRT_COMPUTE_OCCL_RT_SIZE + j;			
			
			// Derive local hemisphere direction from tex coord of the paraboloid projection
			float u = ((j + 0.5f) * OCCL_RT_SIZE_RCP * 2.0f) - 1.0f;
			float v = ((1.0f - (i + 0.5f) * OCCL_RT_SIZE_RCP) * 2.0f) - 1.0f;
			
			gmtl::VecA3f dir;
			dir[0] = (2.0f * u) / (v * v + u * u + 1.0f);
			dir[1] = -(v * v + u * u - 1.0f) / (v * v + u * u + 1.0f);
			dir[2] = (2.0f * v) / (v * v + u * u + 1.0f);
			
			// Then transform the dir with respect to the world space
			NormalizeVec(dir);
			TransformVec(&invParaViewRot, &dir, &dir);

			// Weight the occlusion amount, taking into account the other side of the sphere
			// as well as the probability distribution of the texture sample
			// Then evaluate the SH for all bands in that hemisphere direction
			// Final SH coeff: 
			// 4PI/2N * SUM(2N, Sh(nInSphere) * Occl(nInSphere)) = 
			// 4PI/2N * SUM(N, Sh(nInHemi) * Occl(nInHemi)) =
			// SUM( N, Sh(nInHemi) * (4PI/2N * Occl(nInHemi)) )
			
			uint data = pParaTexData[index];
			float occlusionAmountB = (data & 0xFF) * BYTE_RCP;
			occlusionAmountB *= PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index];
			float occlusionAmountG = ((data >> 8) & 0xFF) * BYTE_RCP;
			occlusionAmountG *= PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index];
			float occlusionAmountR = ((data >> 16) & 0xFF) * BYTE_RCP;
			occlusionAmountR *= PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index];

			_LOOPk(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
			{
				float ylm = EvaluateSHFastCartesian(k, _CAST_VEC3(dir));

				red.sh[k] += (ylm * occlusionAmountR);
				green.sh[k] += (ylm * occlusionAmountG);
				blue.sh[k] += (ylm * occlusionAmountB);
			}

			//_LOOPk(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
			//{
			//	float ylm = EvaluateSHFastCartesian(k, _CAST_VEC3(dir));

			//	pDataEntryRDest->sh[k] += (ylm * occlusionAmountR);
			//	pDataEntryGDest->sh[k] += (ylm * occlusionAmountG);
			//	pDataEntryBDest->sh[k] += (ylm * occlusionAmountB);
			//}
		}
	}

	_LOOPi(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
	{
		pDataEntryRDest->sh[i] = red.sh[i];
		pDataEntryGDest->sh[i] = green.sh[i];
		pDataEntryBDest->sh[i] = blue.sh[i];
	}
}

void CSHPRTCompute::CompressOcclTexDataDualToSH(uint* pParaTexData, uint* pParaTexDataDual, SHPRTDataEntry* pDataEntryRDest, SHPRTDataEntry* pDataEntryGDest, SHPRTDataEntry* pDataEntryBDest, const gmtl::Matrix44f& paraView, const gmtl::Matrix44f& paraViewDual, float dualSelfOcclScale)
{
	const float BYTE_RCP = 1.0f / 255.0f;
	const float OCCL_RT_SIZE_RCP =  1.0f / SHPRT_COMPUTE_OCCL_RT_SIZE;

	gmtl::MatrixA44f invParaViewRot;
	invParaViewRot.set( paraView[0][0], paraView[1][0], paraView[2][0],
		paraView[0][1], paraView[1][1], paraView[2][1],
		paraView[0][2], paraView[1][2], paraView[2][2] );

	gmtl::MatrixA44f invParaViewRotDual;
	invParaViewRotDual.set( paraViewDual[0][0], paraViewDual[1][0], paraViewDual[2][0],
		paraViewDual[0][1], paraViewDual[1][1], paraViewDual[2][1],
		paraViewDual[0][2], paraViewDual[1][2], paraViewDual[2][2] );

	// Zero SH
	SHPRTDataEntry red;
	SHPRTDataEntry green;
	SHPRTDataEntry blue;

	_LOOPi(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
	{
		red.sh[i] = 0.0f;
		green.sh[i] = 0.0f;
		blue.sh[i] = 0.0f;
	}

	_LOOPi(SHPRT_COMPUTE_OCCL_RT_SIZE)
	{
		_LOOPj(SHPRT_COMPUTE_OCCL_RT_SIZE)
		{
			uint index = i * SHPRT_COMPUTE_OCCL_RT_SIZE + j;			

			// Derive local hemisphere direction from tex coord of the paraboloid projection
			float u = ((j + 0.5f) * OCCL_RT_SIZE_RCP * 2.0f) - 1.0f;
			float v = ((1.0f - (i + 0.5f) * OCCL_RT_SIZE_RCP) * 2.0f) - 1.0f;

			gmtl::VecA3f dir;
			dir[0] = (2.0f * u) / (v * v + u * u + 1.0f);
			dir[1] = -(v * v + u * u - 1.0f) / (v * v + u * u + 1.0f);
			dir[2] = (2.0f * v) / (v * v + u * u + 1.0f);

			// Then transform the dir with respect to the world space
			NormalizeVec(dir);
			TransformVec(&invParaViewRot, &dir, &dir);

			// Weight the occlusion amount, taking into account the other side of the sphere
			// as well as the probability distribution of the texture sample
			// Then evaluate the SH for all bands in that hemisphere direction
			// Final SH coeff: 
			// 4PI/2N * SUM(2N, Sh(nInSphere) * Occl(nInSphere)) = 
			// 4PI/2N * SUM(N, Sh(nInHemi) * Occl(nInHemi)) =
			// SUM( N, Sh(nInHemi) * (4PI/2N * Occl(nInHemi)) )

			uint data = pParaTexData[index];
			float occlusionAmountB = (data & 0xFF) * BYTE_RCP;
			occlusionAmountB *= PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index];
			float occlusionAmountG = ((data >> 8) & 0xFF) * BYTE_RCP;
			occlusionAmountG *= PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index];
			float occlusionAmountR = ((data >> 16) & 0xFF) * BYTE_RCP;
			occlusionAmountR *= PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index];

			_LOOPk(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
			{
				float ylm = EvaluateSHFastCartesian(k, _CAST_VEC3(dir));

				red.sh[k] += (ylm * occlusionAmountR);
				green.sh[k] += (ylm * occlusionAmountG);
				blue.sh[k] += (ylm * occlusionAmountB);
			}
		}
	}

	// Other hemisphere
	_LOOPi(SHPRT_COMPUTE_OCCL_RT_SIZE)
	{
		_LOOPj(SHPRT_COMPUTE_OCCL_RT_SIZE)
		{
			uint index = i * SHPRT_COMPUTE_OCCL_RT_SIZE + j;			

			// Derive local hemisphere direction from tex coord of the paraboloid projection
			float u = ((j + 0.5f) * OCCL_RT_SIZE_RCP * 2.0f) - 1.0f;
			float v = ((1.0f - (i + 0.5f) * OCCL_RT_SIZE_RCP) * 2.0f) - 1.0f;

			gmtl::VecA3f dir;
			dir[0] = (2.0f * u) / (v * v + u * u + 1.0f);
			dir[1] = -(v * v + u * u - 1.0f) / (v * v + u * u + 1.0f);
			dir[2] = (2.0f * v) / (v * v + u * u + 1.0f);

			// Then transform the dir with respect to the world space
			NormalizeVec(dir);
			TransformVec(&invParaViewRotDual, &dir, &dir);

			// Weight the occlusion amount, taking into account the other side of the sphere
			// as well as the probability distribution of the texture sample
			// Then evaluate the SH for all bands in that hemisphere direction
			// Final SH coeff: 
			// 4PI/2N * SUM(2N, Sh(nInSphere) * Occl(nInSphere)) = 
			// 4PI/2N * SUM(N, Sh(nInHemi) * Occl(nInHemi)) =
			// SUM( N, Sh(nInHemi) * (4PI/2N * Occl(nInHemi)) )

			uint data = pParaTexDataDual[index];
			float occlusionAmountB = (data & 0xFF) * BYTE_RCP;
			occlusionAmountB *= (PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index] * dualSelfOcclScale);
			float occlusionAmountG = ((data >> 8) & 0xFF) * BYTE_RCP;
			occlusionAmountG *= (PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index] * dualSelfOcclScale);
			float occlusionAmountR = ((data >> 16) & 0xFF) * BYTE_RCP;
			occlusionAmountR *= (PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index] * dualSelfOcclScale);

			_LOOPk(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
			{
				float ylm = EvaluateSHFastCartesian(k, _CAST_VEC3(dir));

				red.sh[k] += (ylm * occlusionAmountR);
				green.sh[k] += (ylm * occlusionAmountG);
				blue.sh[k] += (ylm * occlusionAmountB);
			}
		}
	}

	_LOOPi(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
	{
		pDataEntryRDest->sh[i] = red.sh[i];
		pDataEntryGDest->sh[i] = green.sh[i];
		pDataEntryBDest->sh[i] = blue.sh[i];
	}
}

struct CompressOcclTexToSHJob : public IPRunnable
{
	CSHPRTCompute* pOwner;
	ObjectPoolAtomic<CompressOcclTexToSHJob, 4>* pOwnerPool;
	
	SHPRTDataEntry* pDataR;
	SHPRTDataEntry* pDataG;
	SHPRTDataEntry* pDataB;
	gmtl::Matrix44f paraView;
	uint texData[SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE];
	
	int Run()
	{
		pOwner->CompressOcclTexDataToSH(texData, pDataR, pDataG, pDataB, paraView);
		pOwnerPool->Dispose(this);

		return 0;
	}
};

struct CompressOcclTexDualToSHJob : public IPRunnable
{
	CSHPRTCompute* pOwner;
	ObjectPoolAtomic<CompressOcclTexDualToSHJob, 4>* pOwnerPool;

	SHPRTDataEntry* pDataR;
	SHPRTDataEntry* pDataG;
	SHPRTDataEntry* pDataB;
	gmtl::Matrix44f paraView;
	gmtl::Matrix44f paraViewDual;
	uint texData[SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE];
	uint texDataDual[SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE];
	float dualSelfOcclScale;

	int Run()
	{
		pOwner->CompressOcclTexDataDualToSH(texData, texDataDual, pDataR, pDataG, pDataB, paraView, paraViewDual, dualSelfOcclScale);
		pOwnerPool->Dispose(this);

		return 0;
	}
};

void CSHPRTCompute::ComputeOccludedTransferDual(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, 
											SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dualSelfOcclScale,
											ISHPRTComputeOccludedCallback& occlCallback,
											IPThreadPool* pThreadPool)
{
	// TODO: Should let the app allocate mem
	ObjectPoolAtomic<CompressOcclTexDualToSHJob, 4> compressJobPool;
	ObjectPoolAtomic<CompressOcclTexDualToSHJob, 4>* pCompressJobPool = &compressJobPool;

	int readBackRTIndex = -((int)NUM_LATENCY_BUFFERS);

	uint curDataEntry = 0;
	gmtl::MatrixA44f paraViewStore[NUM_LATENCY_BUFFERS];
	gmtl::MatrixA44f paraViewStoreDual[NUM_LATENCY_BUFFERS];

	_LOOPi(numEntries)
	{		
		if(m_pRenderer->BeginRender())
		{
			readBackRTIndex++;

			// For each location entry, render the scene and set
			// the appropriate render states

			BFXRenderContainer renderContainer;
			BFXParamContainer paramContainer;
			//renderContainer.SetRenderCallback((IBFXRenderCallback*) &m_Pipeline, SHPRTCOMPUTE_LIB_ID);
			paramContainer.SetParamCallback((IBFXParamCallback*) &m_ParamPool, SHPRTCOMPUTE_LIB_ID);

			// Derive and set the paraboloid view matrix for the current location
			gmtl::MatrixA44f paraView;
			gmtl::VecA3f x(1.0f, 0.0f, 0.0f);

			// Singularity check
			if( pLocEntries[i].normal[0] > 0.99f )
			{
				x[0] = 0.0f;
				x[1] = -1.0f;
				x[2] = 0.0f;
			}
			else if( pLocEntries[i].normal[0] < -0.99f )
			{
				x[0] = 0.0f;
				x[1] = 1.0f;
				x[2] = 0.0f;
			}

			gmtl::VecA3f z;
			gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal));
			gmtl::normalize(_CAST_VEC3(z));

			gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal), _CAST_VEC3(z));
			gmtl::normalize(_CAST_VEC3(x));

			gmtl::setViewWithAxis(paraView, pLocEntries[i].pos, x, pLocEntries[i].normal, z); 

			m_ParamPool.SetParaboloidViewMatrix(&_CAST_MAT44(paraView));

			// Then set the current render target
			int curRTIndex = i % NUM_LATENCY_BUFFERS;
			m_Pipeline.SetOcclusionRT(curRTIndex);
			m_Pipeline.Clear();

			// Begin app callback for render pipeline
			occlCallback.Render(pLocEntries[i].pos, paraView, renderContainer, paramContainer, m_Pipeline.GetOcclusionRT(curRTIndex), m_Pipeline.GetOcclusionDB());

			m_ParamPool.ResetParams();
			paraViewStore[curRTIndex] = paraView;

			// Fake other hemisphere
			gmtl::VecA3f flippedNormal = -pLocEntries[i].normal;
			x = gmtl::VecA3f(1.0f, 0.0f, 0.0f);
			if( flippedNormal[0] > 0.99f )
			{
				x[0] = 0.0f;
				x[1] = -1.0f;
				x[2] = 0.0f;
			}
			else if( flippedNormal[0] < -0.99f )
			{
				x[0] = 0.0f;
				x[1] = 1.0f;
				x[2] = 0.0f;
			}
			
			gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(flippedNormal));
			gmtl::normalize(_CAST_VEC3(z));

			gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(flippedNormal), _CAST_VEC3(z));
			gmtl::normalize(_CAST_VEC3(x));
			x = -x;
			z = -z;

			gmtl::setViewWithAxis(paraView, pLocEntries[i].pos, x, flippedNormal, z); 

			paraViewStoreDual[curRTIndex] = paraView;

			IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(curRTIndex)->GetTextureRT(0);
			pOcclusionRT->LockReadOnlyPrepare();

			// Read back from GPU and perform integral
			if(readBackRTIndex >= 0)
			{
				readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;
				pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex)->GetTextureRT(0);

				uint pitch;
				byte* pToRead = pOcclusionRT->LockReadOnly(pitch);
				
				if(pThreadPool)
				{
					CompressOcclTexDualToSHJob* pJob = pCompressJobPool->Get();
					if(!pJob)
					{
						pThreadPool->ProcessJobs();
						pJob = pCompressJobPool->Get();
						_DEBUG_ASSERT(pJob);
					}

					memcpy(pJob->texData, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
					memcpy(pJob->texDataDual, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
					pJob->pOwner = this;
					pJob->pDataR = &pDataEntriesR[curDataEntry];
					pJob->pDataG = &pDataEntriesG[curDataEntry];
					pJob->pDataB = &pDataEntriesB[curDataEntry];
					pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
					pJob->paraViewDual = _CAST_MAT44(paraViewStoreDual[readBackRTIndex]);
					pJob->pOwnerPool = pCompressJobPool;
					pJob->dualSelfOcclScale = dualSelfOcclScale;

					pThreadPool->QueueJob(*pJob);
				}
				else
				{
					// Integrate occlusion amount from texture and store in the PRT data entry
					CompressOcclTexDataDualToSH((uint*) pToRead, (uint*) pToRead, &pDataEntriesR[curDataEntry], &pDataEntriesG[curDataEntry], &pDataEntriesB[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]), _CAST_MAT44(paraViewStoreDual[readBackRTIndex]), dualSelfOcclScale);
				}

				pOcclusionRT->UnlockReadOnly();

				curDataEntry++;
			}
		}
		else
		{
			i--;
		}

	}

	_LOOPi(NUM_LATENCY_BUFFERS-1)
	{
		readBackRTIndex++;

		if(readBackRTIndex >= 0)
		{
			readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;
			IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex)->GetTextureRT(0);
		
			uint pitch;
			byte* pToRead = pOcclusionRT->LockReadOnly(pitch);
			
			if(pThreadPool)
			{
				CompressOcclTexDualToSHJob* pJob = pCompressJobPool->Get();
				if(!pJob)
				{
					pThreadPool->ProcessJobs();
					pJob = pCompressJobPool->Get();
					_DEBUG_ASSERT(pJob);
				}

				memcpy(pJob->texData, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
				memcpy(pJob->texDataDual, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
				pJob->pOwner = this;
				pJob->pDataR = &pDataEntriesR[curDataEntry];
				pJob->pDataG = &pDataEntriesG[curDataEntry];
				pJob->pDataB = &pDataEntriesB[curDataEntry];
				pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
				pJob->paraViewDual = _CAST_MAT44(paraViewStoreDual[readBackRTIndex]);
				pJob->pOwnerPool = pCompressJobPool;
				pJob->dualSelfOcclScale = dualSelfOcclScale;

				pThreadPool->QueueJob(*pJob);
			}
			else
			{
				// Integrate occlusion amount from texture and store in the PRT data entry
				CompressOcclTexDataDualToSH((uint*) pToRead, (uint*) pToRead, &pDataEntriesR[curDataEntry], &pDataEntriesG[curDataEntry], &pDataEntriesB[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]), _CAST_MAT44(paraViewStoreDual[readBackRTIndex]), dualSelfOcclScale);
			}

			pOcclusionRT->UnlockReadOnly();
			
			curDataEntry++;
		}
	}

	if(pThreadPool)
	{
		// Finish processing pending jobs if any
		pThreadPool->ProcessJobs();

		//_DELETE(pCompressJobPool);
	}
}

void CSHPRTCompute::ComputeOccludedTransferSphere(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, 
												SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dualSelfOcclScale,
												ISHPRTComputeOccludedCallback& occlCallback,
												IPThreadPool* pThreadPool)
{
	// TODO: Should let the app allocate mem
	ObjectPoolAtomic<CompressOcclTexDualToSHJob, 4> compressJobPool;
	ObjectPoolAtomic<CompressOcclTexDualToSHJob, 4>* pCompressJobPool = &compressJobPool;

	int readBackRTIndex = -((int)NUM_LATENCY_BUFFERS);

	uint curDataEntry = 0;
	gmtl::MatrixA44f paraViewStore[NUM_LATENCY_BUFFERS];
	gmtl::MatrixA44f paraViewStoreDual[NUM_LATENCY_BUFFERS];

	_LOOPi(numEntries)
	{		
		if(m_pRenderer->BeginRender())
		{
			readBackRTIndex++;

			// For each location entry, render the scene and set
			// the appropriate render states

			BFXRenderContainer renderContainer;
			BFXParamContainer paramContainer;
			//renderContainer.SetRenderCallback((IBFXRenderCallback*) &m_Pipeline, SHPRTCOMPUTE_LIB_ID);
			paramContainer.SetParamCallback((IBFXParamCallback*) &m_ParamPool, SHPRTCOMPUTE_LIB_ID);

			// Derive and set the paraboloid view matrix for the current location
			gmtl::MatrixA44f paraView;
			gmtl::VecA3f x(1.0f, 0.0f, 0.0f);

			// Singularity check
			if( pLocEntries[i].normal[0] > 0.99f )
			{
				x[0] = 0.0f;
				x[1] = -1.0f;
				x[2] = 0.0f;
			}
			else if( pLocEntries[i].normal[0] < -0.99f )
			{
				x[0] = 0.0f;
				x[1] = 1.0f;
				x[2] = 0.0f;
			}

			gmtl::VecA3f z;
			gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal));
			gmtl::normalize(_CAST_VEC3(z));

			gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal), _CAST_VEC3(z));
			gmtl::normalize(_CAST_VEC3(x));

			gmtl::setViewWithAxis(paraView, pLocEntries[i].pos, x, pLocEntries[i].normal, z); 

			m_ParamPool.SetParaboloidViewMatrix(&_CAST_MAT44(paraView));

			// Then set the current render target
			int curRTIndex = i % NUM_LATENCY_BUFFERS;
			m_Pipeline.SetOcclusionRT(curRTIndex);
			m_Pipeline.Clear();

			// Begin app callback for render pipeline
			occlCallback.Render(pLocEntries[i].pos, paraView, renderContainer, paramContainer, m_Pipeline.GetOcclusionRT(curRTIndex), m_Pipeline.GetOcclusionDB());

			m_ParamPool.ResetParams();
			paraViewStore[curRTIndex] = paraView;
			
			// Render other hemisphere
			gmtl::VecA3f flippedNormal = -pLocEntries[i].normal;
			x = gmtl::VecA3f(1.0f, 0.0f, 0.0f);
			if( flippedNormal[0] > 0.99f )
			{
				x[0] = 0.0f;
				x[1] = -1.0f;
				x[2] = 0.0f;
			}
			else if( flippedNormal[0] < -0.99f )
			{
				x[0] = 0.0f;
				x[1] = 1.0f;
				x[2] = 0.0f;
			}
			gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(flippedNormal));
			gmtl::normalize(_CAST_VEC3(z));

			gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(flippedNormal), _CAST_VEC3(z));
			gmtl::normalize(_CAST_VEC3(x));

			gmtl::setViewWithAxis(paraView, pLocEntries[i].pos, x, flippedNormal, z); 

			m_ParamPool.SetParaboloidViewMatrix(&_CAST_MAT44(paraView));

			// Then set the current render target
			m_Pipeline.SetOcclusionRTDual(curRTIndex);
			m_Pipeline.Clear();

			// Begin app callback for render pipeline
			occlCallback.Render(pLocEntries[i].pos, paraView, renderContainer, paramContainer, m_Pipeline.GetOcclusionRTDual(curRTIndex), m_Pipeline.GetOcclusionDB());

			m_ParamPool.ResetParams();
			m_pRenderer->EndRender();

			paraViewStoreDual[curRTIndex] = paraView;

			IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(curRTIndex)->GetTextureRT(0);
			pOcclusionRT->LockReadOnlyPrepare();
			IRTextureRT* pOcclusionRTDual = m_Pipeline.GetOcclusionRTDual(curRTIndex)->GetTextureRT(0);
			pOcclusionRTDual->LockReadOnlyPrepare();

			// Read back from GPU and perform integral
			if(readBackRTIndex >= 0)
			{
				readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;
				pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex)->GetTextureRT(0);
				pOcclusionRTDual = m_Pipeline.GetOcclusionRTDual(readBackRTIndex)->GetTextureRT(0);
				//IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex);
				//pOcclusionRT->LockReadOnlyPrepare();
				//IRTextureRT* pOcclusionRTDual = m_Pipeline.GetOcclusionRTDual(readBackRTIndex);
				//pOcclusionRTDual->LockReadOnlyPrepare();

				uint pitch;
				byte* pToRead = pOcclusionRT->LockReadOnly(pitch);
				uint pitchDual;
				byte* pToReadDual = pOcclusionRTDual->LockReadOnly(pitchDual);

				if(pThreadPool)
				{
					CompressOcclTexDualToSHJob* pJob = pCompressJobPool->Get();
					if(!pJob)
					{
						pThreadPool->ProcessJobs();
						pJob = pCompressJobPool->Get();
						_DEBUG_ASSERT(pJob);
					}

					memcpy(pJob->texData, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
					memcpy(pJob->texDataDual, pToReadDual, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
					pJob->pOwner = this;
					pJob->pDataR = &pDataEntriesR[curDataEntry];
					pJob->pDataG = &pDataEntriesG[curDataEntry];
					pJob->pDataB = &pDataEntriesB[curDataEntry];
					pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
					pJob->paraViewDual = _CAST_MAT44(paraViewStoreDual[readBackRTIndex]);
					pJob->pOwnerPool = pCompressJobPool;
					pJob->dualSelfOcclScale = dualSelfOcclScale;

					pThreadPool->QueueJob(*pJob);
				}
				else
				{
					// Integrate occlusion amount from texture and store in the PRT data entry
					CompressOcclTexDataDualToSH((uint*) pToRead, (uint*) pToReadDual, &pDataEntriesR[curDataEntry], &pDataEntriesG[curDataEntry], &pDataEntriesB[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]), _CAST_MAT44(paraViewStoreDual[readBackRTIndex]), dualSelfOcclScale);
				}

				pOcclusionRT->UnlockReadOnly();
				pOcclusionRTDual->UnlockReadOnly();

				curDataEntry++;
			}
		}
		else
		{
			i--;
		}

	}

	_LOOPi(NUM_LATENCY_BUFFERS-1)
	{
		readBackRTIndex++;

		if(readBackRTIndex >= 0)
		{
			readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;
			IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex)->GetTextureRT(0);
			//pOcclusionRT->LockReadOnlyPrepare();
			IRTextureRT* pOcclusionRTDual = m_Pipeline.GetOcclusionRTDual(readBackRTIndex)->GetTextureRT(0);
			//pOcclusionRTDual->LockReadOnlyPrepare();

			uint pitch;
			byte* pToRead = pOcclusionRT->LockReadOnly(pitch);
			uint pitchDual;
			byte* pToReadDual = pOcclusionRTDual->LockReadOnly(pitchDual);

			if(pThreadPool)
			{
				CompressOcclTexDualToSHJob* pJob = pCompressJobPool->Get();
				if(!pJob)
				{
					pThreadPool->ProcessJobs();
					pJob = pCompressJobPool->Get();
					_DEBUG_ASSERT(pJob);
				}

				memcpy(pJob->texData, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
				memcpy(pJob->texDataDual, pToReadDual, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
				pJob->pOwner = this;
				pJob->pDataR = &pDataEntriesR[curDataEntry];
				pJob->pDataG = &pDataEntriesG[curDataEntry];
				pJob->pDataB = &pDataEntriesB[curDataEntry];
				pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
				pJob->paraViewDual = _CAST_MAT44(paraViewStoreDual[readBackRTIndex]);
				pJob->pOwnerPool = pCompressJobPool;
				pJob->dualSelfOcclScale = dualSelfOcclScale;

				pThreadPool->QueueJob(*pJob);
			}
			else
			{
				// Integrate occlusion amount from texture and store in the PRT data entry
				CompressOcclTexDataDualToSH((uint*) pToRead, (uint*) pToReadDual, &pDataEntriesR[curDataEntry], &pDataEntriesG[curDataEntry], &pDataEntriesB[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]), _CAST_MAT44(paraViewStoreDual[readBackRTIndex]), dualSelfOcclScale);
			}

			pOcclusionRT->UnlockReadOnly();
			pOcclusionRTDual->UnlockReadOnly();

			curDataEntry++;
		}
	}

	if(pThreadPool)
	{
		// Finish processing pending jobs if any
		pThreadPool->ProcessJobs();
		//_DELETE(pCompressJobPool);
	}
}

void CSHPRTCompute::ComputeOccludedTransfer(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, 
											SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, 
											ISHPRTComputeOccludedCallback& occlCallback,
											IPThreadPool* pThreadPool)
{	
	// TODO: Should let the app allocate mem
	ObjectPoolAtomic<CompressOcclTexToSHJob, 4> compressJobPool;
	ObjectPoolAtomic<CompressOcclTexToSHJob, 4>* pCompressJobPool = &compressJobPool;
	//if(pThreadPool)
	//	pCompressJobPool = _NEW ObjectPoolAtomic<CompressOcclTexToSHJob, 7>;

	//ObjectPoolAtomic<CompressOcclTexToSHJob, 5> compressJobPool;

	int readBackRTIndex = -((int)NUM_LATENCY_BUFFERS);

	uint curDataEntry = 0;
	gmtl::MatrixA44f paraViewStore[NUM_LATENCY_BUFFERS];

	_LOOPi(numEntries)
	{		
		if(m_pRenderer->BeginRender())
		{
			readBackRTIndex++;

			// For each location entry, render the scene and set
			// the appropriate render states
		
			BFXRenderContainer renderContainer;
			BFXParamContainer paramContainer;
			//renderContainer.SetRenderCallback((IBFXRenderCallback*) &m_Pipeline, SHPRTCOMPUTE_LIB_ID);
			paramContainer.SetParamCallback((IBFXParamCallback*) &m_ParamPool, SHPRTCOMPUTE_LIB_ID);

			// Derive and set the paraboloid view matrix for the current location
			gmtl::MatrixA44f paraView;
			gmtl::VecA3f x(1.0f, 0.0f, 0.0f);

			// Singularity check
			if( pLocEntries[i].normal[0] > 0.99f )
			{
				x[0] = 0.0f;
				x[1] = -1.0f;
				x[2] = 0.0f;
			}
			else if( pLocEntries[i].normal[0] < -0.99f )
			{
				x[0] = 0.0f;
				x[1] = 1.0f;
				x[2] = 0.0f;
			}

			gmtl::VecA3f z;
			gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal));
			gmtl::normalize(_CAST_VEC3(z));

			gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal), _CAST_VEC3(z));
			gmtl::normalize(_CAST_VEC3(x));

			gmtl::setViewWithAxis(paraView, pLocEntries[i].pos, x, pLocEntries[i].normal, z); 

			m_ParamPool.SetParaboloidViewMatrix(&_CAST_MAT44(paraView));

			// Then set the current render target
			int curRTIndex = i % NUM_LATENCY_BUFFERS;
			m_Pipeline.SetOcclusionRT(curRTIndex);
			m_Pipeline.Clear();

			// Begin app callback for render pipeline
			occlCallback.Render(pLocEntries[i].pos, paraView, renderContainer, paramContainer, m_Pipeline.GetOcclusionRT(curRTIndex), m_Pipeline.GetOcclusionDB());

			m_ParamPool.ResetParams();
			m_pRenderer->EndRender();

			//if(readBackRTIndex >= 0)
			//{
			//	readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;

			//	// Red
			//	IRTextureRT* pAccumulatedRedBand1RT = m_Pipeline.GetAccumulatedRedBand1RT(readBackRTIndex);
			//	pAccumulatedRedBand1RT->LockReadOnlyPrepare();
			//	IRTextureRT* pAccumulatedRedBand2RT = m_Pipeline.GetAccumulatedRedBand2RT(readBackRTIndex);
			//	pAccumulatedRedBand2RT->LockReadOnlyPrepare();
			//	IRTextureRT* pAccumulatedRedBandLastRT = m_Pipeline.GetAccumulatedRedBandLastRT(readBackRTIndex);
			//	pAccumulatedRedBandLastRT->LockReadOnlyPrepare();

			//	// Green
			//	IRTextureRT* pAccumulatedGreenBand1RT = m_Pipeline.GetAccumulatedGreenBand1RT(readBackRTIndex);
			//	pAccumulatedGreenBand1RT->LockReadOnlyPrepare();
			//	IRTextureRT* pAccumulatedGreenBand2RT = m_Pipeline.GetAccumulatedGreenBand2RT(readBackRTIndex);
			//	pAccumulatedGreenBand2RT->LockReadOnlyPrepare();
			//	IRTextureRT* pAccumulatedGreenBandLastRT = m_Pipeline.GetAccumulatedGreenBandLastRT(readBackRTIndex);
			//	pAccumulatedGreenBandLastRT->LockReadOnlyPrepare();

			//	// Blue
			//	IRTextureRT* pAccumulatedBlueBand1RT = m_Pipeline.GetAccumulatedBlueBand1RT(readBackRTIndex);
			//	pAccumulatedBlueBand1RT->LockReadOnlyPrepare();
			//	IRTextureRT* pAccumulatedBlueBand2RT = m_Pipeline.GetAccumulatedBlueBand2RT(readBackRTIndex);
			//	pAccumulatedBlueBand2RT->LockReadOnlyPrepare();
			//	IRTextureRT* pAccumulatedBlueBandLastRT = m_Pipeline.GetAccumulatedBlueBandLastRT(readBackRTIndex);
			//	pAccumulatedBlueBandLastRT->LockReadOnlyPrepare();

			//	// Red
			//	uint pitch;
			//	float* pToRead = (float*) pAccumulatedRedBand1RT->LockReadOnly(pitch);
			//	pDataEntriesR[curDataEntry].sh[0] = pToRead[0];
			//	pDataEntriesR[curDataEntry].sh[1] = pToRead[1];
			//	pDataEntriesR[curDataEntry].sh[2] = pToRead[2];
			//	pDataEntriesR[curDataEntry].sh[3] = pToRead[3];
			//	pAccumulatedRedBand1RT->UnlockReadOnly();

			//	pToRead = (float*) pAccumulatedRedBand2RT->LockReadOnly(pitch);
			//	pDataEntriesR[curDataEntry].sh[4] = pToRead[0];
			//	pDataEntriesR[curDataEntry].sh[5] = pToRead[1];
			//	pDataEntriesR[curDataEntry].sh[6] = pToRead[2];
			//	pDataEntriesR[curDataEntry].sh[7] = pToRead[3];
			//	pAccumulatedRedBand2RT->UnlockReadOnly();

			//	pToRead = (float*) pAccumulatedRedBandLastRT->LockReadOnly(pitch);
			//	pDataEntriesR[curDataEntry].sh[8] = pToRead[0];
			//	pAccumulatedRedBandLastRT->UnlockReadOnly();

			//	// Green
			//	pToRead = (float*) pAccumulatedGreenBand1RT->LockReadOnly(pitch);
			//	pDataEntriesG[curDataEntry].sh[0] = pToRead[0];
			//	pDataEntriesG[curDataEntry].sh[1] = pToRead[1];
			//	pDataEntriesG[curDataEntry].sh[2] = pToRead[2];
			//	pDataEntriesG[curDataEntry].sh[3] = pToRead[3];
			//	pAccumulatedGreenBand1RT->UnlockReadOnly();

			//	pToRead = (float*) pAccumulatedGreenBand2RT->LockReadOnly(pitch);
			//	pDataEntriesG[curDataEntry].sh[4] = pToRead[0];
			//	pDataEntriesG[curDataEntry].sh[5] = pToRead[1];
			//	pDataEntriesG[curDataEntry].sh[6] = pToRead[2];
			//	pDataEntriesG[curDataEntry].sh[7] = pToRead[3];
			//	pAccumulatedGreenBand2RT->UnlockReadOnly();

			//	pToRead = (float*) pAccumulatedGreenBandLastRT->LockReadOnly(pitch);
			//	pDataEntriesG[curDataEntry].sh[8] = pToRead[0];
			//	pAccumulatedGreenBandLastRT->UnlockReadOnly();

			//	// Blue
			//	pToRead = (float*) pAccumulatedBlueBand1RT->LockReadOnly(pitch);
			//	pDataEntriesB[curDataEntry].sh[0] = pToRead[0];
			//	pDataEntriesB[curDataEntry].sh[1] = pToRead[1];
			//	pDataEntriesB[curDataEntry].sh[2] = pToRead[2];
			//	pDataEntriesB[curDataEntry].sh[3] = pToRead[3];
			//	pAccumulatedBlueBand1RT->UnlockReadOnly();

			//	pToRead = (float*) pAccumulatedBlueBand2RT->LockReadOnly(pitch);
			//	pDataEntriesB[curDataEntry].sh[4] = pToRead[0];
			//	pDataEntriesB[curDataEntry].sh[5] = pToRead[1];
			//	pDataEntriesB[curDataEntry].sh[6] = pToRead[2];
			//	pDataEntriesB[curDataEntry].sh[7] = pToRead[3];
			//	pAccumulatedBlueBand2RT->UnlockReadOnly();

			//	pToRead = (float*) pAccumulatedBlueBandLastRT->LockReadOnly(pitch);
			//	pDataEntriesB[curDataEntry].sh[8] = pToRead[0];
			//	pAccumulatedBlueBandLastRT->UnlockReadOnly();

			//	curDataEntry++;
			//}

			paraViewStore[curRTIndex] = paraView;

			// Read back from GPU and perform integral
			if(readBackRTIndex >= 0)
			{
				readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;
				IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex)->GetTextureRT(0);
				pOcclusionRT->LockReadOnlyPrepare();
				
				uint pitch;
				byte* pToRead = pOcclusionRT->LockReadOnly(pitch);

				if(pThreadPool)
				{
					CompressOcclTexToSHJob* pJob = pCompressJobPool->Get();
					if(!pJob)
					{
						pThreadPool->ProcessJobs();
						pJob = pCompressJobPool->Get();
						_DEBUG_ASSERT(pJob);
					}

					memcpy(pJob->texData, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
					pJob->pOwner = this;
					pJob->pDataR = &pDataEntriesR[curDataEntry];
					pJob->pDataG = &pDataEntriesG[curDataEntry];
					pJob->pDataB = &pDataEntriesB[curDataEntry];
					pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
					pJob->pOwnerPool = pCompressJobPool;

					pThreadPool->QueueJob(*pJob);
				}
				else
				{
					// Integrate occlusion amount from texture and store in the PRT data entry
					CompressOcclTexDataToSH((uint*) pToRead, &pDataEntriesR[curDataEntry], &pDataEntriesG[curDataEntry], &pDataEntriesB[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]));
				}

				pOcclusionRT->UnlockReadOnly();

				curDataEntry++;
			}
		}
		else
		{
			i--;
		}
		
	}
	
	_LOOPi(NUM_LATENCY_BUFFERS-1)
	{
		readBackRTIndex++;

		//if(readBackRTIndex >= 0)
		//{
		//	readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;

		//	// Red
		//	IRTextureRT* pAccumulatedRedBand1RT = m_Pipeline.GetAccumulatedRedBand1RT(readBackRTIndex);
		//	pAccumulatedRedBand1RT->LockReadOnlyPrepare();
		//	IRTextureRT* pAccumulatedRedBand2RT = m_Pipeline.GetAccumulatedRedBand2RT(readBackRTIndex);
		//	pAccumulatedRedBand2RT->LockReadOnlyPrepare();
		//	IRTextureRT* pAccumulatedRedBandLastRT = m_Pipeline.GetAccumulatedRedBandLastRT(readBackRTIndex);
		//	pAccumulatedRedBandLastRT->LockReadOnlyPrepare();

		//	// Green
		//	IRTextureRT* pAccumulatedGreenBand1RT = m_Pipeline.GetAccumulatedGreenBand1RT(readBackRTIndex);
		//	pAccumulatedGreenBand1RT->LockReadOnlyPrepare();
		//	IRTextureRT* pAccumulatedGreenBand2RT = m_Pipeline.GetAccumulatedGreenBand2RT(readBackRTIndex);
		//	pAccumulatedGreenBand2RT->LockReadOnlyPrepare();
		//	IRTextureRT* pAccumulatedGreenBandLastRT = m_Pipeline.GetAccumulatedGreenBandLastRT(readBackRTIndex);
		//	pAccumulatedGreenBandLastRT->LockReadOnlyPrepare();

		//	// Blue
		//	IRTextureRT* pAccumulatedBlueBand1RT = m_Pipeline.GetAccumulatedBlueBand1RT(readBackRTIndex);
		//	pAccumulatedBlueBand1RT->LockReadOnlyPrepare();
		//	IRTextureRT* pAccumulatedBlueBand2RT = m_Pipeline.GetAccumulatedBlueBand2RT(readBackRTIndex);
		//	pAccumulatedBlueBand2RT->LockReadOnlyPrepare();
		//	IRTextureRT* pAccumulatedBlueBandLastRT = m_Pipeline.GetAccumulatedBlueBandLastRT(readBackRTIndex);
		//	pAccumulatedBlueBandLastRT->LockReadOnlyPrepare();

		//	// Red
		//	uint pitch;
		//	float* pToRead = (float*) pAccumulatedRedBand1RT->LockReadOnly(pitch);
		//	pDataEntriesR[curDataEntry].sh[0] = pToRead[0];
		//	pDataEntriesR[curDataEntry].sh[1] = pToRead[1];
		//	pDataEntriesR[curDataEntry].sh[2] = pToRead[2];
		//	pDataEntriesR[curDataEntry].sh[3] = pToRead[3];
		//	pAccumulatedRedBand1RT->UnlockReadOnly();

		//	pToRead = (float*) pAccumulatedRedBand2RT->LockReadOnly(pitch);
		//	pDataEntriesR[curDataEntry].sh[4] = pToRead[0];
		//	pDataEntriesR[curDataEntry].sh[5] = pToRead[1];
		//	pDataEntriesR[curDataEntry].sh[6] = pToRead[2];
		//	pDataEntriesR[curDataEntry].sh[7] = pToRead[3];
		//	pAccumulatedRedBand2RT->UnlockReadOnly();

		//	pToRead = (float*) pAccumulatedRedBandLastRT->LockReadOnly(pitch);
		//	pDataEntriesR[curDataEntry].sh[8] = pToRead[0];
		//	pAccumulatedRedBandLastRT->UnlockReadOnly();

		//	// Green
		//	pToRead = (float*) pAccumulatedGreenBand1RT->LockReadOnly(pitch);
		//	pDataEntriesG[curDataEntry].sh[0] = pToRead[0];
		//	pDataEntriesG[curDataEntry].sh[1] = pToRead[1];
		//	pDataEntriesG[curDataEntry].sh[2] = pToRead[2];
		//	pDataEntriesG[curDataEntry].sh[3] = pToRead[3];
		//	pAccumulatedGreenBand1RT->UnlockReadOnly();

		//	pToRead = (float*) pAccumulatedGreenBand2RT->LockReadOnly(pitch);
		//	pDataEntriesG[curDataEntry].sh[4] = pToRead[0];
		//	pDataEntriesG[curDataEntry].sh[5] = pToRead[1];
		//	pDataEntriesG[curDataEntry].sh[6] = pToRead[2];
		//	pDataEntriesG[curDataEntry].sh[7] = pToRead[3];
		//	pAccumulatedGreenBand2RT->UnlockReadOnly();

		//	pToRead = (float*) pAccumulatedGreenBandLastRT->LockReadOnly(pitch);
		//	pDataEntriesG[curDataEntry].sh[8] = pToRead[0];
		//	pAccumulatedGreenBandLastRT->UnlockReadOnly();

		//	// Blue
		//	pToRead = (float*) pAccumulatedBlueBand1RT->LockReadOnly(pitch);
		//	pDataEntriesB[curDataEntry].sh[0] = pToRead[0];
		//	pDataEntriesB[curDataEntry].sh[1] = pToRead[1];
		//	pDataEntriesB[curDataEntry].sh[2] = pToRead[2];
		//	pDataEntriesB[curDataEntry].sh[3] = pToRead[3];
		//	pAccumulatedBlueBand1RT->UnlockReadOnly();

		//	pToRead = (float*) pAccumulatedBlueBand2RT->LockReadOnly(pitch);
		//	pDataEntriesB[curDataEntry].sh[4] = pToRead[0];
		//	pDataEntriesB[curDataEntry].sh[5] = pToRead[1];
		//	pDataEntriesB[curDataEntry].sh[6] = pToRead[2];
		//	pDataEntriesB[curDataEntry].sh[7] = pToRead[3];
		//	pAccumulatedBlueBand2RT->UnlockReadOnly();

		//	pToRead = (float*) pAccumulatedBlueBandLastRT->LockReadOnly(pitch);
		//	pDataEntriesB[curDataEntry].sh[8] = pToRead[0];
		//	pAccumulatedBlueBandLastRT->UnlockReadOnly();

		//	curDataEntry++;
		//}

		if(readBackRTIndex >= 0)
		{
			readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;
			IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex)->GetTextureRT(0);
			pOcclusionRT->LockReadOnlyPrepare();

			// TODO: This portion should be threaded
			uint pitch;
			byte* pToRead = pOcclusionRT->LockReadOnly(pitch);

			if(pThreadPool)
			{
				CompressOcclTexToSHJob* pJob = pCompressJobPool->Get();
				if(!pJob)
				{
					pThreadPool->ProcessJobs();
					pJob = pCompressJobPool->Get();
					_DEBUG_ASSERT(pJob);
				}

				memcpy(pJob->texData, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
				pJob->pOwner = this;
				pJob->pDataR = &pDataEntriesR[curDataEntry];
				pJob->pDataG = &pDataEntriesG[curDataEntry];
				pJob->pDataB = &pDataEntriesB[curDataEntry];
				pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
				pJob->pOwnerPool = pCompressJobPool;

				pThreadPool->QueueJob(*pJob);
			}
			else
			{
				// Integrate occlusion amount from texture and store in the PRT data entry
				CompressOcclTexDataToSH((uint*) pToRead, &pDataEntriesR[curDataEntry], &pDataEntriesG[curDataEntry], &pDataEntriesB[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]));
			}

			pOcclusionRT->UnlockReadOnly();

			curDataEntry++;
		}
	}

	if(pThreadPool)
	{
		// Finish processing pending jobs if any
		pThreadPool->ProcessJobs();

		//_DELETE(pCompressJobPool);
	}
}

IRTexture2D* CSHPRTCompute::ComputeOccludedTransferDebug(const SHPRTLocationEntry& locEntry, ISHPRTComputeOccludedCallback& occlCallback)
{
	//_LOOPi(numEntries)
	{		
		if(m_pRenderer->BeginRender())
		{
			// For each location entry, render the scene and set
			// the appropriate render states

			BFXRenderContainer renderContainer;
			BFXParamContainer paramContainer;
			//renderContainer.SetRenderCallback((IBFXRenderCallback*) &m_Pipeline, SHPRTCOMPUTE_LIB_ID);
			paramContainer.SetParamCallback((IBFXParamCallback*) &m_ParamPool, SHPRTCOMPUTE_LIB_ID);

			// Derive and set the paraboloid view matrix for the current location
			gmtl::MatrixA44f paraView;
			gmtl::VecA3f x(1.0f, 0.0f, 0.0f);

			// Singularity check
			if( locEntry.normal[0] > 0.99f )
			{
				x[0] = 0.0f;
				x[1] = -1.0f;
				x[2] = 0.0f;
			}
			else if( locEntry.normal[0] < -0.99f )
			{
				x[0] = 0.0f;
				x[1] = 1.0f;
				x[2] = 0.0f;
			}

			gmtl::VecA3f z;
			gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(locEntry.normal));
			gmtl::normalize(_CAST_VEC3(z));

			gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(locEntry.normal), _CAST_VEC3(z));
			gmtl::normalize(_CAST_VEC3(x));

			gmtl::setViewWithAxis(paraView, locEntry.pos, x, locEntry.normal, z); 

			m_ParamPool.SetParaboloidViewMatrix(&_CAST_MAT44(paraView));

			// Then set the current render target
			int curRTIndex = 0;
			m_Pipeline.SetOcclusionRT(curRTIndex);
			m_Pipeline.Clear();

			// Begin app callback for render pipeline
			occlCallback.Render(locEntry.pos, paraView, renderContainer, paramContainer, m_Pipeline.GetOcclusionRT(curRTIndex), m_Pipeline.GetOcclusionDB());

			m_ParamPool.ResetParams();
			m_pRenderer->EndRender();

			return m_Pipeline.GetOcclusionRT(curRTIndex);
		}
	}

	return NULL;
}

void CSHPRTCompute::ZeroTransfer(SHPRTDataEntry* pDataEntry, uint numEntries)
{
	memset(pDataEntry, 0, sizeof(SHPRTDataEntry) * numEntries);	
}

void CSHPRTCompute::ComputeBRDFTransfer(const SHPRTLocationEntry* pLocEntry, SHPRTDataEntry* pDataEntries, uint numEntries, ISHPRTComputeBRDFCallback& brdfCallback)
{
	float albedos[SHPRT_BRDF_SAMPLE_RANGE * SHPRT_BRDF_SAMPLE_RANGE];
	gmtl::VecA3f dirs[SHPRT_BRDF_SAMPLE_RANGE * SHPRT_BRDF_SAMPLE_RANGE];

	_LOOPi(SHPRT_BRDF_SAMPLE_RANGE * SHPRT_BRDF_SAMPLE_RANGE)
	{
		dirs[i][0] = BRDF_DIRS[i].x;
		dirs[i][1] = BRDF_DIRS[i].y;
		dirs[i][2] = BRDF_DIRS[i].z;
	}

	const float weight = (4.0f * gmtl::Math::PI) * (1.0f / (SHPRT_BRDF_SAMPLE_RANGE * SHPRT_BRDF_SAMPLE_RANGE)) * 0.5f;

	_LOOPi(numEntries)
	{
		_LOOPj(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
		{
			pDataEntries[i].sh[j] = 0.0f;
		}

		// Get BRDF from application
		brdfCallback.GetBRDF(pLocEntry[i].normal, dirs, albedos, SHPRT_BRDF_SAMPLE_RANGE * SHPRT_BRDF_SAMPLE_RANGE);
		
		// Perform SH integral over the sphere
		_LOOPj(SHPRT_BRDF_SAMPLE_RANGE * SHPRT_BRDF_SAMPLE_RANGE)
		{
			// Clamp to zero since we don't want negative albedos for whatever reason
			float albedo = albedos[j];
			if(albedo < 0.0f)
				albedo = 0.0f;

			_LOOPk(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
			{
				pDataEntries[i].sh[k] += albedo * EvaluateSHFastCartesian(k, _CAST_VEC3(dirs[j]));
			}
		}

		_LOOPj(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
		{
			pDataEntries[i].sh[j] *= weight;
		}
	}
}

//void CSHPRTCompute::ComputePushPullForTexture(SHPRTDataEntry* pTexDestData, uint texWidth, uint texHeight, const SHPRTLocationEntry* pLocEntry, const SHPRTDataEntry* pSrcData, uint numEntries)
//{
//	// Allocate data for texture
//	//SHPRTDataEntry* pTexData = _NEW SHPRTDataEntry[texWidth * texHeight];
//	memset(pTexDestData, 0xFF, sizeof(SHPRTDataEntry) * texWidth * texHeight);
//
//	// Get indices of unfilled portions of texture data and initialize data
//	std::vector<uint> unfilledEntries;
//	_LOOPi(texHeight)
//	{
//		_LOOPj(texWidth)
//		{
//			unfilledEntries.push_back(i << 16 | j);
//		}
//	}
//
//	_LOOPi(numEntries)
//	{
//		pTexDestData[pLocEntry[i]->texY * texWidth + pLocEntry[i]->texX]= pSrcData[i];
//		unfilledEntries[pLocEntry[i]->texY * texWidth + pLocEntry[i]->texX] = 0xFFFFFFFF;
//	}
//
//	_LOOPi(unfilledEntries.size())
//	{
//		if(unfilledEntries[i] == 0xFFFFFFFF)
//		{
//			_STL_ARRAY_UNORDERED_REMOVE(unfilledEntries, i);
//			i--;
//		}
//	}
//
//	// Do a push-pull for each empty pixel until they are all filled
//	std::vector<SHPRTDataEntry> tempData;
//	std::vector<uint> tempIndices;
//	while(unfilledEntries.size() > 0)
//	{
//		// Do a pass and find a neighbour for unfilled entries
//		_LOOPi(unfilledEntries.size())
//		{
//			uint x = unfilledEntries[i] & 0x0000FFFF;
//			uint y = unfilledEntries[i] >> 16;
//
//			uint index = y * texWidth + x;
//			_DEBUG_ASSERT( *((uint*) &(pTexDestData[index].sh[0])) == 0xFFFFFFFF );
//
//			// Pull from neighbours
//			// Right
//			if(x < texWidth-1)
//			{
//				uint* pSH = (uint*) &(pTexDestData[y * texWidth + (x+1)].sh[0]);
//				if(*pSH != 0xFFFFFFFF)
//				{
//					//pTexData[y * texWidth + x] = pTexData[y * texWidth + (x+1)];
//					tempData.push_back(pTexDestData[y * texWidth + (x+1)]);
//					tempIndices.push_back(index);
//
//					_STL_ARRAY_UNORDERED_REMOVE(unfilledEntries, i);
//					i--;
//					continue;
//				}
//			}
//
//			// Top
//			if(y > 0)
//			{
//				uint* pSH = (uint*) &(pTexDestData[(y-1) * texWidth + x].sh[0]);
//				if(*pSH != 0xFFFFFFFF)
//				{
//					//pTexData[y * texWidth + x] = pTexData[(y-1) * texWidth + x];
//
//					tempData.push_back(pTexDestData[(y-1) * texWidth + x]);
//					tempIndices.push_back(index);
//
//					_STL_ARRAY_UNORDERED_REMOVE(unfilledEntries, i);
//					i--;
//					continue;
//				}
//			}
//
//			// Left
//			if(x > 0)
//			{
//				uint* pSH = (uint*) &(pTexDestData[y * texWidth + (x-1)].sh[0]);
//				if(*pSH != 0xFFFFFFFF)
//				{
//					//pTexData[y * texWidth + x] = pTexData[y * texWidth + (x-1)];
//
//					tempData.push_back(pTexDestData[y * texWidth + (x-1)]);
//					tempIndices.push_back(index);
//
//					_STL_ARRAY_UNORDERED_REMOVE(unfilledEntries, i);
//					i--;
//					continue;
//				}
//			}
//
//			// Bottom
//			if(y < texHeight-1)
//			{
//				uint* pSH = (uint*) &(pTexDestData[(y+1) * texWidth + x].sh[0]);
//				if(*pSH != 0xFFFFFFFF)
//				{
//					//pTexData[y * texWidth + x] = pTexData[(y+1) * texWidth + x];
//
//					tempData.push_back(pTexDestData[(y+1) * texWidth + x]);
//					tempIndices.push_back(index);
//
//					_STL_ARRAY_UNORDERED_REMOVE(unfilledEntries, i);
//					i--;
//					continue;
//				}
//			}
//		}
//
//		// Now actually write to the data
//		_LOOPi(tempData.size())
//		{
//			pTexDestData[tempIndices[i]] = tempData[i];
//		}
//
//		tempData.clear();
//		tempIndices.clear();
//	}
//}
//
//IRTexture* CSHPRTCompute::CreateTransferTextureZH(const SHPRTLocationEntry* pLocEntry, const SHPRTDataEntry* pDataEntry, uint numEntries, uint texWidth, uint texHeight)
//{
//	// Allocate data for texture
//	SHPRTDataEntry* pTexData = _NEW SHPRTDataEntry[texWidth * texHeight];
//	ComputePushPullForTexture(pTexData, texWidth, texHeight, pLocEntry, pDataEntry, numEntries);
//
//	// Then perform a software bilinear on each mip level and copy into the texture
//
//	// Compress to ZH and copy top level
//	IRTexture2D* pTex = m_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, texWidth, texHeight, 0, TEXF_A32B32G32R32F, TEXU_DEFAULT);
//	uint pitch;
//	byte* pDest = pTex->Lock(0, pitch, NULL);
//	
//	gmtl::Vec3f defLobeAxis;
//	defLobeAxis[0] = 0.0f;
//	defLobeAxis[1] = 1.0f;
//	defLobeAxis[2] = 0.0f;
//
//	_DEBUG_ASSERT(NUM_SHPRT_BANDS <= 4);
//
//	_LOOPi(texHeight)
//	{
//		_LOOPj(texWidth)
//		{
//			float zh[NUM_SHPRT_BANDS];			
//			CompressSHToZH(pTexData[i*texWidth+j].sh, zh, NUM_SHPRT_BANDS, defLobeAxis);
//
//			float* pLine = (float*) (pDest + ((i * texWidth + j) * sizeof(float) * 4));
//			_LOOPk(NUM_SHPRT_BANDS)
//			{
//				pLine[k] = zh[k];
//			}
//		}
//	}
//	
//	pTex->Unlock(0);
//
//	uint numMips = pTex->GetMipMapLevels();
//	uint prevMipWidth = texWidth;
//	uint prevMipHeight = texHeight;
//	SHPRTDataEntry* pPrevData = pTexData;
//	_LOOPi(numMips - 1)
//	{
//		uint mipLevel = i + 1;
//
//		uint mipWidth = prevMipWidth >> 1;
//		if(mipWidth == 0)
//			mipWidth = 1;
//
//		uint mipHeight = prevMipHeight >> 1;
//		if(mipHeight == 0)
//			mipHeight = 1;
//
//		_DEBUG_ASSERT(pTex->GetWidth(mipLevel) == mipWidth);
//		_DEBUG_ASSERT(pTex->GetHeight(mipLevel) == mipHeight);
//
//		// Perform bilinear filtering and copy to mip map
//		uint offsetRight = prevMipWidth > 1 ? 1 : 0;
//		uint offsetBottom = prevMipHeight > 1 ? 1 : 0;
//
//		SHPRTDataEntry* pMipData = _NEW SHPRTDataEntry[mipWidth * mipHeight];
//		_LOOPj(mipHeight)
//		{
//			_LOOPk(mipWidth)
//			{				
//				const SHPRTDataEntry& tl = pPrevData[(j*2) * prevMipWidth + (k*2)];
//				const SHPRTDataEntry& tr = pPrevData[(j*2) * prevMipWidth + (k*2+offsetRight)];
//				const SHPRTDataEntry& bl = pPrevData[(j*2+offsetBottom) * prevMipWidth + (k*2)];
//				const SHPRTDataEntry& br = pPrevData[(j*2+offsetBottom) * prevMipWidth + (k*2+offsetRight)];
//
//				SHPRTDataEntry& cur = pMipData[j*mipWidth + k];
//				_LOOP(band, NUM_SHPRT_BANDS*NUM_SHPRT_BANDS)
//				{
//					cur.sh[band] = ((tl.sh[band] + tr.sh[band]) + (bl.sh[band] + br.sh[band])) * 0.25f;
//				}
//			}
//		}
//
//		// Copy		
//		pDest = pTex->Lock(mipLevel, pitch, NULL);
//		_LOOPj(mipHeight)
//		{
//			_LOOPk(mipWidth)
//			{
//				float zh[NUM_SHPRT_BANDS];			
//				CompressSHToZH(pMipData[j*mipWidth+k].sh, zh, NUM_SHPRT_BANDS, defLobeAxis);
//
//				float* pLine = (float*) (pDest + ((j * texWidth + k) * sizeof(float) * 4));
//				_LOOPk(NUM_SHPRT_BANDS)
//				{
//					pLine[k] = zh[k];
//				}
//			}
//		}
//
//		pTex->Unlock(mipLevel);
//
//		_DELETE_ARRAY(pPrevData);
//		pPrevData = pMipData;
//	}
//
//	_DELETE_ARRAY(pPrevData);
//
//	return pTex;
//}

void CSHPRTCompute::CreateBRDFTexturesZH(const SHPRTLocationEntry* pLocEntries, const SHPRTDataEntry* pDataEntries, uint numEntries, uint texWidth, uint texHeight, IRTexture2D** ppTex1, IRTexture2D** ppTex2)
{
	const static float ZH_BIAS[3] = { 0.0f, 0.0f, -2.0f };
	const static float ZH_SCALE[3] = { 1.0f, 3.0f, 4.0f };

	// Allocate data for texture

	// Compress to ZH and copy top level
	// TODO: Try to use A8R8G8B8 format
	IRTexture2D* pTex1 = m_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, texWidth, texHeight, 1, TEXF_A8R8G8B8, TEXU_DEFAULT);
	IRTexture2D* pTex2 = m_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, texWidth, texHeight, 1, TEXF_A8R8, TEXU_DEFAULT);
	uint pitch1;
	uint pitch2;

	byte* pDest1 = pTex1->Lock(0, pitch1, NULL);
	byte* pDest2 = pTex2->Lock(0, pitch2, NULL);

	_LOOPi(numEntries)
	{
		const SHPRTLocationEntry& loc = pLocEntries[i];
		const SHPRTDataEntry& data = pDataEntries[i];

		//float* pLine1 = (float*) (pDest1 + ((loc.texY * texWidth + loc.texX) * sizeof(float) * 4));
		
		byte* pLine1 = (byte*) (pDest1 + ((loc.texY * texWidth + loc.texX) * sizeof(byte) * 4));
		byte* pLine2 = (byte*) (pDest2 + ((loc.texY * texWidth + loc.texX) * sizeof(byte) * 2));

		gmtl::Vec3f optimalLobeAxis;
		EvaluateOptimalLinearZH(data.sh, optimalLobeAxis);
		_DEBUG_ASSERT(optimalLobeAxis[1] >= 0.0f);

		// Compress axis to 1 float
		//float x = (float) ((int)((optimalLobeAxis[0] * 0.5f + 0.5f) * 255.0f));
		//float y = (float) ((int)((optimalLobeAxis[1] * 0.5f + 0.5f) * 255.0f));
		//float z = (float) ((int)((optimalLobeAxis[2] * 0.5f + 0.5f) * 255.0f));
		//float compressedAxis = (x / 128.0f) + (y / (128.0f * 256.0f)) + (z / (128.0f * 256.0f * 256.0f));

		//float curUncompressedVal = (compressedAxis * 128.0f);
		//float curUncompressedX = ((int)curUncompressedVal) / 255.0f;
		//curUncompressedVal = (curUncompressedVal - ((int)curUncompressedVal)) * 256.0f;
		//float curUncompressedY = ((int)curUncompressedVal) / 255.0f;
		//curUncompressedVal = (curUncompressedVal - ((int)curUncompressedVal)) * 256.0f;
		//float curUncompressedZ = ((int)curUncompressedVal) / 255.0f;

		//optimalLobeAxis[0] = curUncompressedX * 2.0f - 1.0f;
		//optimalLobeAxis[1] = curUncompressedY * 2.0f - 1.0f;
		//optimalLobeAxis[2] = curUncompressedZ * 2.0f - 1.0f;
		
		float zh[NUM_SHPRT_BANDS];
		CompressSHToZH(data.sh, zh, NUM_SHPRT_BANDS, optimalLobeAxis);
		
		byte zhCompressed[NUM_SHPRT_BANDS];
		_LOOPj(NUM_SHPRT_BANDS)
		{
			float rangeVal = (zh[j] - ZH_BIAS[j]) / ZH_SCALE[j];
			_DEBUG_ASSERT(rangeVal >= 0.0f && rangeVal <= 1.0f);

			zhCompressed[j] = (byte) (rangeVal * 255.0f);
		}

		//pLine1[0] = zh[2];
		//pLine1[1] = zh[1];
		//pLine1[2] = zh[0];
		//pLine1[3] = optimalLobeAxis[1];

		byte y = (byte)((optimalLobeAxis[1] * 0.5f + 0.5f) * 255.0f);
		
		pLine1[0] = zhCompressed[2];
		pLine1[1] = zhCompressed[1];
		pLine1[2] = zhCompressed[0];
		pLine1[3] = y;

		byte x = (byte)((optimalLobeAxis[0] * 0.5f + 0.5f) * 255.0f);
		byte z = (byte)((optimalLobeAxis[2] * 0.5f + 0.5f) * 255.0f);
		pLine2[0] = x;
		pLine2[1] = z;
	}

	pTex1->Unlock(0);
	pTex2->Unlock(0);

	*ppTex1 = pTex1;
	*ppTex2 = pTex2;
}

IRVertexBuffer* CSHPRTCompute::CreateTransferVB(const SHPRTDataEntry* pDataEntryR, const SHPRTDataEntry* pDataEntryG, const SHPRTDataEntry* pDataEntryB, uint numEntries)
{
	_DEBUG_COMPILE_ASSERT(NUM_SHPRT_BANDS == 3);

	RVertexDesc desc[5];
	desc[0].usage = VDU_TEXCOORDF4;
	desc[0].usageIndex = 4;
	desc[1].usage = VDU_TEXCOORDF4;
	desc[1].usageIndex = 5;
	desc[2].usage = VDU_TEXCOORDF4;
	desc[2].usageIndex = 6;
	desc[3].usage = VDU_TEXCOORDF3;
	desc[3].usageIndex = 7;
	desc[4].usage = VDU_END;
	desc[4].usageIndex = 0;

	const static SHPRTDataEntry BIAS_DATA = { 0.0f, -2.0f, -2.0f, -2.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f };
	const static SHPRTDataEntry RANGE_DATA = { 2.0f, 4.0f, 4.0f, 4.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };

	IRVertexBuffer* pVB = m_pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, desc, numEntries, BUF_DEFAULT);
	float* pDest = (float*) pVB->Lock(0, numEntries);

	_LOOPi(numEntries)
	{
		float compressedSH[4];

		// Red
		_LOOPj(4)
		{
			float val1 = (pDataEntryR[i].sh[j] - BIAS_DATA.sh[j]) / RANGE_DATA.sh[j];
			float val2 = (pDataEntryR[i].sh[j+4] - BIAS_DATA.sh[j+4]) / RANGE_DATA.sh[j+4];

			_DEBUG_ASSERT(val1 >= 0.0f && val1 <= 1.0f);
			_DEBUG_ASSERT(val2 >= 0.0f && val2 <= 1.0f);

			val1 = (float) ((int)(val1 * 4095.0f)); 			
			val1 = val1 / 2048.0f;

			val2 = (float) ((int)(val2 * 4095.0f)); 			
			val2 = val2 / (2048.0f * 4096.0f);

			compressedSH[j] = val1 + val2;
		}

		//// Test uncompress
		//float uncompressedSHTest[8];
		//
		//_LOOPj(4)
		//{
		//	float uncompressVal = (compressedSH[j] * 2048.0f);
		//	float unVal1 = ((int) uncompressVal) / 4095.0f;
		//	uncompressVal = (uncompressVal - ((int)uncompressVal)) * 4096.0f;
		//	float unVal2 = ((int) uncompressVal) / 4095.0f;

		//	unVal1 = (unVal1 * RANGE_DATA.sh[j]) + BIAS_DATA.sh[j];
		//	unVal2 = (unVal2 * RANGE_DATA.sh[j+4]) + BIAS_DATA.sh[j+4];
		//	uncompressedSHTest[j] = unVal1;
		//	uncompressedSHTest[j+4] = unVal2;
		//}

		float* pRed = pDest;
		pRed[0] = compressedSH[0];
		pRed[1] = compressedSH[1];
		pRed[2] = compressedSH[2];		
		pRed[3] = compressedSH[3];		
		pDest += 4;

		// Green
		_LOOPj(4)
		{
			float val1 = (pDataEntryG[i].sh[j] - BIAS_DATA.sh[j]) / RANGE_DATA.sh[j];
			float val2 = (pDataEntryG[i].sh[j+4] - BIAS_DATA.sh[j+4]) / RANGE_DATA.sh[j+4];

			_DEBUG_ASSERT(val1 >= 0.0f && val1 <= 1.0f);
			_DEBUG_ASSERT(val2 >= 0.0f && val2 <= 1.0f);

			val1 = (float) ((int)(val1 * 4095.0f)); 			
			val1 = val1 / 2048.0f;

			val2 = (float) ((int)(val2 * 4095.0f)); 			
			val2 = val2 / (2048.0f * 4096.0f);

			compressedSH[j] = val1 + val2;
		}

		float* pGreen = pDest;
		pGreen[0] = compressedSH[0];
		pGreen[1] = compressedSH[1];
		pGreen[2] = compressedSH[2];
		pGreen[3] = compressedSH[3];
		pDest += 4;

		// Blue
		_LOOPj(4)
		{
			float val1 = (pDataEntryB[i].sh[j] - BIAS_DATA.sh[j]) / RANGE_DATA.sh[j];
			float val2 = (pDataEntryB[i].sh[j+4] - BIAS_DATA.sh[j+4]) / RANGE_DATA.sh[j+4];

			_DEBUG_ASSERT(val1 >= 0.0f && val1 <= 1.0f);
			_DEBUG_ASSERT(val2 >= 0.0f && val2 <= 1.0f);

			val1 = (float) ((int)(val1 * 4095.0f)); 			
			val1 = val1 / 2048.0f;

			val2 = (float) ((int)(val2 * 4095.0f)); 			
			val2 = val2 / (2048.0f * 4096.0f);

			compressedSH[j] = val1 + val2;
		}

		float* pBlue = pDest;
		pBlue[0] = compressedSH[0];
		pBlue[1] = compressedSH[1];
		pBlue[2] = compressedSH[2];
		pBlue[3] = compressedSH[3];
		pDest += 4;

		// Scalars
		float* pScalar = pDest;
		pScalar[0] = pDataEntryR[i].sh[8];
		pScalar[1] = pDataEntryG[i].sh[8];
		pScalar[2] = pDataEntryB[i].sh[8];
		pDest += 3;

	}

	pVB->Unlock();

	return pVB;
}

//static SHPRTDataEntry g_DataTestMin = {99999990.0f, 99999990.0f, 99999990.0f, 99999990.0f, 99999990.0f, 99999990.0f, 99999990.0f, 99999990.0f, 99999990.0f};
//static SHPRTDataEntry g_DataTestMax = {-99999990.0f, -99999990.0f, -99999990.0f, -99999990.0f, -99999990.0f, -99999990.0f, -99999990.0f, -99999990.0f, -99999990.0f};

IRVertexBuffer* CSHPRTCompute::CreateTransferVBZH(const SHPRTDataEntry* pDataEntryR, const SHPRTDataEntry* pDataEntryG, const SHPRTDataEntry* pDataEntryB, uint numEntries)
{
	_DEBUG_COMPILE_ASSERT(NUM_SHPRT_BANDS == 3);

	RVertexDesc desc[4];
	desc[0].usage = VDU_TEXCOORDF4;
	desc[0].usageIndex = 5;
	desc[1].usage = VDU_TEXCOORDF4;
	desc[1].usageIndex = 6;
	desc[2].usage = VDU_TEXCOORDF4;
	desc[2].usageIndex = 7;
	desc[3].usage = VDU_END;
	desc[3].usageIndex = 0;
	//desc[4].usage = VDU_END;
	//desc[4].usageIndex = 0;

	IRVertexBuffer* pVB = m_pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, desc, numEntries, BUF_DEFAULT);
	float* pDest = (float*) pVB->Lock(0, numEntries);
	
	_LOOPi(numEntries)
	{
		float zh[3];
		
		//_LOOPj(NUM_SHPRT_BANDS*NUM_SHPRT_BANDS)
		//{
		//	if(g_DataTestMin.sh[j] > pDataEntryR[i].sh[j])
		//		g_DataTestMin.sh[j] = pDataEntryR[i].sh[j];

		//	if(g_DataTestMax.sh[j] < pDataEntryR[i].sh[j])
		//		g_DataTestMax.sh[j] = pDataEntryR[i].sh[j];

		//	if(g_DataTestMin.sh[j] > pDataEntryG[i].sh[j])
		//		g_DataTestMin.sh[j] = pDataEntryG[i].sh[j];

		//	if(g_DataTestMax.sh[j] < pDataEntryG[i].sh[j])
		//		g_DataTestMax.sh[j] = pDataEntryG[i].sh[j];

		//	if(g_DataTestMin.sh[j] > pDataEntryB[i].sh[j])
		//		g_DataTestMin.sh[j] = pDataEntryB[i].sh[j];

		//	if(g_DataTestMax.sh[j] < pDataEntryB[i].sh[j])
		//		g_DataTestMax.sh[j] = pDataEntryB[i].sh[j];

		//}
		//if(pLocEntries[i].normal[1] < -0.99f)
		//{
		//	__debugbreak();
		//}

		// Red
		gmtl::Vec3f optimalLobeRed;
		optimalLobeRed[0] = 0.0f;
		optimalLobeRed[1] = 1.0f;
		optimalLobeRed[2] = 0.0f;
		EvaluateOptimalLinearZH(pDataEntryR[i].sh, optimalLobeRed);

		// Compress axis to 1 float
		float x = (float) ((int)((optimalLobeRed[0] * 0.5f + 0.5f) * 255.0f));
		float y = (float) ((int)((optimalLobeRed[1] * 0.5f + 0.5f) * 255.0f));
		float z = (float) ((int)((optimalLobeRed[2] * 0.5f + 0.5f) * 255.0f));
		float compressedAxisRed = (x / 128.0f) + (y / (128.0f * 256.0f)) + (z / (128.0f * 256.0f * 256.0f));

		float curUncompressedVal = (compressedAxisRed * 128.0f);
		float curUncompressedX = ((int)curUncompressedVal) / 255.0f;
		curUncompressedVal = (curUncompressedVal - ((int)curUncompressedVal)) * 256.0f;
		float curUncompressedY = ((int)curUncompressedVal) / 255.0f;
		curUncompressedVal = (curUncompressedVal - ((int)curUncompressedVal)) * 256.0f;
		float curUncompressedZ = ((int)curUncompressedVal) / 255.0f;

		optimalLobeRed[0] = curUncompressedX * 2.0f - 1.0f;
		optimalLobeRed[1] = curUncompressedY * 2.0f - 1.0f;
		optimalLobeRed[2] = curUncompressedZ * 2.0f - 1.0f;
		gmtl::normalize(optimalLobeRed);

		CompressSHToZH(pDataEntryR[i].sh, zh, NUM_SHPRT_BANDS, optimalLobeRed);

		//float sh[9];
		//ExpandZHToSH(zh, sh, NUM_SHPRT_BANDS, optimalLobeRed);

		float* pRed = pDest;
		pRed[0] = zh[0];
		pRed[1] = zh[1];
		pRed[2] = zh[2];		
		pDest += 4;

		// Green
		gmtl::Vec3f optimalLobeGreen;
		optimalLobeGreen[0] = 0.0f;
		optimalLobeGreen[1] = 1.0f;
		optimalLobeGreen[2] = 0.0f;
		EvaluateOptimalLinearZH(pDataEntryG[i].sh, optimalLobeGreen);

		// Compress axis to 1 float
		x = (float) ((int)((optimalLobeGreen[0] * 0.5f + 0.5f) * 255.0f));
		y = (float) ((int)((optimalLobeGreen[1] * 0.5f + 0.5f) * 255.0f));
		z = (float) ((int)((optimalLobeGreen[2] * 0.5f + 0.5f) * 255.0f));
		float compressedAxisGreen = (x / 128.0f) + (y / (128.0f * 256.0f)) + (z / (128.0f * 256.0f * 256.0f));

		curUncompressedVal = (compressedAxisGreen * 128.0f);
		curUncompressedX = ((int)curUncompressedVal) / 255.0f;
		curUncompressedVal = (curUncompressedVal - ((int)curUncompressedVal)) * 256.0f;
		curUncompressedY = ((int)curUncompressedVal) / 255.0f;
		curUncompressedVal = (curUncompressedVal - ((int)curUncompressedVal)) * 256.0f;
		curUncompressedZ = ((int)curUncompressedVal) / 255.0f;

		optimalLobeGreen[0] = curUncompressedX * 2.0f - 1.0f;
		optimalLobeGreen[1] = curUncompressedY * 2.0f - 1.0f;
		optimalLobeGreen[2] = curUncompressedZ * 2.0f - 1.0f;
		gmtl::normalize(optimalLobeGreen);

		CompressSHToZH(pDataEntryG[i].sh, zh, NUM_SHPRT_BANDS, optimalLobeGreen);
		float* pGreen = pDest;
		pGreen[0] = zh[0];
		pGreen[1] = zh[1];
		pGreen[2] = zh[2];
		pDest += 4;

		// Blue
		gmtl::Vec3f optimalLobeBlue;
		optimalLobeBlue[0] = 0.0f;
		optimalLobeBlue[1] = 1.0f;
		optimalLobeBlue[2] = 0.0f;
		EvaluateOptimalLinearZH(pDataEntryB[i].sh, optimalLobeBlue);

		// Compress axis to 1 float
		x = (float) ((int)((optimalLobeBlue[0] * 0.5f + 0.5f) * 255.0f));
		y = (float) ((int)((optimalLobeBlue[1] * 0.5f + 0.5f) * 255.0f));
		z = (float) ((int)((optimalLobeBlue[2] * 0.5f + 0.5f) * 255.0f));
		float compressedAxisBlue = (x / 128.0f) + (y / (128.0f * 256.0f)) + (z / (128.0f * 256.0f * 256.0f));

		curUncompressedVal = (compressedAxisBlue * 128.0f);
		curUncompressedX = ((int)curUncompressedVal) / 255.0f;
		curUncompressedVal = (curUncompressedVal - ((int)curUncompressedVal)) * 256.0f;
		curUncompressedY = ((int)curUncompressedVal) / 255.0f;
		curUncompressedVal = (curUncompressedVal - ((int)curUncompressedVal)) * 256.0f;
		curUncompressedZ = ((int)curUncompressedVal) / 255.0f;

		optimalLobeBlue[0] = curUncompressedX * 2.0f - 1.0f;
		optimalLobeBlue[1] = curUncompressedY * 2.0f - 1.0f;
		optimalLobeBlue[2] = curUncompressedZ * 2.0f - 1.0f;
		gmtl::normalize(optimalLobeBlue);

		CompressSHToZH(pDataEntryB[i].sh, zh, NUM_SHPRT_BANDS, optimalLobeBlue);
		float* pBlue = pDest;
		pBlue[0] = zh[0];
		pBlue[1] = zh[1];
		pBlue[2] = zh[2];
		pDest += 4;

		// Axis
		pRed[3] = compressedAxisRed;
		pGreen[3] = compressedAxisGreen;
		pBlue[3] = compressedAxisBlue;

		//pRed[3] = optimalLobeRed[0];
		//pGreen[3] = optimalLobeRed[1];
		//pBlue[3] = optimalLobeRed[2];

		//_DEBUG_ASSERT(optimalLobeGreen[1] >= 0.0f);
		//_DEBUG_ASSERT(optimalLobeBlue[1] >= 0.0f);

		//float* pAxis = pDest;
		//pAxis[0] = optimalLobeGreen[0];
		//pAxis[1] = optimalLobeGreen[2];
		//pAxis[2] = optimalLobeBlue[0];
		//pAxis[3] = optimalLobeBlue[2];
		//pDest += 4;

	}

	pVB->Unlock();

	return pVB;
}

void CSHPRTCompute::FilterVertexTransfer(const SHPRTLocationEntry* pLocEntries, IByteBuffer* pAdjacencyInfo, IByteBuffer* pNumAdjacency, 
										 SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dotThreshold)
{
	//const float COLOR_THRESHOLD = 0.0f;
	float dotThresholdRCP = 1.0f / dotThreshold;
	
	//const float COLOR_APROX_SH_MAX_RCP = 1.0f / (3.5449274f * 3.5449274f * 0.5f);
	
	SHPRTDataEntry* pTempR = _NEW SHPRTDataEntry[numEntries];
	SHPRTDataEntry* pTempG = _NEW SHPRTDataEntry[numEntries];
	SHPRTDataEntry* pTempB = _NEW SHPRTDataEntry[numEntries];
	float* pContrib = _NEW float[numEntries];

	_LOOPi(numEntries)
	{
		pTempR[i] = pDataEntriesR[i];
		pTempG[i] = pDataEntriesG[i];
		pTempB[i] = pDataEntriesB[i];
		pContrib[i] = 1.0f;
	}

	_DEBUG_ASSERT(pNumAdjacency->GetDataLength() == sizeof(uint) * numEntries);
	
	uint* pNumAdjacencyData = (uint*) pNumAdjacency->GetData();
	uint* pAdjacencyData = (uint*) pAdjacencyInfo->GetData();

	_LOOPi(numEntries)
	{
		const SHPRTLocationEntry& srcEntry = pLocEntries[i];
		const SHPRTDataEntry& srcDataR = pDataEntriesR[i];
		const SHPRTDataEntry& srcDataG = pDataEntriesG[i];
		const SHPRTDataEntry& srcDataB = pDataEntriesB[i];

		// Determine the average color (using 1st band of SH)
		//float srcColorR = srcDataR.sh[0] * 3.5449274f;
		//float srcColorG = srcDataG.sh[0] * 3.5449274f;
		//float srcColorB = srcDataB.sh[0] * 3.5449274f;

		//_LOOPj(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
		//{
		//	srcColorR += scrDataR[j] * unitSH[j];
		//	srcColorG += scrDataG[j] * unitSH[j];
		//	srcColorB += scrDataB[j] * unitSH[j];

		//}

		uint numAdjacencies = pNumAdjacencyData[i];
		_LOOPj(numAdjacencies)
		{
			uint vertIndex = pAdjacencyData[j];
			const SHPRTLocationEntry& otherEntry = pLocEntries[vertIndex];

			// First determine if both points lie on the same surface
			gmtl::VecA3f srcToOther = srcEntry.pos - otherEntry.pos;
			float dist;
			VecVecDot(&dist, &srcToOther, &srcToOther);
			//VecLength(&dist, &srcToOther);
			//NormalizeVecLength(&dist, &srcToOther, &srcToOther);
			
			float srcDot;
			float otherDot;
			float normalSimilarity;
			if(dist < 0.000001f)
			{
				srcDot = 0.0f;
				otherDot = 0.0f;
			}
			else
			{
				NormalizeVec(srcToOther);
				VecVecDot(&srcDot, &srcEntry.normal, &srcToOther);
				VecVecDot(&otherDot, &otherEntry.normal, &srcToOther);
			}

			VecVecDot(&normalSimilarity, &srcEntry.normal, &otherEntry.normal);

			float maxDot = abs(srcDot) > abs(otherDot) ? abs(srcDot) : abs(otherDot);
			float similarSurfaceAmount = ((normalSimilarity * (1.0f - maxDot)) - (1.0f - dotThreshold)) * dotThresholdRCP;
			//if(maxDot <= dotThreshold)
			{
				float contrib = similarSurfaceAmount;
				contrib = contrib < 0.0f ? 0.0f : contrib;
				contrib = contrib > 1.0f ? 1.0f : contrib;
				//float contrib = 1.0f - (maxDot * dotThresholdRCP);

				const SHPRTDataEntry& otherDataR = pDataEntriesR[vertIndex];
				const SHPRTDataEntry& otherDataG = pDataEntriesG[vertIndex];
				const SHPRTDataEntry& otherDataB = pDataEntriesB[vertIndex];

				//float otherColorR = otherDataR.sh[0] * 3.5449274f;
				//float otherColorG = otherDataG.sh[0] * 3.5449274f;
				//float otherColorB = otherDataB.sh[0] * 3.5449274f;

				//if( (abs(srcColorR - otherColorR) * COLOR_APROX_SH_MAX_RCP) > COLOR_THRESHOLD ||
				//	(abs(srcColorG - otherColorG) * COLOR_APROX_SH_MAX_RCP) > COLOR_THRESHOLD ||
				//	(abs(srcColorB - otherColorB) * COLOR_APROX_SH_MAX_RCP) > COLOR_THRESHOLD )
				{
					// So we need to filter, then add contribution of this sh
					_LOOPk(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
					{
						pTempR[i].sh[k] += otherDataR.sh[k] * contrib;
						pTempG[i].sh[k] += otherDataG.sh[k] * contrib;
						pTempB[i].sh[k] += otherDataB.sh[k] * contrib;
					}

					pContrib[i] += contrib;
				}

			}
		}

		pAdjacencyData += numAdjacencies;
	}

	_LOOPi(numEntries)
	{
		float rcp = 1.0f / pContrib[i];
		
		_LOOPj(NUM_SHPRT_BANDS * NUM_SHPRT_BANDS)
		{
			pTempR[i].sh[j] *= rcp;
			pTempG[i].sh[j] *= rcp;
			pTempB[i].sh[j] *= rcp;
		}

		pDataEntriesR[i] = pTempR[i];
		pDataEntriesG[i] = pTempG[i];
		pDataEntriesB[i] = pTempB[i];
	}

	_DELETE_ARRAY(pContrib);
	_DELETE_ARRAY(pTempR);
	_DELETE_ARRAY(pTempG);
	_DELETE_ARRAY(pTempB);
}

_NAMESPACE_END