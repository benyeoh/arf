//==============================================================================
//
//		CDFXDebugFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/21/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CDFXDebugFX : public CRefCounted<IDFXDebugFX>
{	
	struct SharedIndicesPool
	{
		std::vector< IRIndexBufferPtr > sharedIndices;
		uint numSharedIndicesUsed;

		SharedIndicesPool()
			: numSharedIndicesUsed(0)
		{
		}
	};

private: 
	IAppCallback* m_pAppCallback;
	IRRendererPtr m_pRenderer;
	CDFXResourceMgr m_ResMgr;
	CDFXParams m_Params;
	
	IREffectPtr m_pWireFrame;

	IRRenderGroupPtr m_pRenderGroup;

	SharedIndicesPool m_WireBoxSharedIBPool;
	SharedIndicesPool m_WireSphereSharedIBPool;
	SharedIndicesPool m_LineSharedIBPool;
	SharedIndicesPool m_QuadSharedIBPool;

public: 
	CDFXDebugFX()
		: m_pAppCallback(NULL)
	{
		m_ResMgr.SetDebugFX(this);
		m_Params.SetDebugFX(this);
	}

	~CDFXDebugFX()
	{
	}

private:
	IRIndexBuffer* AllocSharedIBFromPool(SharedIndicesPool& pool, IRIndexBuffer* pSourceIB);

public: 
	boolean Initialize(IRRenderer& renderer, IAppCallback& callback);

	IAppCallback& GetAppCallback() { return *m_pAppCallback; }
	IRRenderer& GetRenderer() { return *m_pRenderer; }
	IDFXResourceMgr& GetResourceMgr() { return m_ResMgr; }
	IDFXParams& GetParams() { return m_Params; }
	
	void SetViewport(uint left, uint top, uint width, uint height);
	void AddRenderTarget(IRTextureRT* pTexRT);
	void ClearRenderTargets();
	void SetDepthBuffer(IRDepthBuffer* pDepth);

	void BeginWireFrame();
	void EndWireFrame();
		
	void RenderWireBox(const AABox& box,
		const gmtl::Vec4f& color);
		//const gmtl::Matrix44f& transform);
	void RenderWireBox(const OOBox& box,
		const gmtl::Vec4f& color);
	void RenderWireSphere(const gmtl::Vec3f& pos,
		float radius,
		const gmtl::Vec4f& color);
	void RenderLine(const gmtl::Vec3f& pos1, 
		const gmtl::Vec3f& pos2, 
		const gmtl::Vec4f& color);
	void RenderQuad(const gmtl::Vec3f& pos, 
		const gmtl::Vec2f& size,
		const gmtl::Vec2f& topLeftTexPos,
		const gmtl::Vec2f& texSize,
		IRTexture2D* pTex);
	//void RenderQuad(const gmtl::Vec3f& pos, 
	//	const gmtl::Vec2f& size,
	//	const gmtl::Vec2f& topLeftTexPos,
	//	const gmtl::Vec2f& texSize,
	//	IRTextureRT* pTex);
	void Flush();
};

_NAMESPACE_END
