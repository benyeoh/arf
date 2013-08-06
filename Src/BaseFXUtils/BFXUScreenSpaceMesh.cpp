//==============================================================================
//
//		BFXUScreenSpaceMesh.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/20/2008
//
//==============================================================================

#include "BaseFXUtils.h"

_NAMESPACE_BEGIN

void 
BFXUScreenSpaceMesh::Initialize(uint screenWidth, uint screenHeight, IRRenderer* pRenderer, eRBufferUsage usage, boolean isUsePixelOffset)
{
	// Create the fullscreen quad
	RVertexDesc desc[] = 
	{
		{ VDU_POSITION, 0 },
		{ VDU_TEXCOORDF2, 0 },
		//{ VDU_NORMAL, 0 },
		{ VDU_BLENDINDICES, 0 },
		{ VDU_END, 0 },
	};

	// Create the vertex and index buffers
	gmtl::Vec3f pos[4];
	gmtl::Vec2f texUV[4];
	//gmtl::Vec3f normal[4];
	uint blendIndices[4];

	const RRendererInfo& info = pRenderer->GetRendererInfo();
	
	gmtl::Vec2f pixelOffset(0.0f, 0.0f);
	if(isUsePixelOffset)
	{
		pixelOffset[0] = ((info.pixelOffset*2.0f) / screenWidth);
		pixelOffset[1] = ((info.pixelOffset*2.0f) / screenHeight);
	}

	// Top Left
	pos[0][0] = -1.0f - pixelOffset[0];
	pos[0][1] = 1.0f + pixelOffset[1];
	pos[0][2] = 0.0f;
	texUV[0][0] = 0.0f;
	texUV[0][1] = 0.0f;
	blendIndices[0] = 0;

	//normal[0][0] = 0.0f;
	//normal[0][1] = 0.0f;
	//normal[0][2] = 1.0f;

	// Top Right
	pos[1][0] = 1.0f - pixelOffset[0];
	pos[1][1] = 1.0f + pixelOffset[1];
	pos[1][2] = 0.0f;
	texUV[1][0] = 1.0f;
	texUV[1][1] = 0.0f;
	blendIndices[1] = (0x1 << 24) | (0x1 << 16) | (0x1 << 8) | 0x1;

	//normal[0][0] = 0.0f;
	//normal[0][1] = 0.0f;
	//normal[0][2] = 1.0f;
	
	// Bottom Left
	pos[2][0] = -1.0f - pixelOffset[0];
	pos[2][1] = -1.0f + pixelOffset[1];
	pos[2][2] = 0.0f;
	texUV[2][0] = 0.0f;
	texUV[2][1] = 1.0f;
	blendIndices[2] = (0x2 << 24) | (0x2 << 16) | (0x2 << 8) | 0x2;
	//normal[0][0] = 0.0f;
	//normal[0][1] = 0.0f;
	//normal[0][2] = 1.0f;
	
	// Bottom Right
	pos[3][0] = 1.0f - pixelOffset[0];
	pos[3][1] = -1.0f + pixelOffset[1];
	pos[3][2] = 0.0f;
	texUV[3][0] = 1.0f;
	texUV[3][1] = 1.0f;
	blendIndices[3] = (0x3 << 24) | (0x3 << 16) | (0x3 << 8) | 0x3;
	//normal[0][0] = 0.0f;
	//normal[0][1] = 0.0f;
	//normal[0][2] = 1.0f;
	
	m_pQuadVerts = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
	IRVertexBufferPtr m_pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, desc, 4, usage);
	uint vertexStride = m_pVB->GetVertexSize() / sizeof(float);
	float* pVBData = (float*) m_pVB->Lock(0, 4);
	_LOOPi(4)
	{
		pVBData[i*vertexStride] = pos[i][0];
		pVBData[i*vertexStride+1] = pos[i][1];
		pVBData[i*vertexStride+2] = pos[i][2];
		pVBData[i*vertexStride+3] = texUV[i][0];
		pVBData[i*vertexStride+4] = texUV[i][1];
		*((uint*)&(pVBData[i*vertexStride+5])) = blendIndices[i];
		//normal[i][0];
		//pVBData[i*vertexStride+6] = normal[i][1];
		//pVBData[i*vertexStride+7] = normal[i][2];
	}
	m_pVB->Unlock();
	
	m_pQuadIndices = pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, 6, BUF_DEFAULT);
	ushort* pIBData = m_pQuadIndices->Lock(0, 6);
	pIBData[0] = 0;
	pIBData[1] = 1;
	pIBData[2] = 3;
	pIBData[3] = 0;
	pIBData[4] = 3;
	pIBData[5] = 2;
	m_pQuadIndices->Unlock();

	m_pQuadVerts->AddVertexBuffer(m_pVB);
	
	m_ScreenHeight = screenHeight;
	m_ScreenWidth = screenWidth;
}

