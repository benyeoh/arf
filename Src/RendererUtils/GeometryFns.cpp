//==============================================================================
//
//		GeometryFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/2/2007
//
//==============================================================================

#include "RendererUtils.h"

_NAMESPACE_BEGIN


struct UniformLooseGridCell
{
	IByteBuffer* pAllPoints;
};

class UniformLooseGrid
{
private:
	AABox m_Bounds;
	float m_SearchSpan;
	float m_SearchSpanRCP;

	UniformLooseGridCell* m_pCell;
	int m_NumX;
	int m_NumY;
	int m_NumZ;

	std::vector<IByteBufferPtr> m_CellPoints;

public:
	UniformLooseGrid()
	{
	}

	~UniformLooseGrid()
	{
		_DELETE_ARRAY(m_pCell);
	}

private:
	UniformLooseGridCell* GetCell(const gmtl::Vec3f& pos)
	{
		int x = FastTruncate(pos[0] * m_SearchSpanRCP);
		int y = FastTruncate(pos[1] * m_SearchSpanRCP);
		int z = FastTruncate(pos[2] * m_SearchSpanRCP);

		if((x >= 0 && y >= 0 && z >= 0) &&
			(x < m_NumX && y < m_NumY && z < m_NumZ))
		{
			return &m_pCell[x * m_NumY * m_NumZ + y * m_NumZ + z];
		}

		return NULL;
	};

public:
	void Initialize(const AABox& bounds, float searchRadius)
	{
		const float VOL_LIMIT = 1000000.0f;

		m_Bounds = bounds;
		m_SearchSpan = searchRadius * 2.0f;
		_DEBUG_ASSERT(searchRadius > 0.0f);

		float spanX = m_Bounds.max[0] - m_Bounds.min[0];
		if(spanX < m_SearchSpan)
			spanX = m_SearchSpan;

		float spanY = m_Bounds.max[1] - m_Bounds.min[1]; 
		if(spanY < m_SearchSpan)
			spanY = m_SearchSpan;

		float spanZ = m_Bounds.max[2] - m_Bounds.min[2];
		if(spanZ < m_SearchSpan)
			spanZ = m_SearchSpan;

		float vol = spanX * spanY * spanZ;
		float searchVol = m_SearchSpan * m_SearchSpan * m_SearchSpan;
		if((vol / searchVol) > VOL_LIMIT)
		{
			m_SearchSpan = pow(vol / VOL_LIMIT, 1.0f / 3.0f);
			//searchVol = m_SearchRadius * m_SearchRadius * m_SearchRadius;
		}

		m_SearchSpanRCP = 1.0f / m_SearchSpan;

		m_NumX = FastTruncate( spanX * m_SearchSpanRCP ) + 1;
		m_NumY = FastTruncate( spanY * m_SearchSpanRCP ) + 1;
		m_NumZ = FastTruncate( spanZ * m_SearchSpanRCP ) + 1;

		m_pCell = _NEW UniformLooseGridCell[m_NumX * m_NumY * m_NumZ];
		memset(m_pCell, 0, sizeof(UniformLooseGridCell) * m_NumX * m_NumY * m_NumZ);
	}

	UniformLooseGridCell* GetCellFromPos(const gmtl::Vec3f& pos)
	{
		gmtl::Vec3f curPos = pos - _CAST_VEC3(m_Bounds.min);
		return GetCell(curPos);
	}

	void AddToOneCell(const gmtl::Vec3f& pos, uint userData)
	{
		UniformLooseGridCell* pCell = GetCell(pos);
		if(pCell)
		{
			if(!pCell->pAllPoints)
			{
				IByteBuffer* pBuffer = _NEW CByteBuffer(32 * 4);
				m_CellPoints.push_back(pBuffer);
				pCell->pAllPoints = pBuffer;
			}

			// Check if last value added is the same
			if(pCell->pAllPoints->GetDataLength() > 0)
			{
				uint* pLastVal = ((uint*) pCell->pAllPoints->GetData());
				if(pLastVal[ (pCell->pAllPoints->GetDataLength() / sizeof(uint)) - 1 ] == userData)
					return;
			}

			AppendData(pCell->pAllPoints, userData);
		}
	}

