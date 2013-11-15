//================================================================================
//
//		BakedSMMeshCreate.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2012
//
//================================================================================

#include "stdafx.h"

BFXMesh CreatePlanePosY(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX, uint numSubZ)
{
    _DEBUG_ASSERT(numSubX > 0);
    _DEBUG_ASSERT(numSubZ > 0);

    float stepX = sizeX / numSubX;
    float stepZ = sizeZ / numSubZ;
    
    float curX = -sizeX * 0.5f;
    float curZ = -sizeZ * 0.5f;

    float stepU = 1.0f / numSubX;
    float stepV = 1.0f / numSubZ;

    float curU = 0.0f;
    float curV = 0.0f;

    uint numVerticesX = numSubX + 1;
    uint numVerticesZ = numSubZ + 1;
    uint numVertices = numVerticesX * numVerticesZ;

    uint numIndices = numSubX * numSubZ * 6;    

    IRIndexBufferPtr pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, numIndices, BUF_DEFAULT);
    ushort* pIBData = pIB->Lock(0, numIndices);
    _LOOPi(numSubZ)
    {
        ushort curRowIndex  = i * numVerticesX;
        ushort nextRowIndex = (i+1) * numVerticesX;

        _LOOPj(numSubX)
        {
            uint srcIndex = (i * numSubX + j) * 6;
            pIBData[srcIndex]   = curRowIndex+j;
            pIBData[srcIndex+1] = curRowIndex+j+1;
            pIBData[srcIndex+2] = nextRowIndex+j+1;

            pIBData[srcIndex+3] = curRowIndex+j;
            pIBData[srcIndex+4] = nextRowIndex+j+1;
            pIBData[srcIndex+5] = nextRowIndex+j;
        }
    }
    
    pIB->Unlock();

    RVertexDesc vbDesc[] = 
    {
        { VDU_POSITION, 0 },
        { VDU_NORMAL, 0 },
        { VDU_TEXCOORDF2, 0 },
        { VDU_TANGENT, 0 },
        { VDU_BINORMAL, 0 },
        { VDU_END, 0 },
    };

    IRVertexBufferPtr pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, vbDesc, numVertices, BUF_DEFAULT);
    uint posOffset      = GetVertexOffset(vbDesc, VDU_POSITION, 0);
    uint normalOffset   = GetVertexOffset(vbDesc, VDU_NORMAL, 0);
    uint texCoordOffset = GetVertexOffset(vbDesc, VDU_TEXCOORDF2, 0);
    uint tangentOffset  = GetVertexOffset(vbDesc, VDU_TANGENT, 0);
    uint binormalOffset = GetVertexOffset(vbDesc, VDU_BINORMAL, 0);

    byte* pVBData = pVB->Lock(0, numVertices);

    _LOOPi(numVerticesZ)
    {
        _LOOPj(numVerticesX)
        {
            float* pPos = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + posOffset);
            pPos[0] = curX;
            pPos[1] = 0.0f;
            pPos[2] = curZ;

            float* pNormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + normalOffset);
            pNormal[0] = 0.0f;
            pNormal[1] = 1.0f;
            pNormal[2] = 0.0f;

            float* pTexCoord = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + texCoordOffset);
            pTexCoord[0] = curU;
            pTexCoord[1] = curV;

            float* pTangent = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + tangentOffset);
            pTangent[0] = 1.0f;
            pTangent[1] = 0.0f;
            pTangent[2] = 0.0f;

            float* pBinormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + binormalOffset);
            pBinormal[0] = 0.0f;
            pBinormal[1] = 0.0f;
            pBinormal[2] = 1.0f;

            curX += stepX;
            curU += stepU;
        }

        curX = -sizeX * 0.5f;
        curZ += stepZ;
        curV += stepV;
    }

    pVB->Unlock();

    IRVertexBufferGroupPtr pVBGroup = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
    pVBGroup->AddVertexBuffer(pVB);

    BFXMesh toReturn;
    toReturn.pVBGroup   = pVBGroup;
    toReturn.pIB        = pIB;

    // No material...
    return toReturn;
}

