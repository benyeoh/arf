//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, February 07, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
//	
//	Renderer Structs
//
//////////////////////////////////////////////////////////////////////////

// Forward declarations
struct IAppCallback;
struct IRTexture2D;
struct IRVertexBuffer;
struct IRVertexBufferGroup;
struct IRIndexBuffer;
struct IRResourceMgr;
struct IREffect;
struct IREffectTemplate;
struct REffectParam;
struct RRenderOp;

const static int R_UNKNOWN = -1;

enum eRVDType
{
	VDT_FLOAT1	= 0x00000010,
	VDT_FLOAT2	= 0x00000020,
	VDT_FLOAT3	= 0x00000040,
	VDT_FLOAT4	= 0x00000080,
	VDT_SHORT2	= 0x00000100,
	VDT_SHORT4	= 0x00000200,
	VDT_COLOR	= 0x00000400,
	VDT_END		= 0x80000000,
};

// TODO: Refactor this
enum eRVDUsage
{	
	VDU_POSITION		= 0x00001000,
	VDU_NORMAL			= 0x00002000,
	VDU_TEXCOORDF		= 0x00004000,
	VDU_BINORMAL		= 0x00008000,
	VDU_TANGENT			= 0x00010000,
	VDU_BLENDINDICES	= 0x00020000,
	VDU_BLENDWEIGHT		= 0x00040000,
	VDU_COLOR			= 0x00080000,

	// Pseudonyms
	VDU_TEXCOORDF2		= 0x00004000,
	VDU_TEXCOORDF1		= 0x00004100,
	VDU_TEXCOORDF3		= 0x00004200,
	VDU_TEXCOORDF4		= 0x00004400,
	
	VDU_END				= 0x80000000,
};

enum eRTexFormat
{
	TEXF_A8R8G8B8,
	TEXF_X8R8G8B8,
	TEXF_R32F,
	TEXF_G16R16F,
	TEXF_DXT1,
	TEXF_DXT2,
	TEXF_DXT3,
	TEXF_DXT4,
	TEXF_DXT5,
	TEXF_A8,
	TEXF_A8R8,
	TEXF_A32B32G32R32F,
	TEXF_G32R32F,
	TEXF_A16B16G16R16F,
	TEXF_G16R16,
	TEXF_R8,
	TEXF_A2R10G10B10,
	TEXF_END,
};

enum eRDepthFormat
{
	DF_D32,
	DF_D24S8,
	DF_D16,
	DF_END,
};

enum eRTexUsage
{
	TEXU_DEFAULT,		
	TEXU_DYNAMIC,
	TEXU_DEFAULT_STAGING,
	TEXU_END,
};

enum eRResourceType
{
	RT_TEXTURE_2D = 0,
	RT_EFFECT,
	RT_EFFECT_TEMPLATE,
	RT_VERTEX_BUFFER,
	RT_INDEX_BUFFER,
	RT_DEPTH_BUFFER,
	RT_TEXTURE_RT,
	RT_TEXTURE_CUBE,
	RT_TEXTURE_CUBE_RT,
	RT_VERTEX_BUFFER_GROUP,
	RT_TEXTURE_3D,

	RT_UNKNOWN,
};

enum eRBufferUsage
{
	BUF_DEFAULT = 0,
	BUF_DYNAMIC,
	BUF_DEFAULT_STAGING,
};

enum eRPrimitiveType
{
	PRIM_TRIANGLE,
	PRIM_LINE,
};

typedef void* WinHandle;

// Common Renderer specific structs
struct RVarData
{
	union
	{
		void*	pVal;
		uint	iVal;
		float	fVal;
	};
};

struct RDisplayInfo
{
	WinHandle	hWnd;
	uint		backBufferWidth;
	uint		backBufferHeight;
	boolean		fullScreen;	
};

struct RInitInfo
{
	RDisplayInfo presentInfo;
	IAppCallback* pApplication;
};

struct RRendererInfo
{
	//int maxShaderModelVersion;
	float pixelOffset;
};

struct RMetrics
{
	uint numDrawCalls;
	uint numPrimitivesRendered;
	uint numVerticesRendered;
	uint numRenderGroups;
	uint numBeginPass;
	uint numBeginTechnique;
	uint numApplyConstantParam;
	uint numApplyDynamicParam;
};

struct RVertexDesc
{
	eRVDUsage usage;
	uint usageIndex;
	
	//RVertexDesc()
	//	: type(VDT_END)
	//	, usage(VDU_END)
	//	, usageIndex(0)
	//{
	//}
	//
	//RVertexDesc(eRVDType t, eRVDUsage u, uint i)
	//	: type(t)
	//	, usage(u)
	//	, usageIndex(i)
	//{
	//}
};

