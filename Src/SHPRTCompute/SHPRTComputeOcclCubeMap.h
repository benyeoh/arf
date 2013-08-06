//================================================================================
//
//		SHPRTComputeOcclCubeMap.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/22/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class SHPRTComputeOcclCubeMap
{
private:
	CSHPRTCompute* m_pCompute;

public:
	SHPRTComputeOcclCubeMap()
	{
	}

	~SHPRTComputeOcclCubeMap()
	{
	}

public:
	void SetCompute(SHPRTCompute* pCompute) { m_pCompute = pCompute; }

	void ComputeOccludedTransferSphere(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, ISHPRTComputeOccludedCallback& occlCallback, IPThreadPool* pThreadPool);
};

_NAMESPACE_END