	void AddToAllSearchCells(const gmtl::Vec3f& pos, uint userData)
	{
		gmtl::Vec3f refPos = pos - _CAST_VEC3(m_Bounds.min);
		gmtl::Vec3f curPos = refPos;
		AddToOneCell(curPos, userData);

		float halfSearchSpan = 0.5f * m_SearchSpan;

		_LOOPi(2)
		{
			curPos[0] = refPos[0] + halfSearchSpan * ((i*2)-1.0f);
			_LOOPj(2)
			{
				curPos[1] = refPos[1] + halfSearchSpan * ((j*2)-1.0f);
				_LOOPk(2)
				{
					curPos[2] = refPos[2] + halfSearchSpan * ((k*2)-1.0f);
					AddToOneCell(curPos, userData);
				}
			}
		}
	}
};

void GetUniformGridVertices(void* pVertices, uint vertStride, uint numVertices, float searchRadius, UniformLooseGrid& dest)
{
	_DEBUG_ASSERT(vertStride >= sizeof(float) * 4);

	AABox bounds;
	bounds.min.set(9999999999.0f, 9999999999.0f, 9999999999.0f);
	bounds.max.set(-9999999999.0f, -9999999999.0f, -9999999999.0f);

	PointsToAABox((gmtl::Vec4f*) pVertices, numVertices, vertStride, &bounds);
	dest.Initialize(bounds, searchRadius);

	_LOOPi(numVertices)
	{
		dest.AddToAllSearchCells(*((gmtl::Vec3f*)pVertices), i);

		pVertices = ((byte*)pVertices) + vertStride;
	}
}

void 
ComputeTangentAndBinormal(const gmtl::Vec3f& posA, const gmtl::Vec2f& uvA, 
						const gmtl::Vec3f& posB, const gmtl::Vec2f& uvB, 
						const gmtl::Vec3f& posC, const gmtl::Vec2f& uvC,
						gmtl::Vec3f& tangent, gmtl::Vec3f& binormal)
{
	// Get edge 1 (v1 - v2)
	float ex1 = posA[0] - posB[0];
	float ey1 = posA[1] - posB[1];
	float ez1 = posA[2] - posB[2];
	float es1 = uvA[0] - uvB[0];
	float et1 = uvA[1] - uvB[1];

	// Get edge 2 (v1 - v3)
	float ex2 = posA[0] - posC[0];
	float ey2 = posA[1] - posC[1];
	float ez2 = posA[2] - posC[2];
	float es2 = uvA[0] - uvC[0];
	float et2 = uvA[1] - uvC[1];

	// Get plane 1 (ex1 Ax, es1 Ay, et1 Az cross ex2 Bx, es2 By, et2 Bz)
	// Cx = AyBz - AzBy x
	// Cy = AzBx - AxBz u
	// Cz = AxBy - AyBx v
	float A1 = es1*et2 - et1*es2;
	float B1 = et1*ex2 - ex1*et2;
	float C1 = ex1*es2 - es1*ex2;

	// Get plane 2 (ey1 Ax, es1 Ay, et1 Az cross ey2 Bx, es2 By, et2 Bz)
	float A2 = es1*et2 - et1*es2;
	float B2 = et1*ey2 - ey1*et2;
	float C2 = ey1*es2 - es1*ey2;

	// Get plane 3 (ez1 Ax, es1 Ay, et1 Az cross ez2 Bx, es2 By, et2 Bz)
	float A3 = es1*et2 - et1*es2;
	float B3 = et1*ez2 - ez1*et2;
	float C3 = ez1*es2 - es1*ez2;

	const static float EPSILON = 0.000001f;
	
	if(gmtl::Math::abs(A1) < EPSILON)
		A1 = EPSILON;
	if(gmtl::Math::abs(A2) < EPSILON)
		A2 = EPSILON;
	if(gmtl::Math::abs(A3) < EPSILON)
		A3 = EPSILON;

	if((gmtl::Math::abs(B1) < EPSILON) && 
		(gmtl::Math::abs(B2) < EPSILON) && 
		(gmtl::Math::abs(B3) < EPSILON))
	{
		B1 = EPSILON;
		B2 = EPSILON;
		B3 = EPSILON;
	}
	
	if((gmtl::Math::abs(C1) < EPSILON)
	 && (gmtl::Math::abs(C2) < EPSILON)
	  && (gmtl::Math::abs(C3) < EPSILON))
	{
		C1 = EPSILON;
		C2 = EPSILON;
		C3 = EPSILON;
	}

	// U
	tangent[0] = -B1 / A1;	// dx/du
	tangent[1] = -B2 / A2;	// dy/du
	tangent[2] = -B3 / A3;	// dz/du
	//_DEBUG_ASSERT(length(tangent) >= EPSILON);

	gmtl::normalize(tangent);
	
	// V
	binormal[0] = -C1 / A1;	// dx/dv
	binormal[1] = -C2 / A2;	// dy/dv
	binormal[2] = -C3 / A3;	// dz/dv
	//_DEBUG_ASSERT(length(binormal) >= EPSILON);
	
	gmtl::normalize(binormal);
}