void 
BFXUScreenSpaceMesh::UpdateMesh(IRRenderer* pRenderer, uint screenWidth, uint screenHeight, 
								const gmtl::Vec2f& bottomLeft, const gmtl::Vec2f& topRight, boolean isUsePixelOffset)
{
	const RRendererInfo& info = pRenderer->GetRendererInfo();

	gmtl::Vec2f pixelOffset(0.0f, 0.0f);
	if(isUsePixelOffset)
	{
		pixelOffset[0] = ((info.pixelOffset*2.0f) / screenWidth);
		pixelOffset[1] = ((info.pixelOffset*2.0f) / screenHeight);
	}

	// Top Left
	gmtl::Vec3f pos[4];
	gmtl::Vec2f texUV[4];
	pos[0][0] = bottomLeft[0] - pixelOffset[0];
	pos[0][1] = topRight[1] + pixelOffset[1];
	texUV[0][0] = bottomLeft[0] * 0.5f + 0.5f;
	texUV[0][1] = 1.0f - (topRight[1] * 0.5f + 0.5f);

	// Top Right
	pos[1][0] = topRight[0] - pixelOffset[0];
	pos[1][1] = pos[0][1];
	texUV[1][0] = topRight[0] * 0.5f + 0.5f;
	texUV[1][1] = texUV[0][1];

	// Bottom Left
	pos[2][0] = pos[0][0];
	pos[2][1] = bottomLeft[1] + (pixelOffset[1]);
	texUV[2][0] = texUV[0][0];
	texUV[2][1] = 1.0f - (bottomLeft[1] * 0.5f + 0.5f);

	// Bottom Right
	pos[3][0] = pos[1][0];
	pos[3][1] = pos[2][1];
	texUV[3][0] = texUV[1][0];
	texUV[3][1] = texUV[2][1];

	IRVertexBuffer* pVB = m_pQuadVerts->GetVertexBuffer(0);
	uint vertexStride = pVB->GetVertexSize() / sizeof(float);
	float* pVBData = (float*) pVB->Lock(0, 4);
	_LOOPi(4)
	{
		pVBData[i*vertexStride] = pos[i][0];
		pVBData[i*vertexStride+1] = pos[i][1];
		pVBData[i*vertexStride+3] = texUV[i][0];
		pVBData[i*vertexStride+4] = texUV[i][1];
	}
	pVB->Unlock();
}

//
//void 
//RFXScreenSpaceMesh::Shutdown()
//{
//	m_pQuadVerts = NULL;
//	m_pQuadIndices = NULL;
//	m_ScreenHeight = 0;
//	m_ScreenWidth = 0;
//}


uint 
BFXUScreenSpaceMesh::GetScreenWidth()
{
	return m_ScreenWidth;
}

uint 
BFXUScreenSpaceMesh::GetScreenHeight()
{
	return m_ScreenHeight;
}

IRVertexBufferGroup* 
BFXUScreenSpaceMesh::GetQuadVertices()
{
	return m_pQuadVerts;
}

IRIndexBuffer* 
BFXUScreenSpaceMesh::GetQuadIndices()
{
	return m_pQuadIndices;
}

_NAMESPACE_END