enum eREffectParamType 
{
	EPT_TEX2D_P = 0,
	EPT_VOID_P,
	EPT_MAT44_P,
	EPT_MAT33_P,
	EPT_MAT22_P,
	EPT_VEC4_P,
	EPT_VEC3_P,
	EPT_VEC2_P,
	EPT_FLOAT,
	EPT_INT,
	EPT_BOOL,
	EPT_STRING,
	EPT_TEXCUBE_P,
	
	EPT_END,
};

enum eRCubeFace
{
	CF_POS_X = 0,
	CF_NEG_X,
	CF_POS_Y,
	CF_NEG_Y,
	CF_POS_Z,
	CF_NEG_Z,
	
	CF_NUM_FACES,
};

// Parameters are RRenderOps
typedef int (*RenderOpSort)(const void*, const void*);

struct IRRenderOpCallback
{
	_PURE( void BeginRenderOp(const RRenderOp* pSortData) )
	_PURE( void EndRenderOp(const RRenderOp* pSortData) )
};

struct IRRenderOpAllocCallback
{
	_PURE( RRenderOp* AllocRenderOp() )
};

//struct IRRenderCallback
//{
//	//------------------------------------------------------------------------------
//	//	BeginRenderGroup()
//	//		Notifies the application that a group type is being rendered
//	//		Allows the application to perform any pre-render setup if needed
//	//------------------------------------------------------------------------------
//	_PURE( void BeginRenderGroup(uint groupNum) )
//
//	//------------------------------------------------------------------------------
//	//	EndRenderGroup()
//	//		Notifies the application that a group type has finished rendering
//	//		Allows the application to perform any cleanup if needed
//	//------------------------------------------------------------------------------
//	_PURE( void EndRenderGroup(uint groupNum) )
//
//	_PURE( void BeginRenderOp(const RRenderOp* pSortData) )
//
//	_PURE( void EndRenderOp(const RRenderOp* pSortData) )
//};

//////////////////////////////////////////////////////////////////////////
//	
//	Public Interfaces
//
//////////////////////////////////////////////////////////////////////////

struct IRResource : public IRefCounted
{
	_PURE( const wchar* GetResourceName() const )
	_PURE( const CRCDataKey& GetResourceID() const )
	_PURE( void OnLostDevice() )
	_PURE( void OnRestoreDevice() )
	_PURE( void OnResetDevice() )
};

// IRTexture
struct IRTexture : public IRResource
{
	_PURE( eRTexUsage GetUsage() const )
	_PURE( eRTexFormat GetFormat() const )
	_PURE( uint GetBitDepth() const )
	_PURE( uint GetMipMapLevels() const )
	_PURE( void SetFileName(const wchar* pName) )
	_PURE( const wchar* GetFileName() const )
};

struct IRTextureRT : public IRTexture
{
	_PURE( uint GetWidth() const )
	_PURE( uint GetHeight() const )

	_PURE( void CopyFromTextureRT(IRTextureRT* pRT) )

	_PURE( boolean LockReadOnlyPrepare() )
	_PURE( byte* LockReadOnly(uint& pitch) )
	_PURE( boolean UnlockReadOnly() )

		//_PURE( byte* LockWriteOnly(uint& pitch, const gmtl::Vec4i* pToWrite) )
		//_PURE( boolean UnlockWriteOnly() )
};

struct IRTextureCube : public IRTexture
{
	_PURE( uint GetSize(uint level) const )
	
	_PURE( byte* Lock(uint level, eRCubeFace face, uint& pitch, const gmtl::Vec4i* pToWrite) )
	_PURE( boolean Unlock(uint level, eRCubeFace face) )

	_PURE( IRTextureRT* GetTextureRT(uint level, eRCubeFace face) )
};

// IRTexture2D
struct IRTexture2D : public IRTexture
{
	_PURE( uint GetWidth(uint level) const )
	_PURE( uint GetHeight(uint level) const )

	_PURE( byte* Lock(uint level, uint& pitch, const gmtl::Vec4i* pToWrite) )
	_PURE( boolean Unlock(uint level) )	

	_PURE( byte* LockImmediate(uint level, uint& pitch) )
	_PURE( boolean UnlockImmediate(uint level) )

	_PURE( IRTextureRT* GetTextureRT(uint level) )
};

struct IRTexture3D : public IRTexture
{
	_PURE( uint GetWidth(uint level) const )
	_PURE( uint GetHeight(uint level) const )
	_PURE( uint GetDepth(uint level) const )

	_PURE( byte* Lock(uint level, uint& rowPitch, uint& slicePitch, const gmtl::Vec3i* pToWriteStart, const gmtl::Vec3i* pToWriteEnd) )
	_PURE( boolean Unlock(uint level) )	

	_PURE( byte* LockImmediate(uint level, uint& rowPitch, uint& slicePitch) )
	_PURE( boolean UnlockImmediate(uint level) )
};