BFXMesh CreatePlaneNegY(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX, uint numSubZ)
{
    _DEBUG_ASSERT(numSubX > 0);
    _DEBUG_ASSERT(numSubZ > 0);

    float stepX = sizeX / numSubX;
    float stepZ = -sizeZ / numSubZ;

    float origX = -sizeX * 0.5f;
    float curX = origX;
    float curZ = sizeZ * 0.5f;

    float stepU = 1.0f / numSubX;
    float stepV = 1.0f / numSubZ;

    float curU = 0.0f;
    float curV = 0.0f;

    uint numVerticesX = numSubX + 1;
    uint numVerticesZ = numSubZ + 1;
    uint numVertices = numVerticesX * numVerticesZ;

    uint numIndices = numSubX * numSubZ * 6;    

    IRIndexBufferPtr pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, numIndices, BUF_DEFAULT);
    ushort* pIBData = pIB->Lock(0, numIndices);
    _LOOPi(numSubZ)
    {
        ushort curRowIndex  = i * numVerticesX;
        ushort nextRowIndex = (i+1) * numVerticesX;

        _LOOPj(numSubX)
        {
            uint srcIndex = (i * numSubX + j) * 6;
            pIBData[srcIndex]   = curRowIndex+j;
            pIBData[srcIndex+1] = curRowIndex+j+1;
            pIBData[srcIndex+2] = nextRowIndex+j+1;

            pIBData[srcIndex+3] = curRowIndex+j;
            pIBData[srcIndex+4] = nextRowIndex+j+1;
            pIBData[srcIndex+5] = nextRowIndex+j;
        }
    }

    pIB->Unlock();

    RVertexDesc vbDesc[] = 
    {
        { VDU_POSITION, 0 },
        { VDU_NORMAL, 0 },
        { VDU_TEXCOORDF2, 0 },
        { VDU_TANGENT, 0 },
        { VDU_BINORMAL, 0 },
        { VDU_END, 0 },
    };

    IRVertexBufferPtr pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, vbDesc, numVertices, BUF_DEFAULT);
    uint posOffset      = GetVertexOffset(vbDesc, VDU_POSITION, 0);
    uint normalOffset   = GetVertexOffset(vbDesc, VDU_NORMAL, 0);
    uint texCoordOffset = GetVertexOffset(vbDesc, VDU_TEXCOORDF2, 0);
    uint tangentOffset  = GetVertexOffset(vbDesc, VDU_TANGENT, 0);
    uint binormalOffset = GetVertexOffset(vbDesc, VDU_BINORMAL, 0);

    byte* pVBData = pVB->Lock(0, numVertices);

    _LOOPi(numVerticesZ)
    {
        _LOOPj(numVerticesX)
        {
            float* pPos = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + posOffset);
            pPos[0] = curX;
            pPos[1] = 0.0f;
            pPos[2] = curZ;

            float* pNormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + normalOffset);
            pNormal[0] = 0.0f;
            pNormal[1] = -1.0f;
            pNormal[2] = 0.0f;

            float* pTexCoord = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + texCoordOffset);
            pTexCoord[0] = curU;
            pTexCoord[1] = curV;

            float* pTangent = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + tangentOffset);
            pTangent[0] = 1.0f;
            pTangent[1] = 0.0f;
            pTangent[2] = 0.0f;

            float* pBinormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + binormalOffset);
            pBinormal[0] = 0.0f;
            pBinormal[1] = 0.0f;
            pBinormal[2] = -1.0f;

            curX += stepX;
            curU += stepU;
        }

        curX = origX;
        curZ += stepZ;
        curV += stepV;
    }

    pVB->Unlock();

    IRVertexBufferGroupPtr pVBGroup = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
    pVBGroup->AddVertexBuffer(pVB);

    BFXMesh toReturn;
    toReturn.pVBGroup   = pVBGroup;
    toReturn.pIB        = pIB;

    // No material...
    return toReturn;
}