uint 
GetVertexSize(const RVertexDesc* pDesc)
{
	uint i = 0;
	uint size = 0;
	while(pDesc[i].usage != VDU_END)
	{
		size += GetVertexDataSize(pDesc[i].usage);		
		++i;
	}
	
	return size;
}

uint
GetVertexDataSize(eRVDUsage usage)
{
	switch( usage )
	{
		case VDU_POSITION:		return sizeof(float)*3;
		case VDU_NORMAL:		return sizeof(float)*3;
		case VDU_BINORMAL:		return sizeof(float)*3;	
		case VDU_TANGENT:		return sizeof(float)*3;
		case VDU_BLENDINDICES:	return sizeof(byte)*4;	
		case VDU_BLENDWEIGHT:	return sizeof(float)*4;					
		case VDU_COLOR:			return sizeof(byte)*4;					
		case VDU_TEXCOORDF1:		return sizeof(float);
		case VDU_TEXCOORDF2:		return sizeof(float)*2;
		case VDU_TEXCOORDF3:		return sizeof(float)*3;
		case VDU_TEXCOORDF4:		return sizeof(float)*4;				

		default: _DEBUG_ASSERT( FALSE );
	}

	return 0;
}

uint
GetVertexOffset(const RVertexDesc* pDesc, eRVDUsage usage, uint usageIndex)
{
	uint i = 0;
	uint size = 0;
	while(pDesc[i].usage != VDU_END &&
		  (pDesc[i].usage != usage || pDesc[i].usageIndex != usageIndex))
	{
		size += GetVertexDataSize(pDesc[i].usage);
		++i;
	}
	
	return size;	
}