struct IRDepthBuffer : public IRResource
{
	_PURE( uint GetWidth() const )
	_PURE( uint GetHeight() const )
	_PURE( eRDepthFormat GetDepthFormat() const )
	_PURE( uint GetMultiSampleQuality() const )
};

struct IRRenderGroup : public IRefCounted
{
	_PURE( void SetViewport(uint left, uint top, uint width, uint height) )

	_PURE( void AddRenderTarget(IRTextureRT* pRT) )
	_PURE( void ClearRenderTargets() )
	_PURE( void SetDepthBuffer(IRDepthBuffer* pDepth) )
	_PURE( void SetClearColor(const gmtl::Vec4f& color) )
	_PURE( void SetClearDepth(float val) )

	_PURE( uint GetNumOfRenderTargets() )
	_PURE( IRTextureRT* GetRenderTarget(uint index) )
	_PURE( IRDepthBuffer* GetDepthBuffer() );

	_PURE( void Flush() )
	_PURE( void Reset() )

	// NOTE: pParams must be filled with all dynamic parameters
	// followed by instanced parameters
	_PURE( void AddDrawOp(IRVertexBufferGroup* pVBGroup,
						IRIndexBuffer* pIB,
						IREffect* pEffect,
						REffectParam* pParams,
						uint techToUse,
						RVarData userData,
						eRPrimitiveType prim=PRIM_TRIANGLE) )

	_PURE( void FillDrawOp(RRenderOp* pDest,
						IRVertexBufferGroup* pVBGroup,
						IRIndexBuffer* pIB,
						IREffect* pEffect,
						REffectParam* pParams,
						uint techToUse,
						RVarData userData,
						eRPrimitiveType prim=PRIM_TRIANGLE) )

	_PURE( void AddToList(RRenderOp** pOp, uint numOps) )	
	_PURE( uint GetNumOfDrawOps() )
};

//// IRPipeline
//struct IRPipeline
//{	
//	_PURE( uint CreateNewRenderGroup(uint groupNum, RenderOpSort sortFn, IRRenderCallback& callback) )
//	_PURE( void AddRenderTarget(uint groupIndex, IRTextureRT* pRT) )
//	_PURE( void SetDepthBuffer(uint groupIndex, IRDepthBuffer* pDepth) )
//	_PURE( void SetClearColor(uint groupIndex, const gmtl::Vec4f& color) )
//	_PURE( void SetClearDepth(uint groupIndex, float val) )
//	//_PURE( void SetClearStencil(uint groupIndex, uint stencil) )
//	_PURE( uint GetRenderGroupIndex(uint groupNum) )
//	_PURE( void AddToRenderGroup(uint groupIndex,
//								IRVertexBufferGroup* pVBGroup,
//								IRIndexBuffer* pIB,
//								IREffect* pEffect,
//								REffectParam* pParams,
//								uint techToUse,
//								void* pSortData,
//								eRPrimitiveType prim=PRIM_TRIANGLE) )
//
//	_PURE( void RenderPipeline() )
//	_PURE( void RenderImmediate(uint groupIndex) )
//
//	_PURE( uint GetNumOfRenderGroups() )
//	_PURE( uint GetRenderGroupID(uint groupIndex) )
//	_PURE( uint GetNumOfRenderTargets(uint groupIndex) )
//	_PURE( IRTextureRT* GetRenderTarget(uint groupIndex, uint rtIndex) )
//	_PURE( IRDepthBuffer* GetDepthBuffer(uint groupIndex) )
//};

// IREffectTemplate :
struct IREffectTemplate : public IRResource
{
	_PURE( uint GetNumOfInstancedParams(uint techIndex) )
	_PURE( uint GetInstancedParamSemantic(uint techIndex, uint paramIndex) )
	_PURE( void ApplyInstancedParams(const REffectParam* pEffectParams) )

	_PURE( uint GetNumOfDynamicParams(uint techIndex) )
	_PURE( uint GetDynamicParamSemantic(uint techIndex, uint paramIndex) )
	_PURE( eREffectParamType GetDynamicParamType(uint techIndex, uint paramIndex) )
	
	_PURE( uint GetNumOfConstantParams() )
	_PURE( eREffectParamType GetConstantParamType(uint paramIndex) )
	_PURE( int GetConstantParamIndex(uint semantic) )
	_PURE( uint GetConstantParamSemantic(uint paramIndex) )
	_PURE( uint GetConstantParamNumDesc(uint paramIndex) )
	_PURE( REffectParam GetConstantParamDesc(uint paramIndex, uint descIndex) )
	_PURE( const wchar* GetConstantParamDescName(uint paramIndex, uint descIndex) )
	_PURE( REffectParam GetConstantParamDefaultValue(uint paramIndex) )
	
	_PURE( void BeginTechnique(uint techIndex) )
	_PURE( void BeginPass(uint passIndex) )