BFXMesh CreatePlanePosZ(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX, uint numSubZ)
{
    _DEBUG_ASSERT(numSubX > 0);
    _DEBUG_ASSERT(numSubZ > 0);

    float stepX = sizeX / numSubX;
    float stepZ = -sizeZ / numSubZ;

    float origX = -sizeX * 0.5f;
    float curX = origX;
    float curZ = sizeZ * 0.5f;

    float stepU = 1.0f / numSubX;
    float stepV = 1.0f / numSubZ;

    float curU = 0.0f;
    float curV = 0.0f;

    uint numVerticesX = numSubX + 1;
    uint numVerticesZ = numSubZ + 1;
    uint numVertices = numVerticesX * numVerticesZ;

    uint numIndices = numSubX * numSubZ * 6;    

    IRIndexBufferPtr pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, numIndices, BUF_DEFAULT);
    ushort* pIBData = pIB->Lock(0, numIndices);
    _LOOPi(numSubZ)
    {
        ushort curRowIndex  = i * numVerticesX;
        ushort nextRowIndex = (i+1) * numVerticesX;

        _LOOPj(numSubX)
        {
            uint srcIndex = (i * numSubX + j) * 6;
            pIBData[srcIndex]   = curRowIndex+j;
            pIBData[srcIndex+1] = curRowIndex+j+1;
            pIBData[srcIndex+2] = nextRowIndex+j+1;

            pIBData[srcIndex+3] = curRowIndex+j;
            pIBData[srcIndex+4] = nextRowIndex+j+1;
            pIBData[srcIndex+5] = nextRowIndex+j;
        }
    }

    pIB->Unlock();

    RVertexDesc vbDesc[] = 
    {
        { VDU_POSITION, 0 },
        { VDU_NORMAL, 0 },
        { VDU_TEXCOORDF2, 0 },
        { VDU_TANGENT, 0 },
        { VDU_BINORMAL, 0 },
        { VDU_END, 0 },
    };

    IRVertexBufferPtr pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, vbDesc, numVertices, BUF_DEFAULT);
    uint posOffset      = GetVertexOffset(vbDesc, VDU_POSITION, 0);
    uint normalOffset   = GetVertexOffset(vbDesc, VDU_NORMAL, 0);
    uint texCoordOffset = GetVertexOffset(vbDesc, VDU_TEXCOORDF2, 0);
    uint tangentOffset  = GetVertexOffset(vbDesc, VDU_TANGENT, 0);
    uint binormalOffset = GetVertexOffset(vbDesc, VDU_BINORMAL, 0);

    byte* pVBData = pVB->Lock(0, numVertices);

    _LOOPi(numVerticesZ)
    {
        _LOOPj(numVerticesX)
        {
            float* pPos = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + posOffset);
            pPos[0] = curX;
            pPos[1] = curZ;
            pPos[2] = 0.0f;

            float* pNormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + normalOffset);
            pNormal[0] = 0.0f;
            pNormal[1] = 0.0f;
            pNormal[2] = 1.0f;

            float* pTexCoord = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + texCoordOffset);
            pTexCoord[0] = curU;
            pTexCoord[1] = curV;

            float* pTangent = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + tangentOffset);
            pTangent[0] = 1.0f;
            pTangent[1] = 0.0f;
            pTangent[2] = 0.0f;

            float* pBinormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + binormalOffset);
            pBinormal[0] = 0.0f;
            pBinormal[1] = -1.0f;
            pBinormal[2] = 0.0f;

            curX += stepX;
            curU += stepU;
        }

        curX = origX;
        curZ += stepZ;
        curV += stepV;
    }

    pVB->Unlock();

    IRVertexBufferGroupPtr pVBGroup = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
    pVBGroup->AddVertexBuffer(pVB);

    BFXMesh toReturn;
    toReturn.pVBGroup   = pVBGroup;
    toReturn.pIB        = pIB;

    // No material...
    return toReturn;
}