void
ComputeTangentAndBinormals(void* pVertexData,
						uint numVertices,
						const ushort* pIndices, 
						uint numIndices,
						const RVertexDesc* pDesc,
						uint posIndex,
						uint texUVIndex,
						uint normalIndex,
						uint tangentIndex,
						uint binormalIndex)
{
	_DEBUG_ASSERT(pVertexData);
	_DEBUG_ASSERT(pIndices);
	_DEBUG_ASSERT(pDesc);
	
	byte* pVertices = (byte*) pVertexData;
	
	uint vertexSize = GetVertexSize(pDesc);
	uint posOffset = GetVertexOffset(pDesc, VDU_POSITION, posIndex);
	_DEBUG_ASSERT(posOffset >= 0 && posOffset < vertexSize);
	uint normalOffset = GetVertexOffset(pDesc, VDU_NORMAL, normalIndex);
	_DEBUG_ASSERT(normalOffset >= 0 && normalOffset < vertexSize);
	uint tangentOffset = GetVertexOffset(pDesc, VDU_TANGENT, tangentIndex);
	_DEBUG_ASSERT(tangentOffset >= 0 && tangentOffset < vertexSize);
	uint texUVOffset = GetVertexOffset(pDesc, VDU_TEXCOORDF2, texUVIndex);
	_DEBUG_ASSERT(texUVOffset >= 0 && texUVOffset < vertexSize);

	uint binormalOffset = GetVertexOffset(pDesc, VDU_BINORMAL, binormalIndex);
	
	//struct Vector3
	//{
	//	float x, y, z;
	//};
	
	// Reset the tangent and binormal	
	IByteBufferPtr pTempTan = _NEW CByteBuffer(numVertices*sizeof(float)*3);
	IByteBufferPtr pTempBin = _NEW CByteBuffer(numVertices*sizeof(float)*3);

	memset(pTempTan->GetData(), 0, numVertices*sizeof(float)*3);
	memset(pTempBin->GetData(), 0, numVertices*sizeof(float)*3);
		
	//_LOOPi(numVertices)
	//{
	//	gmtl::Vec3f& tan = *((gmtl::Vec3f*) (pTempTan->GetData()+i*sizeof(float)*3));
	//	gmtl::Vec3f& bin = *((gmtl::Vec3f*) (pTempBin->GetData()+i*sizeof(float)*3));
	//	
	////	gmtl::Vec3f& tan = *((gmtl::Vec3f*)(pVertices+i*vertexSize+tangentOffset));
	//	tan[0] = 0.0f;
	//	tan[1] = 0.0f;
	//	tan[2] = 0.0f;
	//	
	////	gmtl::Vec3f& bin = *((gmtl::Vec3f*)(pVertices+i*vertexSize+binormalOffset));
	//	bin[0] = 0.0f;
	//	bin[1] = 0.0f;
	//	bin[2] = 0.0f;
	//}
	
	uint numFaces = numIndices / 3;
	_DEBUG_ASSERT((numIndices % 3 == 0));
	_LOOPi(numFaces)
	{
		uint i1 = pIndices[i*3];
		uint i2 = pIndices[i*3+1];
		uint i3 = pIndices[i*3+2];
		
		gmtl::Vec3f& posA = *((gmtl::Vec3f*)(pVertices+i1*vertexSize+posOffset));
		gmtl::Vec3f& posB = *((gmtl::Vec3f*)(pVertices+i2*vertexSize+posOffset));
		gmtl::Vec3f& posC = *((gmtl::Vec3f*)(pVertices+i3*vertexSize+posOffset));
		
		gmtl::Vec2f& uvA = *((gmtl::Vec2f*)(pVertices+i1*vertexSize+texUVOffset));
		gmtl::Vec2f& uvB = *((gmtl::Vec2f*)(pVertices+i2*vertexSize+texUVOffset));
		gmtl::Vec2f& uvC = *((gmtl::Vec2f*)(pVertices+i3*vertexSize+texUVOffset));
		
		gmtl::Vec3f& tanA = *((gmtl::Vec3f*) (pTempTan->GetData()+i1*sizeof(float)*3));
		gmtl::Vec3f& tanB = *((gmtl::Vec3f*) (pTempTan->GetData()+i2*sizeof(float)*3));
		gmtl::Vec3f& tanC = *((gmtl::Vec3f*) (pTempTan->GetData()+i3*sizeof(float)*3));
		
		gmtl::Vec3f& binA = *((gmtl::Vec3f*) (pTempBin->GetData()+i1*sizeof(float)*3));
		gmtl::Vec3f& binB = *((gmtl::Vec3f*) (pTempBin->GetData()+i2*sizeof(float)*3));
		gmtl::Vec3f& binC = *((gmtl::Vec3f*) (pTempBin->GetData()+i3*sizeof(float)*3));

		gmtl::Vec3f tangent;
		gmtl::Vec3f binormal;
		ComputeTangentAndBinormal(posA, uvA, posB, uvB, posC, uvC, tangent, binormal);
		
		tanA += tangent;
		tanB += tangent;
		tanC += tangent;
		//gmtl::normalize(tanA);
		//gmtl::normalize(tanB);
		//gmtl::normalize(tanC);

		binA += binormal;
		binB += binormal;
		binC += binormal;
		//gmtl::normalize(binA);
		//gmtl::normalize(binB);
		//gmtl::normalize(binC);		
	}
	
	_LOOPi(numVertices)
	{
		gmtl::Vec3f& tan = *((gmtl::Vec3f*) (pTempTan->GetData()+i*sizeof(float)*3));
		gmtl::Vec3f& bin = *((gmtl::Vec3f*) (pTempBin->GetData()+i*sizeof(float)*3));

		//_DEBUG_ASSERT(gmtl::length(tan) >= 0.00001f);
		//_DEBUG_ASSERT(gmtl::length(bin) >= 0.00001f);
	
		gmtl::normalize(tan);
		gmtl::normalize(bin);
		
		//gmtl::Vec3f& tanDest = *((gmtl::Vec3f*)(pVertices+i*vertexSize+tangentOffset));
		//gmtl::Vec3f& binDest = *((gmtl::Vec3f*)(pVertices+i*vertexSize+binormalOffset));
		//
		//tanDest = tan;
		//binDest = bin;
	}
	
	byte* pTempTanData = pTempTan->GetData();
	byte* pTempBinData = pTempBin->GetData();
	
	// Average tangent and binormal per vertex based on proximity and similiarity of normals
	const static float distDiff = 0.01f;
	UniformLooseGrid grid;
	GetUniformGridVertices(pVertices + posOffset, vertexSize, numVertices, distDiff, grid);
	
	if(binormalOffset < vertexSize)
	{
		_LOOPi(numVertices)
		{
			byte* pCurVertex = pVertices+i*vertexSize;
		
			gmtl::Vec3f& pos = *((gmtl::Vec3f*)(pCurVertex+posOffset));
			gmtl::Vec3f& norm = *((gmtl::Vec3f*)(pCurVertex+normalOffset));
			gmtl::Vec3f& tan = *((gmtl::Vec3f*)(pCurVertex+tangentOffset));
			gmtl::Vec3f& bin = *((gmtl::Vec3f*)(pCurVertex+binormalOffset));
			tan[0] = 0.0f;
			tan[1] = 0.0f;
			tan[2] = 0.0f;
		
			bin[0] = 0.0f;
			bin[1] = 0.0f;
			bin[2] = 0.0f;
		
			//gmtl::Vec3f& tempTan = *((gmtl::Vec3f*) (pTempTan->GetData()+i*sizeof(float)*3));
			//gmtl::Vec3f& tempBin = *((gmtl::Vec3f*) (pTempBin->GetData()+i*sizeof(float)*3));

			UniformLooseGridCell* pCell = grid.GetCellFromPos(pos);
			_DEBUG_ASSERT(pCell);

			uint numPotentialVerts = pCell->pAllPoints->GetDataLength() / sizeof(uint);
			_LOOPj(numPotentialVerts)
			{
				uint* pPointData = (uint*) pCell->pAllPoints->GetData();
				uint pointIndex = pPointData[j];

				byte* pTempVertex = pVertices+pointIndex*vertexSize;
				gmtl::Vec3f& posT = *((gmtl::Vec3f*)(pTempVertex+posOffset));
				gmtl::Vec3f& normT = *((gmtl::Vec3f*)(pTempVertex+normalOffset));

				gmtl::Vec3f diff = posT - pos;
				float lengthSq = gmtl::lengthSquared(diff);
				if(lengthSq < (distDiff * distDiff) && gmtl::dot(normT, norm) >= 0.987f)
				{
					gmtl::Vec3f& tanT = *((gmtl::Vec3f*) (pTempTanData+pointIndex*sizeof(float)*3));//*((gmtl::Vec3f*)(pTempVertex+tangentOffset));
					gmtl::Vec3f& binT = *((gmtl::Vec3f*) (pTempBinData+pointIndex*sizeof(float)*3));//*((gmtl::Vec3f*)(pTempVertex+binormalOffset));
					tan += tanT;
					bin += binT;
				}
			}

			//_LOOPj(numVertices)
			//{
			//	//if(i != j)
			//	{
			//		byte* pTempVertex = pVertices+j*vertexSize;
			//		gmtl::Vec3f& posT = *((gmtl::Vec3f*)(pTempVertex+posOffset));
			//		gmtl::Vec3f& normT = *((gmtl::Vec3f*)(pTempVertex+normalOffset));

			//		gmtl::Vec3f diff = posT - pos;
			//		float lengthSq = gmtl::lengthSquared(diff);
			//		if(lengthSq < 0.00001f && gmtl::dot(normT, norm) >= 0.987f)
			//		{
			//			gmtl::Vec3f& tanT = *((gmtl::Vec3f*) (pTempTanData+j*sizeof(float)*3));//*((gmtl::Vec3f*)(pTempVertex+tangentOffset));
			//			gmtl::Vec3f& binT = *((gmtl::Vec3f*) (pTempBinData+j*sizeof(float)*3));//*((gmtl::Vec3f*)(pTempVertex+binormalOffset));
			//			tan += tanT;
			//			bin += binT;
			//		}
			//	}
			//}	
		
			gmtl::normalize(tan);
			gmtl::normalize(bin);	
		}
	}
	else
	{
		_LOOPi(numVertices)
		{
			byte* pCurVertex = pVertices+i*vertexSize;
		
			gmtl::Vec3f& pos = *((gmtl::Vec3f*)(pCurVertex+posOffset));
			gmtl::Vec3f& norm = *((gmtl::Vec3f*)(pCurVertex+normalOffset));
			gmtl::Vec3f& tan = *((gmtl::Vec3f*)(pCurVertex+tangentOffset));
			tan[0] = 0.0f;
			tan[1] = 0.0f;
			tan[2] = 0.0f;
		
			UniformLooseGridCell* pCell = grid.GetCellFromPos(pos);
			_DEBUG_ASSERT(pCell);

			uint numPotentialVerts = pCell->pAllPoints->GetDataLength() / sizeof(uint);
			_LOOPj(numPotentialVerts)
			{
				uint* pPointData = (uint*) pCell->pAllPoints->GetData();
				uint pointIndex = pPointData[j];

				byte* pTempVertex = pVertices+pointIndex*vertexSize;
				gmtl::Vec3f& posT = *((gmtl::Vec3f*)(pTempVertex+posOffset));
				gmtl::Vec3f& normT = *((gmtl::Vec3f*)(pTempVertex+normalOffset));

				gmtl::Vec3f diff = posT - pos;
				float lengthSq = gmtl::lengthSquared(diff);
				if(lengthSq < (distDiff * distDiff) && gmtl::dot(normT, norm) >= 0.987f)
				{
					gmtl::Vec3f& tanT = *((gmtl::Vec3f*) (pTempTanData+pointIndex*sizeof(float)*3));//*((gmtl::Vec3f*)(pTempVertex+tangentOffset));
					tan += tanT;
				}
			}

			gmtl::normalize(tan);
		}
	}
	
	//_LOOPi(numVertices)
	//{
	//	gmtl::Vec3f& tan = *((gmtl::Vec3f*)(pVertices+i*vertexSize+tangentOffset));
	//	gmtl::Vec3f& bin = *((gmtl::Vec3f*)(pVertices+i*vertexSize+binormalOffset));
	//	gmtl::Vec3f& tempTan = *((gmtl::Vec3f*) (pTempTan->GetData()+i*sizeof(float)*3));
	//	gmtl::Vec3f& tempBin = *((gmtl::Vec3f*) (pTempBin->GetData()+i*sizeof(float)*3));
	//	tan = tempTan;
	//	bin = tempBin;
	//	
	//	gmtl::normalize(tan);
	//	gmtl::normalize(bin);
	//}
}