	_PURE( void EndTechnique() )
	_PURE( void EndPass() )

	_PURE( uint GetNumOfPasses(uint techIndex) )
	_PURE( uint GetNumOfTechs() )

	//_PURE( REffectParam GetEffectDesc(uint index) )
	//_PURE( const wchar* GetEffectDescName(uint index) )
	//_PURE( uint GetNumEffectDesc() )

};

// IREffect
struct IREffect : public IRResource
{
	_PURE( uint GetNumOfTechs() const )
//	_PURE( int GetEffectType() )	
	_PURE( IREffectTemplate* GetTemplate() )
	_PURE( uint GetNumOfParams() const )
	_PURE( const REffectParam* GetParam(uint index) const )
	_PURE( void SetTemplateFileName(const wchar* pFileName) )
	_PURE( const wchar* GetTemplateFileName() const )
};


// IRVertexBuffer
struct IRVertexBuffer : public IRResource
{
	_PURE( void SetValidRange(uint startVertex, uint numVertices) )

	_PURE( byte* Lock(uint startVertexToWrite, uint numVerticesToWrite) )
	_PURE( boolean Unlock(boolean isSetValidRange=TRUE) )

	_PURE( byte* LockImmediate(uint startVertexToWrite, uint numVerticesToWrite) )
	_PURE( boolean UnlockImmediate(boolean isSetValidRange=TRUE) )

	_PURE( uint GetVertexSize() const )
	_PURE( uint GetSize() const )
	_PURE( uint GetNumVertices() const )
	_PURE( uint GetStartVertex() const )
	_PURE( eRBufferUsage GetUsage() const )
	
	_PURE( const RVertexDesc* GetDescription() const )
};

struct IRVertexBufferGroup : public IRResource
{
	_PURE( void AddVertexBuffer(IRVertexBuffer* pVB) )
	_PURE( void RemoveVertexBuffer(uint index) )
	_PURE( IRVertexBuffer* GetVertexBuffer(uint index) )
	_PURE( uint GetNumOfVertexBuffers() )
};

// IRIndexBuffer
struct IRIndexBuffer : public IRResource
{
	_PURE( ushort* Lock(uint startIndexToWrite, uint numIndicesToWrite) )
	_PURE( boolean Unlock(boolean isSetValidRange=TRUE) )

	_PURE( uint GetSize() const )
	_PURE( uint GetNumIndices() const )
	_PURE( uint GetStartIndex() const )
	_PURE( eRBufferUsage GetUsage() const )
	_PURE( void SetValidRange(uint startIndex, uint numIndices) )	

};

// IRRenderer
struct IRRenderer : public IRefCounted
{	
	_PURE( boolean Initialize(const RInitInfo& info) )
	_PURE( boolean SetDisplayInfo(const RDisplayInfo& info) )
	_PURE( const RDisplayInfo& GetDisplayInfo() )
	_PURE( boolean BeginRender() )
	_PURE( void EndRender() )
	
	_PURE( void DisplayFrame() )
	//_PURE( void GetBackBuffer(IRTextureRT* pDestRT) )
	
	_PURE( IRTextureRT* GetBackBufferColor() )
	_PURE( IRDepthBuffer* GetBackBufferDepth() )

	_PURE( const RRendererInfo& GetRendererInfo() )
	_PURE( const RMetrics& GetMetrics() )
	
	//_PURE( IRPipeline&	GetRPipeline() )
	_PURE( IRResourceMgr& GetRResourceMgr() )
	_PURE( IAppCallback& GetAppCallback() ) 
};

// IRResourceMgr
struct IRResourceMgr
{
	_PURE( IRRenderGroup* CreateRenderGroup(RenderOpSort sortFn, uint maxRenderOps=0) )

	_PURE( IREffectTemplate* LoadEffectTemplate(const CRCDataKey* pKey, 
												IByteBuffer* pData) )
												
	_PURE( IREffect* LoadEffect(const CRCDataKey* pTemplateKey, 
								IByteBuffer* pTemplateData,
								const REffectParam* pConstantParams,
								const uint* pSemantics,
								uint numParams) )
								
	_PURE( IREffect* LoadEffect(IREffectTemplate* pTemplate,
								const REffectParam* pConstantParams,
								const uint* pSemantics,
								uint numParams) )

	// TODO: To be removed (need to parse format in user apps manually)
	_PURE( IRTexture2D* LoadTexture2D(const CRCDataKey* pKey, IByteBuffer* pData) )
	_PURE( void SaveTexture2D(IRTexture2D* pTex, IByteBuffer* pData) )

	_PURE( IRTexture2D* CreateTexture2D(const CRCDataKey* pKey,
									uint width, 
									uint height, 
									uint mipMapLevel, 
									eRTexFormat format,
									eRTexUsage usage) )

