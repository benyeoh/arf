//==============================================================================
//
//		CDFXResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/27/2008
//
//==============================================================================

#include "DebugFXFramework.h"

_NAMESPACE_BEGIN

//IDFXWireBox*
//CDFXResourceMgr::CreateWireBox(const gmtl::Vec3f& center, const gmtl::Vec3f& extents)
//{
//	CDFXWireBox* pWireBox = _NEW CDFXWireBox(m_pDebugFX, center, extents);
//	return pWireBox;
//}	

void 
CDFXResourceMgr::InitializeWireBoxData()
{
	// Initialize wirebox buffers
	RVertexDesc wireDesc[] =
	{
		{ VDU_POSITION, 0 },
		{ VDU_BLENDINDICES, 0 },
		{ VDU_END, 0 },
	};

	m_WireBox.pVB = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateVertexBuffer(NULL, wireDesc, DFX_MAX_NUM_OF_BATCHES * 8, BUF_DEFAULT);
	m_WireBox.pVBGroup = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateVertexBufferGroup(NULL);
	m_WireBox.pIB = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateIndexBuffer(NULL, DFX_MAX_NUM_OF_BATCHES * 24, BUF_DEFAULT);
	m_WireBox.pVBGroup->AddVertexBuffer(m_WireBox.pVB);

	byte* pWireData = m_WireBox.pVB->Lock(0, DFX_MAX_NUM_OF_BATCHES * 8);
	_LOOPi(DFX_MAX_NUM_OF_BATCHES)
	{
		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(-1.0f, 1.0f, -1.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);

		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(-1.0f, 1.0f, 1.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);

		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(1.0f, 1.0f, -1.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);

		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(1.0f, 1.0f, 1.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);


		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(-1.0f, -1.0f, -1.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);

		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(-1.0f, -1.0f, 1.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);

		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(1.0f, -1.0f, -1.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);

		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(1.0f, -1.0f, 1.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);
	}

	m_WireBox.pVB->Unlock();

	ushort* pWireIndices = m_WireBox.pIB->Lock(0, DFX_MAX_NUM_OF_BATCHES * 24);

	_LOOPi(DFX_MAX_NUM_OF_BATCHES)
	{
		*pWireIndices = (ushort) i*8;		pWireIndices++;
		*pWireIndices = (ushort) i*8+1;		pWireIndices++;
		*pWireIndices = (ushort) i*8;		pWireIndices++;
		*pWireIndices = (ushort) i*8+2;		pWireIndices++;
		*pWireIndices = (ushort) i*8+3;		pWireIndices++;
		*pWireIndices = (ushort) i*8+1;		pWireIndices++;		
		*pWireIndices = (ushort) i*8+3;		pWireIndices++;
		*pWireIndices = (ushort) i*8+2;		pWireIndices++;

		*pWireIndices = (ushort) i*8;		pWireIndices++;
		*pWireIndices = (ushort) i*8+4;		pWireIndices++;
		*pWireIndices = (ushort) i*8+1;		pWireIndices++;
		*pWireIndices = (ushort) i*8+5;		pWireIndices++;
		*pWireIndices = (ushort) i*8+2;		pWireIndices++;
		*pWireIndices = (ushort) i*8+6;		pWireIndices++;		
		*pWireIndices = (ushort) i*8+3;		pWireIndices++;
		*pWireIndices = (ushort) i*8+7;		pWireIndices++;

		*pWireIndices = (ushort) i*8+4;		pWireIndices++;
		*pWireIndices = (ushort) i*8+5;		pWireIndices++;
		*pWireIndices = (ushort) i*8+4;		pWireIndices++;
		*pWireIndices = (ushort) i*8+6;		pWireIndices++;
		*pWireIndices = (ushort) i*8+7;		pWireIndices++;
		*pWireIndices = (ushort) i*8+5;		pWireIndices++;		
		*pWireIndices = (ushort) i*8+7;		pWireIndices++;
		*pWireIndices = (ushort) i*8+6;		pWireIndices++;
	}

	m_WireBox.pIB->Unlock();
	
	wstring path = _W("[shd]\\DebugFX\\DebugBasic.fx");
	IByteBufferPtr pBuffer = m_pDebugFX->GetAppCallback().GetFileData(path.c_str());
	
	CRCDataKey bufferID;
	bufferID.Set(path.c_str(), (uint) path.size());
	m_WireBox.pEffect = m_pDebugFX->GetRenderer().GetRResourceMgr().LoadEffect(&bufferID, pBuffer, NULL, NULL, 0);
}