BFXMesh CreatePlaneNegZ(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX, uint numSubZ)
{
    _DEBUG_ASSERT(numSubX > 0);
    _DEBUG_ASSERT(numSubZ > 0);

    float stepX = -sizeX / numSubX;
    float stepZ = -sizeZ / numSubZ;

    float origX = sizeX * 0.5f;
    float curX = origX;
    float curZ = sizeZ * 0.5f;
    
    float stepU = 1.0f / numSubX;
    float stepV = 1.0f / numSubZ;

    float curU = 0.0f;
    float curV = 0.0f;

    uint numVerticesX = numSubX + 1;
    uint numVerticesZ = numSubZ + 1;
    uint numVertices = numVerticesX * numVerticesZ;

    uint numIndices = numSubX * numSubZ * 6;    

    IRIndexBufferPtr pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, numIndices, BUF_DEFAULT);
    ushort* pIBData = pIB->Lock(0, numIndices);
    _LOOPi(numSubZ)
    {
        ushort curRowIndex  = i * numVerticesX;
        ushort nextRowIndex = (i+1) * numVerticesX;

        _LOOPj(numSubX)
        {
            uint srcIndex = (i * numSubX + j) * 6;
            pIBData[srcIndex]   = curRowIndex+j;
            pIBData[srcIndex+1] = curRowIndex+j+1;
            pIBData[srcIndex+2] = nextRowIndex+j+1;

            pIBData[srcIndex+3] = curRowIndex+j;
            pIBData[srcIndex+4] = nextRowIndex+j+1;
            pIBData[srcIndex+5] = nextRowIndex+j;
        }
    }

    pIB->Unlock();

    RVertexDesc vbDesc[] = 
    {
        { VDU_POSITION, 0 },
        { VDU_NORMAL, 0 },
        { VDU_TEXCOORDF2, 0 },
        { VDU_TANGENT, 0 },
        { VDU_BINORMAL, 0 },
        { VDU_END, 0 },
    };

    IRVertexBufferPtr pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, vbDesc, numVertices, BUF_DEFAULT);
    uint posOffset      = GetVertexOffset(vbDesc, VDU_POSITION, 0);
    uint normalOffset   = GetVertexOffset(vbDesc, VDU_NORMAL, 0);
    uint texCoordOffset = GetVertexOffset(vbDesc, VDU_TEXCOORDF2, 0);
    uint tangentOffset  = GetVertexOffset(vbDesc, VDU_TANGENT, 0);
    uint binormalOffset = GetVertexOffset(vbDesc, VDU_BINORMAL, 0);

    byte* pVBData = pVB->Lock(0, numVertices);

    _LOOPi(numVerticesZ)
    {
        _LOOPj(numVerticesX)
        {
            float* pPos = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + posOffset);
            pPos[0] = curX;
            pPos[1] = curZ;
            pPos[2] = 0.0f;

            float* pNormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + normalOffset);
            pNormal[0] = 0.0f;
            pNormal[1] = 0.0f;
            pNormal[2] = -1.0f;

            float* pTexCoord = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + texCoordOffset);
            pTexCoord[0] = curU;
            pTexCoord[1] = curV;

            float* pTangent = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + tangentOffset);
            pTangent[0] = -1.0f;
            pTangent[1] = 0.0f;
            pTangent[2] = 0.0f;

            float* pBinormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + binormalOffset);
            pBinormal[0] = 0.0f;
            pBinormal[1] = -1.0f;
            pBinormal[2] = 0.0f;

            curX += stepX;
            curU += stepU;
        }

        curX = origX;
        curZ += stepZ;
        curV += stepV;
    }

    pVB->Unlock();

    IRVertexBufferGroupPtr pVBGroup = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
    pVBGroup->AddVertexBuffer(pVB);

    BFXMesh toReturn;
    toReturn.pVBGroup   = pVBGroup;
    toReturn.pIB        = pIB;

    // No material...
    return toReturn;
}