	_PURE( IRTexture3D* LoadTexture3D(const CRCDataKey* pKey, IByteBuffer* pData) )

	// TODO: To be removed (need to parse format in user apps manually)
	_PURE( IRTextureCube* LoadTextureCube(const CRCDataKey* pKey, IByteBuffer* pData) )
	
	_PURE( IRTextureCube* CreateTextureCube(const CRCDataKey* pKey,
											uint size, 
											uint mipMapLevel, 
											eRTexFormat format,
											eRTexUsage usage) )

	_PURE( IRTextureCube* CreateTextureCubeRT(uint size, eRTexFormat format) )

	_PURE( IRTexture2D* CreateTexture2DRT(uint width, 
										uint height,
										uint mipLevels,
										eRTexFormat format) )
									
	_PURE( IRDepthBuffer* CreateDepthBuffer(uint width, 
											uint height,
											eRDepthFormat format,
											uint multiSampleType) )
	
	_PURE( IRVertexBuffer* CreateVertexBuffer(const CRCDataKey* pKey, 
											const RVertexDesc* pDesc, 
											uint numVertices, 
											eRBufferUsage usage) )

	_PURE( IRVertexBuffer* CreateVertexBufferShared(const RVertexDesc* pDesc, uint numVertices, IRVertexBuffer* pSource) )

	_PURE( IRIndexBuffer* CreateIndexBuffer(const CRCDataKey* pKey,
											uint numIndices, 
											eRBufferUsage usage) )

	_PURE( IRIndexBuffer* CreateIndexBufferShared(uint numIndices, IRIndexBuffer* pSource) )

	_PURE( IRVertexBufferGroup* CreateVertexBufferGroup(const CRCDataKey* pKey) )
};


//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IRRenderer)
_DECLARE_SMARTPTR(IREffect)
_DECLARE_SMARTPTR(IREffectTemplate)
_DECLARE_SMARTPTR(IRTextureCube)
_DECLARE_SMARTPTR(IRTexture2D)
_DECLARE_SMARTPTR(IRTexture)
_DECLARE_SMARTPTR(IRTextureRT)
_DECLARE_SMARTPTR(IRVertexBuffer)
_DECLARE_SMARTPTR(IRVertexBufferGroup)
_DECLARE_SMARTPTR(IRIndexBuffer)
_DECLARE_SMARTPTR(IRDepthBuffer)
_DECLARE_SMARTPTR(IRRenderGroup)

//////////////////////////////////////////////////////////////////////////
//
//	Helper Structs
//
//////////////////////////////////////////////////////////////////////////

struct REffectParam
{
	eREffectParamType type;
	uint numBytes;

	union
	{
		int intVal;
		boolean boolVal;
		float floatVal;
		void* pVal;
		//gmtl::Vec3f* pVec3;
		const gmtl::Vec4f* pVec4;
		const gmtl::Vec3f* pVec3;
		const gmtl::Vec2f* pVec2;
		const gmtl::Matrix44f* pMat44;
		const gmtl::Matrix33f* pMat33;
		const gmtl::Matrix22f* pMat22;
		const IRTexture2D* pTex;
		const IRTextureRT* pRT;
		const IRTextureCube* pCube;
		const wchar* pStr;
	};

	//REffectParam()
	//	: type(EPT_END)
	//	, numBytes(0)
	//	, pVal(0)
	//{
	//}

	//~REffectParam()
	//{
	//}

#define _COMPARE( op ) \
	{ \
	switch(type) \
	{ \
	case EPT_FLOAT:		return (floatVal op rhs.floatVal); \
	case EPT_INT:		return (intVal op rhs.intVal); \
	case EPT_BOOL:		return (boolVal op rhs.boolVal); \
	case EPT_VOID_P:	return (pVal op rhs.pVal); \
	case EPT_TEX2D_P:		return (pTex op rhs.pTex); \
	case EPT_TEXCUBE_P:		return (pCube op rhs.pCube); \
	case EPT_MAT44_P:		return (pMat44 op rhs.pMat44); \
	case EPT_MAT33_P:		return (pMat33 op rhs.pMat33); \
	case EPT_MAT22_P:		return (pMat22 op rhs.pMat22); \
	case EPT_VEC4_P:		return (pVec4 op rhs.pVec4); \
	case EPT_VEC3_P:		return (pVec3 op rhs.pVec3); \
	case EPT_VEC2_P:		return (pVec2 op rhs.pVec2); \
	case EPT_STRING:	return (pStr op rhs.pStr); \
	\
default: _DEBUG_ASSERT(FALSE); \
	} \
	\
	return FALSE; \
	}

	inline void SetInt(int val)
	{
		intVal = val;
		type = EPT_INT;
	}

	inline void SetBoolean(boolean val)
	{
		boolVal = val;
		type = EPT_BOOL;
	}