void
ComputeTangentAndBinormals(IRVertexBuffer* pVB,
						   IRIndexBuffer* pIB,
						   uint posIndex,
						   uint texUVIndex,
						   uint normalIndex,
						   uint tangentIndex,
						   uint binormalIndex)
{
    // TODO: Will do a redundant copy since locking to read or write is now exclusive
    _DEBUG_ASSERT(FALSE);

	byte* pVertexData = pVB->Lock(pVB->GetStartVertex(), pVB->GetNumVertices());
	pVertexData += (pVB->GetStartVertex() * pVB->GetVertexSize());
	const RVertexDesc* pDesc = pVB->GetDescription();
	
	const ushort* pIndexData = pIB->Lock(0, 0);	// Readonly
	
	ComputeTangentAndBinormals(pVertexData, pVB->GetNumVertices(), pIndexData, pIB->GetNumIndices(),
								pDesc, posIndex, texUVIndex, normalIndex, tangentIndex, binormalIndex);
								
	pVB->Unlock(FALSE);
	pIB->Unlock(FALSE);
}

void GetIndicesInRangePerVertex(IRVertexBuffer* pVB, IByteBuffer* pInRange, IByteBuffer* pNumInRange, float rangeThreshold)
{
	uint numVertices = pVB->GetNumVertices();

	const RVertexDesc* pDesc = pVB->GetDescription();
	uint offset = GetVertexOffset(pDesc, VDU_POSITION, 0);
	uint vertSize = pVB->GetVertexSize();

	byte* pVBData = pVB->Lock(0, 0) + offset;

	UniformLooseGrid grid;
	if(rangeThreshold > 0.0f)
		GetUniformGridVertices(pVBData, vertSize, numVertices, rangeThreshold, grid);

	_LOOPi(numVertices)
	{
		uint numIndices = 0;
		
		gmtl::Vec3f pos = *((gmtl::Vec3f*)(pVBData + i * vertSize));
		UniformLooseGridCell* pCell = grid.GetCellFromPos(pos);
		_DEBUG_ASSERT(pCell);

		uint numPotentialVerts = pCell->pAllPoints->GetDataLength() / sizeof(uint);
		_LOOPj(numPotentialVerts)
		{
			uint* pPointData = (uint*) pCell->pAllPoints->GetData();
			uint pointIndex = pPointData[j];
			if(pointIndex != i)
			{
				gmtl::Vec3f pointPos = *((gmtl::Vec3f*)(pVBData + pointIndex * vertSize));
				gmtl::Vec3f diff = pos - pointPos;
				float lengthSq = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];
				if(lengthSq <= rangeThreshold * rangeThreshold)
				{
					AppendData(pInRange, pointIndex);
					numIndices++;
				}
			}
		}

		//_DEBUG_ASSERT(numIndices > 1);
		AppendData(pNumInRange, numIndices);
	}

	//_LOOPi(numVertices)
	//{
	//	uint numIndices = 0;
	//	
	//	gmtl::Vec3f pos = *((gmtl::Vec3f*)(pVBData + i * vertSize));
	//	_LOOPj(numVertices)
	//	{
	//		uint pointIndex = j;
	//		if(pointIndex != i)
	//		{
	//			gmtl::Vec3f pointPos = *((gmtl::Vec3f*)(pVBData + pointIndex * vertSize));
	//			gmtl::Vec3f diff = pos - pointPos;
	//			float lengthSq = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];
	//			if(lengthSq <= rangeThreshold * rangeThreshold)
	//			{
	//				AppendData(pInRange, pointIndex);
	//				numIndices++;
	//			}
	//		}
	//	}

	//	//_DEBUG_ASSERT(numIndices > 1);
	//	AppendData(pNumInRange, numIndices);
	//}


	pVB->Unlock(FALSE);

}

