//==============================================================================
//
//		BFXUScreenSpaceMesh.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/20/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class BFXUScreenSpaceMesh 
{
private: 
	IRVertexBufferGroupPtr m_pQuadVerts;
	IRIndexBufferPtr m_pQuadIndices;
	uint m_ScreenWidth;
	uint m_ScreenHeight;
		
public: 
	BFXUScreenSpaceMesh()
		: m_ScreenHeight(0)
		, m_ScreenWidth(0)
	{
	}

	~BFXUScreenSpaceMesh()
	{
		m_pQuadVerts = NULL;
		m_pQuadIndices = NULL;
	}

public: 
	void Initialize(uint screenWidth, uint screenHeight, IRRenderer* pRenderer, eRBufferUsage usage=BUF_DEFAULT, boolean isUsePixelOffset=TRUE);
	void UpdateMesh(IRRenderer* pRenderer, uint screenWidth, uint screenHeight, 
					const gmtl::Vec2f& bottomLeft, const gmtl::Vec2f& topRight, boolean isUsePixelOffset=TRUE);

	//void Shutdown();
	
	uint GetScreenWidth();
	uint GetScreenHeight();
		
	IRVertexBufferGroup* GetQuadVertices();
	IRIndexBuffer* GetQuadIndices();
};

_NAMESPACE_END
