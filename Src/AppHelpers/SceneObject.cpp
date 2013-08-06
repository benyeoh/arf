//==============================================================================
//
//		SceneObject.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/10/2007
//
//==============================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

void
SceneObject::Render(IRFXRenderFX* pRenderFX)
{
	uint numObjects = (uint) m_RenderMeshes.size();
	
	_LOOPi(numObjects)
	{
		pRenderFX->GetRFXCommon().SetWorldMatrix(*(gmtl::Matrix44f*)(&m_WorldMatrix));
		pRenderFX->GetRFXCommon().SetZDistanceFromCamera(m_ZDistance);
		
		// TODO: Can optimize this portion to set only once per frame
		// or can be set from an external source
		//gmtl::Vec3f diffPos = pRenderFX->GetRFXCommon().GetCameraPos();
		//const gmtl::Matrix44f& viewMat = pRenderFX->GetRFXCommon().GetViewMatrix();
		//const float* pPos = m_WorldMatrix.getData() + 4 * 3;
		//
		//diffPos[0] -= pPos[0];
		//diffPos[1] -= pPos[1];
		//diffPos[2] -= pPos[2];

		pRenderFX->RenderFX(&(m_RenderMeshes[i]), 1.0f, RFX_LIGHTING_ONLY);
	}
}

void 
SceneObject::UpdateWorldBounds()
{
	
}
	
_NAMESPACE_END