	inline void SetFloat(float val)
	{
		floatVal = val;
		type = EPT_FLOAT;
	}

	inline void SetValue(void* pData, uint numBytes)
	{
		pVal = pData;
		this->numBytes = numBytes;
		type = EPT_VOID_P;
	}

	inline void SetVector4(const gmtl::Vec4f* pData)
	{
		pVec4 = pData;
		type = EPT_VEC4_P;
	}

	inline void SetVector3(const gmtl::Vec3f* pData)
	{
		pVec3 = pData;
		type = EPT_VEC3_P;
	}

	inline void SetVector2(const gmtl::Vec2f* pData)
	{
		pVec2 = pData;
		type = EPT_VEC2_P;
	}

	inline void SetMatrix44(const gmtl::Matrix44f* pData)
	{
		pMat44 = pData;
		type = EPT_MAT44_P;
	}

	inline void SetMatrix33(const gmtl::Matrix33f* pData)
	{
		pMat33 = pData;
		type = EPT_MAT33_P;
	}

	inline void SetMatrix22(const gmtl::Matrix22f* pData)
	{
		pMat22 = pData;
		type = EPT_MAT22_P;
	}

	inline void SetTexture2D(const IRTexture2D* pData)
	{
		pTex = pData;
		type = EPT_TEX2D_P;
	}

	//inline void SetTextureRT(const IRTextureRT* pData)
	//{
	//	pRT = pData;
	//	type = EPT_TEXRT_P;
	//}
	
	inline void SetTextureCube(const IRTextureCube* pData)
	{
		pCube = pData;
		type = EPT_TEXCUBE_P;
	}

	inline void SetString(const wchar* pData)
	{
		pStr = pData;
		type = EPT_STRING;
	}

	inline bool operator==(const REffectParam& rhs) const
	{
		_COMPARE( == )
	}

	inline bool operator!=(const REffectParam& rhs) const
	{

		_COMPARE( != )
	}

	inline bool operator<(const REffectParam& rhs) const
	{
		_COMPARE( < )
	}
};

struct REffectParamStore
{
	eREffectParamType type;

	IByteBufferPtr pStore;
	
	REffectParamStore()
		: type(EPT_END)
	{
		ClearAlloc();
	}

	REffectParamStore(const REffectParam& param)
		: type(EPT_END)
	{	
		*this = param;
	}

	REffectParamStore(const REffectParamStore& store)
		: type(EPT_END)
	{
		*this = store;
	}

	~REffectParamStore()
	{
		ClearAlloc();
	}

	inline int GetInt()			const { return *((int*)(GetValue()));	}
	inline boolean GetBoolean()	const { return *((boolean*)(GetValue()));	}
	inline float GetFloat()		const { return *((float*)(GetValue()));	}
	inline void* GetValue()		const { return pStore->GetData();	}
	inline const gmtl::Vec4f* GetVector4()	const { return ((const gmtl::Vec4f*)(GetValue()));	}
	inline const gmtl::Vec3f* GetVector3()	const { return ((const gmtl::Vec3f*)(GetValue()));	}
	inline const gmtl::Vec2f* GetVector2()	const { return ((const gmtl::Vec2f*)(GetValue()));	}
	inline const gmtl::Matrix44f* GetMatrix44()	const { return ((const gmtl::Matrix44f*)(GetValue()));	}
	inline const gmtl::Matrix33f* GetMatrix33()	const { return ((const gmtl::Matrix33f*)(GetValue()));	}
	inline const gmtl::Matrix22f* GetMatrix22()	const { return ((const gmtl::Matrix22f*)(GetValue()));	}
	inline const IRTexture2D* GetTexture2D()	const { return *((const IRTexture2D**)(GetValue()));	}
	inline const IRTextureCube* GetTextureCube()	const { return *((const IRTextureCube**)(GetValue()));	}
	inline const wchar* GetString()	const { return ((const wchar*)(GetValue()));	}

	inline void Set(void* pData, uint numBytes, eREffectParamType paramType)
	{
		if(pStore)
			ClearAlloc();
		else
			pStore = _NEW CByteBuffer(numBytes);

		AppendData(pStore, pData, numBytes);
		//AppendData(pStore, (uint)paramType);	// Remove this
		type = paramType;
	}
	
	inline void SetInt(int val)
	{
		Set(&val, sizeof(int), EPT_INT);
	}

	inline void SetBoolean(boolean val)
	{
		Set(&val, sizeof(boolean), EPT_BOOL);
	}

	inline void SetFloat(float val)
	{
		Set(&val, sizeof(float), EPT_FLOAT);
	}
	
	inline void SetValue(void* pData, uint numBytes)
	{
		Set(pData, numBytes, EPT_VOID_P);
	}