void 
CDFXResourceMgr::InitializeWireSphereData()
{
	// Initialize wirebox buffers
	RVertexDesc wireDesc[] =
	{
		{ VDU_POSITION, 0 },
		{ VDU_BLENDINDICES, 0 },
		{ VDU_END, 0 },
	};

	m_WireSphere.pVB = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateVertexBuffer(NULL, wireDesc, 
		DFX_MAX_NUM_OF_BATCHES * 3 * DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE, BUF_DEFAULT);
	m_WireSphere.pVBGroup = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateVertexBufferGroup(NULL);
	m_WireSphere.pIB = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateIndexBuffer(NULL, 
		DFX_MAX_NUM_OF_BATCHES * 6 * DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE, BUF_DEFAULT);
	m_WireSphere.pVBGroup->AddVertexBuffer(m_WireSphere.pVB);

	byte* pWireData = m_WireSphere.pVB->Lock(0, DFX_MAX_NUM_OF_BATCHES * 3 * DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE);
	_LOOPi(DFX_MAX_NUM_OF_BATCHES)
	{
		// X arcs
		_LOOPj(DFX_NUM_OF_CIRCLES_PER_ARC)
		{
			float rotAmountZ = j * gmtl::Math::PI / DFX_NUM_OF_CIRCLES_PER_ARC;
			gmtl::EulerAngleZXYf rotEulerZ;
			rotEulerZ[0] = rotAmountZ;
			gmtl::Matrix33f rotZ;
			gmtl::setRot(rotZ, rotEulerZ);
			
			gmtl::Vec3f axisX;
			gmtl::xform(axisX, rotZ, gmtl::Vec3f(1.0f, 0.0f, 0.0f));
			//gmtl::xform(axisY, rotZ, gmtl::Vec3f(0.0f, 1.0f, 0.0f));
			
			_LOOPk(DFX_NUM_OF_VERTICES_PER_CIRCLE)
			{
				float rotAmountX = k * gmtl::Math::TWO_PI / DFX_NUM_OF_VERTICES_PER_CIRCLE;
				gmtl::AxisAnglef rotAxisX(rotAmountX, axisX);
				gmtl::Matrix33f rot;
				gmtl::setRot(rot, rotAxisX);
				
				gmtl::xform(*((gmtl::Vec3f*)pWireData), rot, gmtl::Vec3f(0.0f, 0.0f, 1.0f));
				pWireData += GetVertexDataSize(VDU_POSITION);
				*((byte*)pWireData)	= i;
				pWireData += GetVertexDataSize(VDU_BLENDINDICES);
			}
		}

		// Y arcs
		_LOOPj(DFX_NUM_OF_CIRCLES_PER_ARC)
		{
			float rotAmountX = j * gmtl::Math::PI / DFX_NUM_OF_CIRCLES_PER_ARC;
			gmtl::EulerAngleXYZf rotEulerX;
			rotEulerX[0] = rotAmountX;
			gmtl::Matrix33f rotX;
			gmtl::setRot(rotX, rotEulerX);

			gmtl::Vec3f axisY;
			gmtl::xform(axisY, rotX, gmtl::Vec3f(0.0f, 1.0f, 0.0f));
		
			_LOOPk(DFX_NUM_OF_VERTICES_PER_CIRCLE)
			{
				float rotAmountY = k * gmtl::Math::TWO_PI / DFX_NUM_OF_VERTICES_PER_CIRCLE;
				gmtl::AxisAnglef rotAxisY(rotAmountY, axisY);
				gmtl::Matrix33f rot;
				gmtl::setRot(rot, rotAxisY);

				gmtl::xform(*((gmtl::Vec3f*)pWireData), rot, gmtl::Vec3f(1.0f, 0.0f, 0.0f));
				pWireData += GetVertexDataSize(VDU_POSITION);
				*((byte*)pWireData)	= i;
				pWireData += GetVertexDataSize(VDU_BLENDINDICES);
			}
		}
		
		// Z arcs
		_LOOPj(DFX_NUM_OF_CIRCLES_PER_ARC)
		{
			float rotAmountY = j * gmtl::Math::PI / DFX_NUM_OF_CIRCLES_PER_ARC;
			gmtl::EulerAngleXYZf rotEulerY;
			rotEulerY[1] = rotAmountY;
			gmtl::Matrix33f rotY;
			gmtl::setRot(rotY, rotEulerY);

			gmtl::Vec3f axisZ;
			gmtl::xform(axisZ, rotY, gmtl::Vec3f(0.0f, 0.0f, 1.0f));

			_LOOPk(DFX_NUM_OF_VERTICES_PER_CIRCLE)
			{
				float rotAmountZ = k * gmtl::Math::TWO_PI / DFX_NUM_OF_VERTICES_PER_CIRCLE;
				gmtl::AxisAnglef rotAxisZ(rotAmountZ, axisZ);
				gmtl::Matrix33f rot;
				gmtl::setRot(rot, rotAxisZ);

				gmtl::xform(*((gmtl::Vec3f*)pWireData), rot, gmtl::Vec3f(0.0f, 1.0f, 0.0f));
				pWireData += GetVertexDataSize(VDU_POSITION);
				*((byte*)pWireData)	= i;
				pWireData += GetVertexDataSize(VDU_BLENDINDICES);
			}
		}
	}

	m_WireSphere.pVB->Unlock();

	ushort* pWireIndices = m_WireSphere.pIB->Lock(0, DFX_MAX_NUM_OF_BATCHES * 6 * DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE);

	_LOOPi(DFX_MAX_NUM_OF_BATCHES)
	{
		// X arcs
		_LOOPj(DFX_NUM_OF_CIRCLES_PER_ARC)
		{
			uint startIndex = i * DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE * 3 + j * DFX_NUM_OF_VERTICES_PER_CIRCLE;

			_LOOPk(DFX_NUM_OF_VERTICES_PER_CIRCLE)
			{
				ushort nextIndex = (k+1) >= DFX_NUM_OF_VERTICES_PER_CIRCLE ? 0 : k+1; 
				*pWireIndices = (ushort) startIndex + k;			pWireIndices++;
				*pWireIndices = (ushort) startIndex + nextIndex;	pWireIndices++;
			}
		}
		
		// Y arcs
		_LOOPj(DFX_NUM_OF_CIRCLES_PER_ARC)
		{
			uint startIndex = i * DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE * 3 
				+ DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE
				+ j * DFX_NUM_OF_VERTICES_PER_CIRCLE;

			_LOOPk(DFX_NUM_OF_VERTICES_PER_CIRCLE)
			{
				ushort nextIndex = (k+1) >= DFX_NUM_OF_VERTICES_PER_CIRCLE ? 0 : k+1; 
				*pWireIndices = (ushort) startIndex + k;			pWireIndices++;
				*pWireIndices = (ushort) startIndex + nextIndex;	pWireIndices++;
			}
		}
		
		// Z arcs
		_LOOPj(DFX_NUM_OF_CIRCLES_PER_ARC)
		{
			uint startIndex = i * DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE * 3 
				+ DFX_NUM_OF_CIRCLES_PER_ARC * DFX_NUM_OF_VERTICES_PER_CIRCLE * 2
				+ j * DFX_NUM_OF_VERTICES_PER_CIRCLE;

			_LOOPk(DFX_NUM_OF_VERTICES_PER_CIRCLE)
			{
				ushort nextIndex = (k+1) >= DFX_NUM_OF_VERTICES_PER_CIRCLE ? 0 : k+1; 
				*pWireIndices = (ushort) startIndex + k;			pWireIndices++;
				*pWireIndices = (ushort) startIndex + nextIndex;	pWireIndices++;
			}
		}
	}

	m_WireSphere.pIB->Unlock();

	wstring path = _W("[shd]\\DebugFX\\DebugBasic.fx");
	IByteBufferPtr pBuffer = m_pDebugFX->GetAppCallback().GetFileData(path.c_str());

	CRCDataKey bufferID;
	bufferID.Set(path.c_str(), (uint) path.size());
	m_WireSphere.pEffect = m_pDebugFX->GetRenderer().GetRResourceMgr().LoadEffect(&bufferID, pBuffer, NULL, NULL, 0);
}

