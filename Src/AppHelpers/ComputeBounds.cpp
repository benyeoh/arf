//==============================================================================
//
//		ComputeBounds.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/27/2008
//
//==============================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

//void 
//ComputeBounds(AHSceneObject& obj)
//{
//	// Reset bounds
//	AABox box;
//	box.min = gmtl::VecA3f(99999999999.0f, 99999999999.0f, 99999999999.0f);
//	box.max = gmtl::VecA3f(-99999999999.0f, -99999999999.0f, -99999999999.0f);
//	
//	obj.SetLocalAABox(box);
//	
//	uint numMeshes = obj.GetNumOfRenderMeshes();
//	_LOOPi(numMeshes)
//	{
//		BFXMesh& mesh = obj.GetRenderMesh(i);
//		uint numVBs = mesh.pVBGroup->GetNumOfVertexBuffers();
//		uint curVBIndex = 0;
//		uint offset = 0;
//		uint vertSize = 0;
//		IRVertexBuffer* pVB = NULL;
//		while(curVBIndex < numVBs)
//		{
//			pVB = mesh.pVBGroup->GetVertexBuffer(curVBIndex);
//			const RVertexDesc* pDesc = pVB->GetDescription();
//			offset = GetVertexOffset(pDesc, VDU_POSITION, 0);
//			vertSize = GetVertexSize(pDesc);
//			if(offset < vertSize)
//				break;
//				
//			curVBIndex++;	
//		}
//		
//		// This should ALWAYS be the case
//		_DEBUG_ASSERT(curVBIndex < numVBs);
//		_DEBUG_ASSERT((offset + sizeof(float))< vertSize);
//		
//		// Compute the bounds based on the vertex cloud
//		uint numVertices = pVB->GetNumVertices();
//		byte* pData = pVB->Lock(0, 0);
//		
//		PointsToAABox((gmtl::Vec4f*) pData, numVertices, vertSize, &box);
//		const AABox& objBox = obj.GetLocalAABox();
//		AABoxMerge(&box, &box, &objBox );
//		obj.SetLocalAABox(box);	
//	}
//}

void
ComputeBounds(BFXUMeshList& meshList, AABox& toStore)
{
	// Reset bounds
	AABox box;
	box.min = gmtl::VecA3f(99999999999.0f, 99999999999.0f, 99999999999.0f);
	box.max = gmtl::VecA3f(-99999999999.0f, -99999999999.0f, -99999999999.0f);
	
	toStore = box;
	
	uint numMeshes = (uint) meshList.meshes.size();
	_LOOPi(numMeshes)
	{
		BFXMesh& mesh = meshList.meshes[i];
		uint numVBs = mesh.pVBGroup->GetNumOfVertexBuffers();
		uint curVBIndex = 0;
		uint offset = 0;
		uint vertSize = 0;
		IRVertexBuffer* pVB = NULL;
		while(curVBIndex < numVBs)
		{
			pVB = mesh.pVBGroup->GetVertexBuffer(curVBIndex);
			const RVertexDesc* pDesc = pVB->GetDescription();
			offset = GetVertexOffset(pDesc, VDU_POSITION, 0);
			vertSize = GetVertexSize(pDesc);
			if(offset < vertSize)
				break;
				
			curVBIndex++;	
		}
		
		// This should ALWAYS be the case
		_DEBUG_ASSERT(curVBIndex < numVBs);
		_DEBUG_ASSERT((offset + sizeof(float))< vertSize);
		
		// Compute the bounds based on the vertex cloud
		uint numVertices = pVB->GetNumVertices();
		byte* pData = pVB->Lock(0, 0);
		
		PointsToAABox((gmtl::Vec4f*) pData, numVertices, vertSize, &box);
		AABoxMerge(&toStore, &box, &toStore);
	}
}

_NAMESPACE_END