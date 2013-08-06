//==============================================================================
//
//		GeometryFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/2/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

uint 
GetVertexSize(const RVertexDesc* pDesc);

uint
GetVertexOffset(const RVertexDesc* pDesc, eRVDUsage usage, uint index);

uint
GetVertexDataSize(eRVDUsage usage);

void
ComputeTangentAndBinormals(void* pVertexData, 
						uint numVertices,
						const ushort* pIndices, 
						uint numIndices,
						const RVertexDesc* pDesc,
						uint posIndex = 0,
						uint texUVIndex = 0,
						uint normalIndex = 0,
						uint tangentIndex = 0,
						uint binormalIndex = 0);

void
ComputeTangentAndBinormals(IRVertexBuffer* pVB,
						IRIndexBuffer* pIB,
						uint posIndex = 0,
						uint texUVIndex = 0,
						uint normalIndex = 0,
						uint tangentIndex = 0,
						uint binormalIndex = 0);

void GetAdjacentIndicesPerVertex(IRVertexBuffer* pVB, IRIndexBuffer* pIB, IByteBuffer* pAdjacent, IByteBuffer* pNumAdjacent, float sharedSurfaceDistThreshold);

void GetIndicesInRangePerVertex(IRVertexBuffer* pVB, IByteBuffer* pInRange, IByteBuffer* pNumInRange, float rangeThreshold);

_NAMESPACE_END