void 
CDFXResourceMgr::InitializeLineData()
{
	// Initialize wirebox buffers
	RVertexDesc wireDesc[] =
	{
		{ VDU_POSITION, 0 },
		{ VDU_BLENDINDICES, 0 },
		{ VDU_END, 0 },
	};

	m_Lines.pVB = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateVertexBuffer(NULL, wireDesc, DFX_MAX_NUM_OF_BATCHES, BUF_DEFAULT);
	m_Lines.pVBGroup = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateVertexBufferGroup(NULL);
	m_Lines.pIB = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateIndexBuffer(NULL, DFX_MAX_NUM_OF_BATCHES * 2, BUF_DEFAULT);
	m_Lines.pVBGroup->AddVertexBuffer(m_Lines.pVB);

	byte* pWireData = m_Lines.pVB->Lock(0, DFX_MAX_NUM_OF_BATCHES);
	_LOOPi(DFX_MAX_NUM_OF_BATCHES)
	{
		*((gmtl::Vec3f*)pWireData) = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
		pWireData += GetVertexDataSize(VDU_POSITION);
		*((byte*)pWireData)	= i;
		pWireData += GetVertexDataSize(VDU_BLENDINDICES);
	}

	m_Lines.pVB->Unlock();

	ushort* pWireIndices = m_Lines.pIB->Lock(0, DFX_MAX_NUM_OF_BATCHES * 2);

	_LOOPi(DFX_MAX_NUM_OF_BATCHES)
	{
		*pWireIndices = (ushort) i*2;		pWireIndices++;
		*pWireIndices = (ushort) i*2+1;		pWireIndices++;
	}

	m_Lines.pIB->Unlock();

	wstring path = _W("[shd]\\DebugFX\\DebugBasicPoint.fx");
	IByteBufferPtr pBuffer = m_pDebugFX->GetAppCallback().GetFileData(path.c_str());

	CRCDataKey bufferID;
	bufferID.Set(path.c_str(), (uint) path.size());
	m_Lines.pEffect = m_pDebugFX->GetRenderer().GetRResourceMgr().LoadEffect(&bufferID, pBuffer, NULL, NULL, 0);	
}