BFXMesh CreatePlanePosX(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX, uint numSubZ)
{
    _DEBUG_ASSERT(numSubX > 0);
    _DEBUG_ASSERT(numSubZ > 0);

    float stepX = -sizeX / numSubX;
    float stepZ = -sizeZ / numSubZ;

    float origX = sizeX * 0.5f;
    float curX = origX;
    float curZ = sizeZ * 0.5f;

    float stepU = 1.0f / numSubX;
    float stepV = 1.0f / numSubZ;

    float curU = 0.0f;
    float curV = 0.0f;

    uint numVerticesX = numSubX + 1;
    uint numVerticesZ = numSubZ + 1;
    uint numVertices = numVerticesX * numVerticesZ;

    uint numIndices = numSubX * numSubZ * 6;    

    IRIndexBufferPtr pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, numIndices, BUF_DEFAULT);
    ushort* pIBData = pIB->Lock(0, numIndices);
    _LOOPi(numSubZ)
    {
        ushort curRowIndex  = i * numVerticesX;
        ushort nextRowIndex = (i+1) * numVerticesX;

        _LOOPj(numSubX)
        {
            uint srcIndex = (i * numSubX + j) * 6;
            pIBData[srcIndex]   = curRowIndex+j;
            pIBData[srcIndex+1] = curRowIndex+j+1;
            pIBData[srcIndex+2] = nextRowIndex+j+1;

            pIBData[srcIndex+3] = curRowIndex+j;
            pIBData[srcIndex+4] = nextRowIndex+j+1;
            pIBData[srcIndex+5] = nextRowIndex+j;
        }
    }

    pIB->Unlock();

    RVertexDesc vbDesc[] = 
    {
        { VDU_POSITION, 0 },
        { VDU_NORMAL, 0 },
        { VDU_TEXCOORDF2, 0 },
        { VDU_TANGENT, 0 },
        { VDU_BINORMAL, 0 },
        { VDU_END, 0 },
    };

    IRVertexBufferPtr pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, vbDesc, numVertices, BUF_DEFAULT);
    uint posOffset      = GetVertexOffset(vbDesc, VDU_POSITION, 0);
    uint normalOffset   = GetVertexOffset(vbDesc, VDU_NORMAL, 0);
    uint texCoordOffset = GetVertexOffset(vbDesc, VDU_TEXCOORDF2, 0);
    uint tangentOffset  = GetVertexOffset(vbDesc, VDU_TANGENT, 0);
    uint binormalOffset = GetVertexOffset(vbDesc, VDU_BINORMAL, 0);

    byte* pVBData = pVB->Lock(0, numVertices);

    _LOOPi(numVerticesZ)
    {
        _LOOPj(numVerticesX)
        {
            float* pPos = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + posOffset);
            pPos[0] = 0.0f;
            pPos[1] = curZ;
            pPos[2] = curX;

            float* pNormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + normalOffset);
            pNormal[0] = 1.0f;
            pNormal[1] = 0.0f;
            pNormal[2] = 0.0f;

            float* pTexCoord = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + texCoordOffset);
            pTexCoord[0] = curU;
            pTexCoord[1] = curV;

            float* pTangent = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + tangentOffset);
            pTangent[0] = 0.0f;
            pTangent[1] = 0.0f;
            pTangent[2] = -1.0f;

            float* pBinormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + binormalOffset);
            pBinormal[0] = 0.0f;
            pBinormal[1] = -1.0f;
            pBinormal[2] = 0.0f;

            curX += stepX;
            curU += stepU;
        }

        curX = origX;
        curZ += stepZ;
        curV += stepV;
    }

    pVB->Unlock();

    IRVertexBufferGroupPtr pVBGroup = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
    pVBGroup->AddVertexBuffer(pVB);

    BFXMesh toReturn;
    toReturn.pVBGroup   = pVBGroup;
    toReturn.pIB        = pIB;

    // No material...
    return toReturn;
}

