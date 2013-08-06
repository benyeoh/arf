//================================================================================
//
//		Interface.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/29/2012
//
//================================================================================

_NAMESPACE_BEGIN

struct SHPRTLocationEntry
{
	gmtl::VecA3f pos;
	gmtl::VecA3f normal;
	uint texX;
	uint texY;

	_IMPL_ALIGNED_ALLOCS(16)
};

struct SHPRTDataEntry
{
	float sh[NUM_SHPRT_BANDS*NUM_SHPRT_BANDS];
};

struct ISHPRTComputeBRDFCallback
{
	_PURE( void GetBRDF(const gmtl::VecA3f& viewDir, const gmtl::VecA3f* pSrcLightDirs, float* pDestAlbedo, uint numDirs) )
};

struct ISHPRTComputeOccludedCallback
{
	// During Render, the application should set up all necessary parameters to correctly render the object
	// IE, loading the correct material for SHPRTCompute, setting up parameters and render containers, rendering the object with any relevant material types etc...
	_PURE( void Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB) )
};

struct ISHPRTCompute : public IRefCounted
{
	_PURE( boolean Initialize(IRRenderer& renderer, IAppCallback& callback) )
	
	//_PURE( uint AddContext() )
	//_PURE( uint GetNumOfContexts() )
	//_PURE( void ResetContext(uint contextID) )

	_PURE( void ComputeVertexLocEntries(SHPRTLocationEntry* pLocEntries, const gmtl::MatrixA44f& worldTrans, 
										void* pPos, uint posStride, void* pNormal, uint normalStride,
										uint numVertices, float posBias) )
	
	_PURE( void ComputeBRDFLocationEntries(SHPRTLocationEntry* pLocEntries, uint texWidth, uint texHeight) )

	_PURE( void ZeroTransfer(SHPRTDataEntry* pDataEntry, uint numEntries) )

	_PURE( void ComputeOccludedTransfer(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, ISHPRTComputeOccludedCallback& occlCallback, IPThreadPool* pThreadPool) )
	_PURE( void ComputeOccludedTransferSphere(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dualSelfOcclScale, ISHPRTComputeOccludedCallback& occlCallback, IPThreadPool* pThreadPool) )
	_PURE( void ComputeOccludedTransferDual(const SHPRTLocationEntry* pLocEntries, SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dualSelfOcclScale, ISHPRTComputeOccludedCallback& occlCallback, IPThreadPool* pThreadPool) )
	_PURE( IRTexture2D* ComputeOccludedTransferDebug(const SHPRTLocationEntry& locEntries, ISHPRTComputeOccludedCallback& occlCallback) )

	_PURE( void ComputeBRDFTransfer(const SHPRTLocationEntry* pLocEntry, SHPRTDataEntry* pDataEntries, uint numEntries, ISHPRTComputeBRDFCallback& brdfCallback) )

	_PURE( void FilterVertexTransfer(const SHPRTLocationEntry* pLocEntries, IByteBuffer* pAdjacencyInfo, IByteBuffer* pNumAdjacency, 
									 SHPRTDataEntry* pDataEntriesR, SHPRTDataEntry* pDataEntriesG, SHPRTDataEntry* pDataEntriesB, uint numEntries, float dotThreshold) )

	_PURE( void CreateBRDFTexturesZH(const SHPRTLocationEntry* pLocEntries, const SHPRTDataEntry* pDataEntries, uint numEntries, uint texWidth, uint texHeight, IRTexture2D** ppTex1, IRTexture2D** ppTex2) )

	_PURE( IRVertexBuffer* CreateTransferVB(const SHPRTDataEntry* pDataEntryR, const SHPRTDataEntry* pDataEntryG, const SHPRTDataEntry* pDataEntryB, uint numEntries) )
	_PURE( IRVertexBuffer* CreateTransferVBZH(const SHPRTDataEntry* pDataEntryR, const SHPRTDataEntry* pDataEntryG, const SHPRTDataEntry* pDataEntryB, uint numEntries) )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(ISHPRTCompute)

_NAMESPACE_END


//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE ISHPRTCompute* (*CreateSHPRTComputeFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE ISHPRTCompute* CreateSHPRTCompute() )
}