void 
CDFXResourceMgr::InitializeQuadData()
{
	// Initialize quad buffers
	RVertexDesc quadDesc[] =
	{
		{ VDU_POSITION, 0 },
		{ VDU_TEXCOORDF2, 0 },
		{ VDU_BLENDINDICES, 0 },
		{ VDU_END, 0 },
	};

	m_Quads.pVB = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateVertexBuffer(NULL, quadDesc, DFX_MAX_NUM_OF_BATCHES * 4, BUF_DEFAULT);
	m_Quads.pVBGroup = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateVertexBufferGroup(NULL);
	m_Quads.pIB = m_pDebugFX->GetRenderer().GetRResourceMgr().CreateIndexBuffer(NULL, DFX_MAX_NUM_OF_BATCHES * 6, BUF_DEFAULT);
	m_Quads.pVBGroup->AddVertexBuffer(m_Quads.pVB);

	byte* pQuadData = m_Quads.pVB->Lock(0, DFX_MAX_NUM_OF_BATCHES*4);
	_LOOPi(DFX_MAX_NUM_OF_BATCHES*4)
	{
		float x = (float)(i % 2);
		float y = (float)(i / 2);
		
		float xPos = x * 2.0f - 1.0f;
		float yPos = y * 2.0f - 1.0f;
		
		*((gmtl::Vec3f*)pQuadData) = gmtl::Vec3f(xPos, yPos, 0.0f);
		pQuadData += GetVertexDataSize(VDU_POSITION);
		*((gmtl::Vec2f*)pQuadData) = gmtl::Vec2f(x, 1.0f - y);
		pQuadData += GetVertexDataSize(VDU_TEXCOORDF2);
		*((byte*)pQuadData)	= i / 4;
		pQuadData += GetVertexDataSize(VDU_BLENDINDICES);
	}

	m_Quads.pVB->Unlock();

	ushort* pQuadIndices = m_Quads.pIB->Lock(0, DFX_MAX_NUM_OF_BATCHES * 6);

	_LOOPi(DFX_MAX_NUM_OF_BATCHES)
	{
		*pQuadIndices = (ushort) i*6;		pQuadIndices++;
		*pQuadIndices = (ushort) i*6+2;		pQuadIndices++;
		*pQuadIndices = (ushort) i*6+3;		pQuadIndices++;
		*pQuadIndices = (ushort) i*6;		pQuadIndices++;
		*pQuadIndices = (ushort) i*6+3;		pQuadIndices++;
		*pQuadIndices = (ushort) i*6+1;		pQuadIndices++;
	}

	m_Quads.pIB->Unlock();

	wstring path = _W("[shd]\\DebugFX\\DebugTexturedQuad.fx");
	IByteBufferPtr pBuffer = m_pDebugFX->GetAppCallback().GetFileData(path.c_str());

	CRCDataKey bufferID;
	bufferID.Set(path.c_str(), (uint) path.size());
	m_Quads.pEffect = m_pDebugFX->GetRenderer().GetRResourceMgr().LoadEffect(&bufferID, pBuffer, NULL, NULL, 0);	
}
	
void 
CDFXResourceMgr::Initialize()
{
	InitializeWireBoxData();
	InitializeWireSphereData();
	InitializeLineData();
	InitializeQuadData();
}


_NAMESPACE_END