void GetAdjacentIndicesPerVertex(IRVertexBuffer* pVB, IRIndexBuffer* pIB, IByteBuffer* pAdjacent, IByteBuffer* pNumAdjacent, float sharedSurfaceDistThreshold)
{
	uint numVertices = pVB->GetNumVertices();

	const uint MAX_TRIANGLES_PER_VERT = 256;
	struct VertexSlot
	{
		uint indices[MAX_TRIANGLES_PER_VERT * 3];
		uint numIndices;
	};

	VertexSlot* pSlots = _NEW VertexSlot[numVertices];
	_LOOPi(numVertices)
	{
		pSlots[i].numIndices = 0;
	}

	ushort* pIndices = pIB->Lock(0, 0);
	
	_LOOPi(pIB->GetNumIndices() / 3)
	{
		uint p1 = pIndices[i*3];
		uint p2 = pIndices[i*3+1];
		uint p3 = pIndices[i*3+2];

		_DEBUG_ASSERT(pSlots[p1].numIndices < MAX_TRIANGLES_PER_VERT * 3);
		pSlots[p1].indices[pSlots[p1].numIndices] = p2;
		pSlots[p1].numIndices++;
		pSlots[p1].indices[pSlots[p1].numIndices] = p3;
		pSlots[p1].numIndices++;
	
		_DEBUG_ASSERT(pSlots[p2].numIndices < MAX_TRIANGLES_PER_VERT * 3);
		pSlots[p2].indices[pSlots[p2].numIndices] = p1;
		pSlots[p2].numIndices++;
		pSlots[p2].indices[pSlots[p2].numIndices] = p3;
		pSlots[p2].numIndices++;

		_DEBUG_ASSERT(pSlots[p3].numIndices < MAX_TRIANGLES_PER_VERT * 3);
		pSlots[p3].indices[pSlots[p3].numIndices] = p1;
		pSlots[p3].numIndices++;
		pSlots[p3].indices[pSlots[p3].numIndices] = p2;
		pSlots[p3].numIndices++;
	}

	pIB->Unlock();

	const RVertexDesc* pDesc = pVB->GetDescription();
	uint offset = GetVertexOffset(pDesc, VDU_POSITION, 0);
	uint vertSize = pVB->GetVertexSize();

	byte* pVBData = pVB->Lock(0, 0) + offset;
	
	UniformLooseGrid grid;
	if(sharedSurfaceDistThreshold > 0.0f)
		GetUniformGridVertices(pVBData, vertSize, numVertices, sharedSurfaceDistThreshold, grid);

	_LOOPi(numVertices)
	{
		_LOOPj(pSlots[i].numIndices)
		{
			AppendData(pAdjacent, pSlots[i].indices[j]);
		}

		// Now find adjacent vertices that may not be in a shared triangle (but is in close proximity)
		// TODO: Checks on normal too?
		uint numSharedIndices = 0;
		if(sharedSurfaceDistThreshold > 0.0f)
		{
			gmtl::Vec3f pos = *((gmtl::Vec3f*)(pVBData + i * vertSize));
			UniformLooseGridCell* pCell = grid.GetCellFromPos(pos);
			_DEBUG_ASSERT(pCell);

			uint numPotentialSharedVerts = pCell->pAllPoints->GetDataLength() / sizeof(uint);
			_LOOPj(numPotentialSharedVerts)
			{
				uint* pSharedData = (uint*) pCell->pAllPoints->GetData();
				uint sharedIndex = pSharedData[j];
				if(sharedIndex != i)
				{
					gmtl::Vec3f sharedPos = *((gmtl::Vec3f*)(pVBData + sharedIndex * vertSize));
					gmtl::Vec3f diff = pos - sharedPos;
					float lengthSq = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];
					if(lengthSq <= sharedSurfaceDistThreshold * sharedSurfaceDistThreshold)
					{
						_LOOPk(pSlots[sharedIndex].numIndices)
						{
							AppendData(pAdjacent, pSlots[sharedIndex].indices[k]);
						}

						numSharedIndices += pSlots[sharedIndex].numIndices;
					}
				}
			}
		}

		AppendData(pNumAdjacent, pSlots[i].numIndices + numSharedIndices);
	}

	pVB->Unlock(FALSE);

	_DELETE_ARRAY(pSlots);
}


_NAMESPACE_END