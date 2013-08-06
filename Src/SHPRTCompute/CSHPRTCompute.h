//================================================================================
//
//		CSHPRTCompute.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/9/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CSHPRTCompute: public CRefCounted<ISHPRTCompute>
{
private:
	IRRendererPtr m_pRenderer;
	//IBFXBaseFXPtr m_pBaseFX;

	IAppCallback* m_pAppCallback;

	CSHPRTComputePipeline m_Pipeline;
	CSHPRTComputeParamPool m_ParamPool;

public:
	CSHPRTCompute()
		: m_pAppCallback(NULL)
	{
	}

	virtual ~CSHPRTCompute()
	{
	}

private:
//	void ComputePushPullForTexture(SHPRTDataEntry* pTexDestData, uint texWidth, uint texHeight, const SHPRTLocationEntry* pLocEntry, const SHPRTDataEntry* pSrcData, uint numEntries);
	//void CompressOcclTexDataToSH(uint* pParaTexData, SHPRTDataEntry* pDataEntryRDest, SHPRTDataEntry* pDataEntryGDest, SHPRTDataEntry* pDataEntryBDest, const gmtl::Matrix44f& paraView);

//protected:
//	_PURE( uint DoComputeUVLocEntries(SHPRTLocationEntry* pLocEntries, const gmtl::VecA4f& v1, const gmtl::VecA4f& v2, const gmtl::VecA4f& v3, 
//									  const float* pV1Inter, const float* pV2Inter, const float* pV3Inter, uint texWidth, uint texHeight) )
//
public:
	boolean Initialize(IRRenderer& renderer, IAppCallback& callback);

	//uint AddContext();
	//uint GetNumOfContexts();
	//void ResetContext(uint contextID);

	//uint ComputeUVLocEntries(SHPRTLocationEntry* pLocEntries, const gmtl::Matrix44f& worldTrans, 
	//						void* pPos, uint posStride, void* pNormal, uint normalStride,
	//						void* pUV, uint uvStride, ushort* pIndices, uint numTri, uint texWidth, uint texHeight);

	void CompressOcclTexDataToSH(uint* pParaTexData, SHPRTDataEntry* pDataEntryRDest, SHPRTDataEntry* pDataEntryGDest, SHPRTDataEntry* pDataEntryBDest, const gmtl::Matrix44f& paraView);
	void CompressOcclTexDataDualToSH(uint* pParaTexData, uint* pParaTexDataDual, SHPRTDataEntry* pDataEntryRDest, SHPRTDataEntry* pDataEntryGDest, SHPRTDataEntry* pDataEntryBDest, const gmtl::Matrix44f& paraView, const gmtl::Matrix44f& paraViewDual, float dualSelfOcclScale);
	
	void ComputeVertexLocEntries(SHPRTLocationEntry* pLocEntries, const gmtl::MatrixA44f& worldTrans, 
								void* pPos, uint posStride, void* pNormal, uint normalStride,
								uint numVertices, float posBias);

	void ComputeBRDFLocationEntries(SHPRTLocationEntry* pLocEntries, uint texWidth, uint texHeight);

	void ZeroTransfer(SHPRTDataEntry* pDataEntry, uint numEntries);

	void ComputeOccludedTransfer(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, ISHPRTComputeOccludedCallback& occlCallback, IPThreadPool* pThreadPool);
	void ComputeOccludedTransferSphere(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dualSelfOcclScale, ISHPRTComputeOccludedCallback& occlCallback, IPThreadPool* pThreadPool);
	void ComputeOccludedTransferDual(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dualSelfOcclScale, ISHPRTComputeOccludedCallback& occlCallback, IPThreadPool* pThreadPool);
	IRTexture2D* ComputeOccludedTransferDebug(const SHPRTLocationEntry& locEntries, ISHPRTComputeOccludedCallback& occlCallback);

	void ComputeBRDFTransfer(const SHPRTLocationEntry* pLocEntry, SHPRTDataEntry* pDataEntries, uint numEntries, ISHPRTComputeBRDFCallback& brdfCallback);

	void FilterVertexTransfer(const SHPRTLocationEntry* pLocEntries, IByteBuffer* pAdjacencyInfo, IByteBuffer* pNumAdjacency, 
							  SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dotThreshold);

	void CreateBRDFTexturesZH(const SHPRTLocationEntry* pLocEntries, const SHPRTDataEntry* pDataEntries, uint numEntries, uint texWidth, uint texHeight, IRTexture2D** ppTex1, IRTexture2D** ppTex2);

	IRVertexBuffer* CreateTransferVB(const SHPRTDataEntry* pDataEntryR, const SHPRTDataEntry* pDataEntryG, const SHPRTDataEntry* pDataEntryB, uint numEntries);
	IRVertexBuffer* CreateTransferVBZH(const SHPRTDataEntry* pDataEntryR, const SHPRTDataEntry* pDataEntryG, const SHPRTDataEntry* pDataEntryB, uint numEntries);
	//IRTexture* CreateTransferTextureZH(const SHPRTLocationEntry* pLocEntries, const SHPRTDataEntry* pDataEntry, uint numEntries, uint texWidth, uint texHeight);
};

_NAMESPACE_END