	inline void SetVector4(const gmtl::Vec4f* pData)
	{
		Set((void*)(pData), sizeof(gmtl::Vec4f), EPT_VEC4_P);
	}

	inline void SetVector3(const gmtl::Vec3f* pData)
	{
		Set((void*)(pData), sizeof(gmtl::Vec3f), EPT_VEC3_P);
	}

	inline void SetVector2(const gmtl::Vec2f* pData)
	{
		Set((void*)(pData), sizeof(gmtl::Vec2f), EPT_VEC2_P);
	}

	inline void SetMatrix44(const gmtl::Matrix44f* pData)
	{
		Set((void*)(pData), sizeof(gmtl::Matrix44f), EPT_MAT44_P);
	}

	inline void SetMatrix33(const gmtl::Matrix33f* pData)
	{
		Set((void*)(pData), sizeof(gmtl::Matrix33f), EPT_MAT33_P);
	}

	inline void SetMatrix22(const gmtl::Matrix22f* pData)
	{
		Set((void*)(pData), sizeof(gmtl::Matrix22f), EPT_MAT22_P);
	}

	inline void SetTexture2D(const IRTexture2D* pData)
	{
		((IRTexture2D*)(pData))->AddRef();
		Set(&pData, sizeof(const IRTexture2D*), EPT_TEX2D_P);
	}

	inline void SetTextureCube(const IRTextureCube* pData)
	{
		((IRTextureCube*)(pData))->AddRef();
		Set(&pData, sizeof(const IRTextureCube*), EPT_TEXCUBE_P);
	}
	
	inline void SetString(const wchar* pData)
	{
		uint size = (uint)((wcslen(pData) + 1) * sizeof(wchar));
		Set((void*)pData, size, EPT_STRING);
	}

	inline void ClearAlloc()
	{
		if(pStore)
		{
			switch(type)
			{
			case EPT_TEX2D_P:
				{
					IRTexture2D* pTex = (IRTexture2D*)(GetTexture2D());
					_RELEASE(pTex);
					break;
				}
				
			//case EPT_TEXRT_P:
			//	{
			//		IRTextureRT* pRT = (IRTextureRT*)(GetTextureRT());
			//		_RELEASE(pRT);
			//		break;
			//	}
				
			case EPT_TEXCUBE_P:
				{
					IRTextureCube* pCube = (IRTextureCube*)(GetTextureCube());
					_RELEASE(pCube);
					break;
				}
			}

			pStore->SetDataLength(0);	
		}

		type = EPT_END;	
	}

	// Comparison
	inline boolean operator ==(const REffectParamStore& rhs) const
	{
		if(type == rhs.type)
			if(pStore->GetDataLength() == rhs.pStore->GetDataLength())
				return memcmp(pStore->GetData(), rhs.pStore->GetData(), pStore->GetDataLength()) == 0;
		
		return FALSE;
	}

	inline boolean operator ==(const REffectParam& rhs) const
	{
		if(type == rhs.type)
		{
			switch(type)
			{
				case EPT_FLOAT:		return GetFloat() == rhs.floatVal;
				case EPT_INT:		return GetInt()	== rhs.intVal;
				case EPT_BOOL:		return GetBoolean() == rhs.boolVal; 
				
				case EPT_TEX2D_P:	return GetTexture2D() == rhs.pTex;
				//case EPT_TEXRT_P:	return GetTextureRT() == rhs.pRT;
				case EPT_TEXCUBE_P:	return GetTextureCube() == rhs.pCube;

				case EPT_VOID_P:
				case EPT_MAT44_P:		
				case EPT_MAT33_P:		
				case EPT_MAT22_P:		
				case EPT_VEC4_P:		
				case EPT_VEC3_P:		
				case EPT_VEC2_P:		
				case EPT_STRING:	
					return memcmp(pStore->GetData(), rhs.pVal, pStore->GetDataLength()) == 0;
			}
		}
			
		return FALSE;
	}
	
	inline boolean operator !=(const REffectParamStore& rhs) const
	{
		return !(*this == rhs);
	}
	
	inline boolean operator !=(const REffectParam& rhs) const
	{
		return !(*this == rhs);
	}

