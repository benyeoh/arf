//================================================================================
//
//		SHPRTComputeOcclCubeMap.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/22/2012
//
//================================================================================

#include "SHPRTComputeFramework.h"

_NAMESPACE_BEGIN

//void SHPRTComputeOcclCubeMap::ComputeOccludedTransferSphere(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, 
//															SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, 
//															ISHPRTComputeOccludedCallback& occlCallback,
//															IPThreadPool* pThreadPool)
//{
//	// TODO: Should let the app allocate mem
//	ObjectPoolAtomic<CompressOcclTexDualToSHJob, 4> compressJobPool;
//	ObjectPoolAtomic<CompressOcclTexDualToSHJob, 4>* pCompressJobPool = &compressJobPool;
//
//	int readBackRTIndex = -((int)NUM_LATENCY_BUFFERS);
//
//	uint curDataEntry = 0;
//	gmtl::MatrixA44f paraViewStore[NUM_LATENCY_BUFFERS];
//	gmtl::MatrixA44f paraViewStoreDual[NUM_LATENCY_BUFFERS];
//
//	_LOOPi(numEntries)
//	{		
//		if(m_pRenderer->BeginRender())
//		{
//			readBackRTIndex++;
//
//			// For each location entry, render the scene and set
//			// the appropriate render states
//
//			BFXRenderContainer renderContainer;
//			BFXParamContainer paramContainer;
//			renderContainer.SetRenderCallback((IBFXRenderCallback*) &m_Pipeline, SHPRTCOMPUTE_LIB_ID);
//			paramContainer.SetParamCallback((IBFXParamCallback*) &m_ParamPool, SHPRTCOMPUTE_LIB_ID);
//
//			// Derive and set the paraboloid view matrix for the current location
//			gmtl::MatrixA44f paraView;
//			gmtl::VecA3f x(1.0f, 0.0f, 0.0f);
//
//			// Singularity check
//			if( pLocEntries[i].normal[0] > 0.99f )
//			{
//				x[0] = 0.0f;
//				x[1] = -1.0f;
//				x[2] = 0.0f;
//			}
//			else if( pLocEntries[i].normal[0] < -0.99f )
//			{
//				x[0] = 0.0f;
//				x[1] = 1.0f;
//				x[2] = 0.0f;
//			}
//
//			gmtl::VecA3f z;
//			gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal));
//			gmtl::normalize(_CAST_VEC3(z));
//
//			gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal), _CAST_VEC3(z));
//			gmtl::normalize(_CAST_VEC3(x));
//
//			gmtl::setViewWithAxis(paraView, pLocEntries[i].pos, x, pLocEntries[i].normal, z); 
//
//			m_ParamPool.SetParaboloidViewMatrix(&_CAST_MAT44(paraView));
//
//			// Then set the current render target
//			int curRTIndex = i % NUM_LATENCY_BUFFERS;
//			m_Pipeline.SetOcclusionRT(curRTIndex);
//
//			// Begin app callback for render pipeline
//			occlCallback.BeginRender(pLocEntries[i].pos, pLocEntries[i].normal, renderContainer, paramContainer);
//			{
//				// Flush our occlusion computation pipeline
//				m_Pipeline.Flush();
//			}
//			occlCallback.EndRender();
//
//			m_ParamPool.ResetParams();
//			paraViewStore[curRTIndex] = paraView;
//
//			// Render other hemisphere
//			gmtl::VecA3f flippedNormal = -pLocEntries[i].normal;
//			x = gmtl::VecA3f(1.0f, 0.0f, 0.0f);
//			if( flippedNormal[0] > 0.99f )
//			{
//				x[0] = 0.0f;
//				x[1] = -1.0f;
//				x[2] = 0.0f;
//			}
//			else if( flippedNormal[0] < -0.99f )
//			{
//				x[0] = 0.0f;
//				x[1] = 1.0f;
//				x[2] = 0.0f;
//			}
//			gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(flippedNormal));
//			gmtl::normalize(_CAST_VEC3(z));
//
//			gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(flippedNormal), _CAST_VEC3(z));
//			gmtl::normalize(_CAST_VEC3(x));
//
//			gmtl::setViewWithAxis(paraView, pLocEntries[i].pos, x, flippedNormal, z); 
//
//			m_ParamPool.SetParaboloidViewMatrix(&_CAST_MAT44(paraView));
//
//			// Then set the current render target
//			m_Pipeline.SetOcclusionRTDual(curRTIndex);
//
//			// Begin app callback for render pipeline
//			occlCallback.BeginRender(pLocEntries[i].pos, flippedNormal, renderContainer, paramContainer);
//			{
//				// Flush our occlusion computation pipeline
//				m_Pipeline.Flush();
//			}
//			occlCallback.EndRender();
//
//			m_ParamPool.ResetParams();
//			m_pRenderer->EndRender();
//
//			paraViewStoreDual[curRTIndex] = paraView;
//
//			IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(curRTIndex);
//			pOcclusionRT->LockReadOnlyPrepare();
//			IRTextureRT* pOcclusionRTDual = m_Pipeline.GetOcclusionRTDual(curRTIndex);
//			pOcclusionRTDual->LockReadOnlyPrepare();
//
//			// Read back from GPU and perform integral
//			if(readBackRTIndex >= 0)
//			{
//				readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;
//				pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex);
//				pOcclusionRTDual = m_Pipeline.GetOcclusionRTDual(readBackRTIndex);
//				//IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex);
//				//pOcclusionRT->LockReadOnlyPrepare();
//				//IRTextureRT* pOcclusionRTDual = m_Pipeline.GetOcclusionRTDual(readBackRTIndex);
//				//pOcclusionRTDual->LockReadOnlyPrepare();
//
//				uint pitch;
//				byte* pToRead = pOcclusionRT->LockReadOnly(pitch);
//				uint pitchDual;
//				byte* pToReadDual = pOcclusionRTDual->LockReadOnly(pitchDual);
//
//				if(pThreadPool)
//				{
//					CompressOcclTexDualToSHJob* pJob = pCompressJobPool->Get();
//					if(!pJob)
//					{
//						pThreadPool->ProcessJob();
//						pJob = pCompressJobPool->Get();
//						_DEBUG_ASSERT(pJob);
//					}
//
//					memcpy(pJob->texData, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
//					memcpy(pJob->texDataDual, pToReadDual, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
//					pJob->pOwner = this;
//					pJob->pDataR = &pDataEntriesR[curDataEntry];
//					pJob->pDataG = &pDataEntriesG[curDataEntry];
//					pJob->pDataB = &pDataEntriesB[curDataEntry];
//					pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
//					pJob->paraViewDual = _CAST_MAT44(paraViewStoreDual[readBackRTIndex]);
//					pJob->pOwnerPool = pCompressJobPool;
//
//					pThreadPool->QueueJob(*pJob);
//				}
//				else
//				{
//					// Integrate occlusion amount from texture and store in the PRT data entry
//					CompressOcclTexDataDualToSH((uint*) pToRead, (uint*) pToReadDual, &pDataEntriesR[curDataEntry], &pDataEntriesG[curDataEntry], &pDataEntriesB[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]), _CAST_MAT44(paraViewStoreDual[readBackRTIndex]));
//				}
//
//				pOcclusionRT->UnlockReadOnly();
//				pOcclusionRTDual->UnlockReadOnly();
//
//				curDataEntry++;
//			}
//		}
//		else
//		{
//			i--;
//		}
//
//	}
//
//	_LOOPi(NUM_LATENCY_BUFFERS-1)
//	{
//		readBackRTIndex++;
//
//		if(readBackRTIndex >= 0)
//		{
//			readBackRTIndex = readBackRTIndex % NUM_LATENCY_BUFFERS;
//			IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex);
//			//pOcclusionRT->LockReadOnlyPrepare();
//			IRTextureRT* pOcclusionRTDual = m_Pipeline.GetOcclusionRTDual(readBackRTIndex);
//			//pOcclusionRTDual->LockReadOnlyPrepare();
//
//			uint pitch;
//			byte* pToRead = pOcclusionRT->LockReadOnly(pitch);
//			uint pitchDual;
//			byte* pToReadDual = pOcclusionRTDual->LockReadOnly(pitchDual);
//
//			if(pThreadPool)
//			{
//				CompressOcclTexDualToSHJob* pJob = pCompressJobPool->Get();
//				if(!pJob)
//				{
//					pThreadPool->ProcessJob();
//					pJob = pCompressJobPool->Get();
//					_DEBUG_ASSERT(pJob);
//				}
//
//				memcpy(pJob->texData, pToRead, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
//				memcpy(pJob->texDataDual, pToReadDual, sizeof(uint) * SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE);
//				pJob->pOwner = this;
//				pJob->pDataR = &pDataEntriesR[curDataEntry];
//				pJob->pDataG = &pDataEntriesG[curDataEntry];
//				pJob->pDataB = &pDataEntriesB[curDataEntry];
//				pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
//				pJob->paraViewDual = _CAST_MAT44(paraViewStoreDual[readBackRTIndex]);
//				pJob->pOwnerPool = pCompressJobPool;
//
//				pThreadPool->QueueJob(*pJob);
//			}
//			else
//			{
//				// Integrate occlusion amount from texture and store in the PRT data entry
//				CompressOcclTexDataDualToSH((uint*) pToRead, (uint*) pToReadDual, &pDataEntriesR[curDataEntry], &pDataEntriesG[curDataEntry], &pDataEntriesB[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]), _CAST_MAT44(paraViewStoreDual[readBackRTIndex]));
//			}
//
//			pOcclusionRT->UnlockReadOnly();
//			pOcclusionRTDual->UnlockReadOnly();
//
//			curDataEntry++;
//		}
//	}
//
//	if(pThreadPool)
//	{
//		// Finish processing pending jobs if any
//		while(pThreadPool->GetNumJobsPending() > 0)
//		{
//			pThreadPool->ProcessJob();
//		}
//
//		//_DELETE(pCompressJobPool);
//	}
//}

_NAMESPACE_END