BFXMesh CreatePlaneNegX(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX, uint numSubZ)
{
    _DEBUG_ASSERT(numSubX > 0);
    _DEBUG_ASSERT(numSubZ > 0);

    float stepX = sizeX / numSubX;
    float stepZ = -sizeZ / numSubZ;

    float origX = -sizeX * 0.5f;
    float curX = origX;
    float curZ = sizeZ * 0.5f;

    float stepU = 1.0f / numSubX;
    float stepV = 1.0f / numSubZ;

    float curU = 0.0f;
    float curV = 0.0f;

    uint numVerticesX = numSubX + 1;
    uint numVerticesZ = numSubZ + 1;
    uint numVertices = numVerticesX * numVerticesZ;

    uint numIndices = numSubX * numSubZ * 6;    

    IRIndexBufferPtr pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, numIndices, BUF_DEFAULT);
    ushort* pIBData = pIB->Lock(0, numIndices);
    _LOOPi(numSubZ)
    {
        ushort curRowIndex  = i * numVerticesX;
        ushort nextRowIndex = (i+1) * numVerticesX;

        _LOOPj(numSubX)
        {
            uint srcIndex = (i * numSubX + j) * 6;
            pIBData[srcIndex]   = curRowIndex+j;
            pIBData[srcIndex+1] = curRowIndex+j+1;
            pIBData[srcIndex+2] = nextRowIndex+j+1;

            pIBData[srcIndex+3] = curRowIndex+j;
            pIBData[srcIndex+4] = nextRowIndex+j+1;
            pIBData[srcIndex+5] = nextRowIndex+j;
        }
    }

    pIB->Unlock();

    RVertexDesc vbDesc[] = 
    {
        { VDU_POSITION, 0 },
        { VDU_NORMAL, 0 },
        { VDU_TEXCOORDF2, 0 },
        { VDU_TANGENT, 0 },
        { VDU_BINORMAL, 0 },
        { VDU_END, 0 },
    };

    IRVertexBufferPtr pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, vbDesc, numVertices, BUF_DEFAULT);
    uint posOffset      = GetVertexOffset(vbDesc, VDU_POSITION, 0);
    uint normalOffset   = GetVertexOffset(vbDesc, VDU_NORMAL, 0);
    uint texCoordOffset = GetVertexOffset(vbDesc, VDU_TEXCOORDF2, 0);
    uint tangentOffset  = GetVertexOffset(vbDesc, VDU_TANGENT, 0);
    uint binormalOffset = GetVertexOffset(vbDesc, VDU_BINORMAL, 0);

    byte* pVBData = pVB->Lock(0, numVertices);

    _LOOPi(numVerticesZ)
    {
        _LOOPj(numVerticesX)
        {
            float* pPos = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + posOffset);
            pPos[0] = 0.0f;
            pPos[1] = curZ;
            pPos[2] = curX;

            float* pNormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + normalOffset);
            pNormal[0] = -1.0f;
            pNormal[1] = 0.0f;
            pNormal[2] = 0.0f;

            float* pTexCoord = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + texCoordOffset);
            pTexCoord[0] = curU;
            pTexCoord[1] = curV;

            float* pTangent = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + tangentOffset);
            pTangent[0] = 0.0f;
            pTangent[1] = 0.0f;
            pTangent[2] = 1.0f;

            float* pBinormal = (float*)(pVBData + (i * numVerticesX + j) * pVB->GetVertexSize() + binormalOffset);
            pBinormal[0] = 0.0f;
            pBinormal[1] = -1.0f;
            pBinormal[2] = 0.0f;

            curX += stepX;
            curU += stepU;
        }

        curX = origX;
        curZ += stepZ;
        curV += stepV;
    }

    pVB->Unlock();

    IRVertexBufferGroupPtr pVBGroup = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
    pVBGroup->AddVertexBuffer(pVB);

    BFXMesh toReturn;
    toReturn.pVBGroup   = pVBGroup;
    toReturn.pIB        = pIB;

    // No material...
    return toReturn;
}