	// Assignment
	inline REffectParamStore& operator =(const REffectParamStore& rhs)
	{
		switch(rhs.type)
		{
		case EPT_TEX2D_P:	SetTexture2D(rhs.GetTexture2D());		break;
		//case EPT_TEXRT_P:	SetTextureRT(rhs.GetTextureRT());		break;
		case EPT_TEXCUBE_P:	SetTextureCube(rhs.GetTextureCube());	break;
		case EPT_VOID_P:	SetValue(rhs.pStore->GetData(), rhs.pStore->GetDataLength()-sizeof(uint));	break;
		case EPT_MAT44_P:	SetMatrix44(rhs.GetMatrix44());		break;
		case EPT_MAT33_P:	SetMatrix33(rhs.GetMatrix33());		break;
		case EPT_MAT22_P:	SetMatrix22(rhs.GetMatrix22());		break;
		case EPT_VEC4_P:	SetVector4(rhs.GetVector4());		break;
		case EPT_VEC3_P:	SetVector3(rhs.GetVector3());		break;
		case EPT_VEC2_P:	SetVector2(rhs.GetVector2());		break;
		case EPT_FLOAT:		SetFloat(rhs.GetFloat());		break;
		case EPT_INT:		SetInt(rhs.GetInt());			break;
		case EPT_BOOL:		SetBoolean(rhs.GetBoolean());		break;
		case EPT_STRING:	SetString(rhs.GetString());			break;
		case EPT_END:		ClearAlloc();					break;

		default: _DEBUG_ASSERT(FALSE);
		}

		return *this;	
	}
	
	inline REffectParamStore& operator =(const REffectParam& rhs)
	{
		switch(rhs.type)
		{
		case EPT_TEX2D_P:		SetTexture2D(rhs.pTex);			break;
		//case EPT_TEXRT_P:		SetTextureRT(rhs.pRT);			break;
		case EPT_TEXCUBE_P:		SetTextureCube(rhs.pCube);		break;
		case EPT_VOID_P:	SetValue(rhs.pVal, rhs.numBytes);	break;
		case EPT_MAT44_P:	SetMatrix44(rhs.pMat44);		break;
		case EPT_MAT33_P:	SetMatrix33(rhs.pMat33);		break;
		case EPT_MAT22_P:	SetMatrix22(rhs.pMat22);		break;
		case EPT_VEC4_P:	SetVector4(rhs.pVec4);		break;
		case EPT_VEC3_P:	SetVector3(rhs.pVec3);		break;
		case EPT_VEC2_P:	SetVector2(rhs.pVec2);		break;
		case EPT_FLOAT:		SetFloat(rhs.floatVal);		break;
		case EPT_INT:		SetInt(rhs.intVal);			break;
		case EPT_BOOL:		SetBoolean(rhs.boolVal);		break;
		case EPT_STRING:	SetString(rhs.pStr);			break;
		case EPT_END:		ClearAlloc();				break;

		default: _DEBUG_ASSERT(FALSE);
		}

		return *this;	
	}

	inline REffectParam& AssignREffectParam(REffectParam& param)
	{
		switch(type)
		{
		case EPT_TEX2D_P:		param.SetTexture2D(GetTexture2D());		break;
		//case EPT_TEXRT_P:		param.SetTextureRT(GetTextureRT());		break;
		case EPT_TEXCUBE_P:		param.SetTextureCube(GetTextureCube());	break;
		case EPT_VOID_P:	param.SetValue(pStore->GetData(), pStore->GetDataLength());	break;
		case EPT_MAT44_P:	param.SetMatrix44(GetMatrix44());		break;
		case EPT_MAT33_P:	param.SetMatrix33(GetMatrix33());		break;
		case EPT_MAT22_P:	param.SetMatrix22(GetMatrix22());		break;
		case EPT_VEC4_P:	param.SetVector4(GetVector4());		break;
		case EPT_VEC3_P:	param.SetVector3(GetVector3());		break;
		case EPT_VEC2_P:	param.SetVector2(GetVector2());		break;
		case EPT_FLOAT:		param.SetFloat(GetFloat());		break;
		case EPT_INT:		param.SetInt(GetInt());			break;
		case EPT_BOOL:		param.SetBoolean(GetBoolean());		break;
		case EPT_STRING:	param.SetString(GetString());			break;
		case EPT_END:		
			param.type = EPT_END;
			param.numBytes = 0;
			param.pVal = NULL; 
			break;

		default: _DEBUG_ASSERT(FALSE);
		}

		return param;
	}
};

struct RRenderOp
{
	const static uint MAX_NUM_PARAMS = 16;

	//------------------------------------------------------------------------------
	//	The renderer will fill in these
	//------------------------------------------------------------------------------
	IRVertexBufferGroup* pVBGroup;
	IRIndexBuffer* pIB;
	IREffect* pEffect;
	IREffectTemplate* pEffectTemplate;
	uint technique;
	
	REffectParam params[MAX_NUM_PARAMS];
	
	eRPrimitiveType prim;
	
	//------------------------------------------------------------------------------
	//	Instancing info
	//------------------------------------------------------------------------------
	uint opHash;
	union
	{
		uint numInstancedParams;
		uint numInstances;
	};

	
	//------------------------------------------------------------------------------
	//	User data for sorting purposes
	//	Added by application
	//------------------------------------------------------------------------------
	RVarData userData;
};

_NAMESPACE_END


//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IRRenderer* (*CreateRendererFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE IRRenderer* CreateRenderer() )
}
