//==============================================================================
//
//		CDEFXParamPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#include "DeferredFXFramework.h"

_NAMESPACE_BEGIN

void 
CDEFXParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
	switch(semantic)
	{
		case DEFX_Z_POS_ACCUM:
		{
			pToStore->SetTexture2D(m_pZPos);
			break;
		}

		case DEFX_NORMALS_ACCUM:
		{
			pToStore->SetTexture2D(m_pNormals);
			break;
		}

		case DEFX_ALBEDO_ACCUM:
		{
			pToStore->SetTexture2D(m_pAlbedo);
			break;
		}

		case DEFX_FAR_PLANE_CORNER_VECS:
		{
			pToStore->SetValue(m_pFarPlaneCorners, sizeof(gmtl::VecA3f) * 4);
			break;
		}

		case DEFX_SCREEN_UV_OFFSET:
		{
			pToStore->SetVector2(GetScreenUVOffset());
			break;
		}

        //case DEFX_DOWNSAMPLE_2X_ZPOS:
        //{
        //    pToStore->SetTextureRT(m_pDownsample2XZPos);
        //    break;
        //}

        //case DEFX_DOWNSAMPLE_2X_NORMALS:
        //{
        //    pToStore->SetTextureRT(m_pDownsample2XNormals);
        //    break;
        //}

		case DEFX_FAR_PLANE_OFFSETS:
		{
			pToStore->SetValue(GetFarPlaneOffset(), sizeof(FarPlaneOffsets));
			break;
		}

		case DEFX_POINT_LIGHT:
		{
			pToStore->SetValue(m_pPointLight, sizeof(DEFXPointLight));
			break;
		}

		case DEFX_VIEW_FAR_PLANE_OFFSETS:
		{
			pToStore->SetValue(m_ViewFarPlaneOffsetStack.GetLast(), sizeof(ViewFarPlaneOffsets));
			break;
		}

		case DEFX_DIR_LIGHT:
		{
			pToStore->SetValue(m_pDirLight, sizeof(DEFXDirLight));
			break;
		}

		default:	_DEBUG_ASSERT(FALSE); break;
	}
}

void CDEFXParamPool::SetViewFarPlaneOffsets(gmtl::Vec3f* pTopLeftCorner, float xRange, float yRange)
{
	ViewFarPlaneOffsets* pOffset = m_ViewFarPlaneOffsetStack.Get();
	pOffset->topLeft[0] = (*pTopLeftCorner)[0];
	pOffset->topLeft[1] = (*pTopLeftCorner)[1];
	pOffset->topLeft[2] = (*pTopLeftCorner)[2];
	pOffset->xy[0] = xRange;
	pOffset->xy[1] = yRange;
}

CDEFXParamPool::FarPlaneOffsets* CDEFXParamPool::GetFarPlaneOffset()
{
	if(m_IsFarPlaneOffsetDirty)
	{
		FarPlaneOffsets* pFarPlaneOffsets = m_FarPlaneOffsetStack.Get();
		pFarPlaneOffsets->offset[0][0] = m_pFarPlaneCorners[0][0];
		pFarPlaneOffsets->offset[0][1] = m_pFarPlaneCorners[0][1];
		pFarPlaneOffsets->offset[0][2] = m_pFarPlaneCorners[0][2];

		gmtl::VecA3f temp = m_pFarPlaneCorners[1] - m_pFarPlaneCorners[0];
		pFarPlaneOffsets->offset[1][0] = temp[0];
		pFarPlaneOffsets->offset[1][1] = temp[1];
		pFarPlaneOffsets->offset[1][2] = temp[2];

		temp = m_pFarPlaneCorners[2] - m_pFarPlaneCorners[0];
		pFarPlaneOffsets->offset[2][0] = temp[0];
		pFarPlaneOffsets->offset[2][1] = temp[1];
		pFarPlaneOffsets->offset[2][2] = temp[2];

		m_IsFarPlaneOffsetDirty = FALSE;
	}

	return m_FarPlaneOffsetStack.GetLast();
}

gmtl::Vec2f* CDEFXParamPool::GetScreenUVOffset()
{
	if(m_IsScreenUVOffsetDirty)
	{
		gmtl::Vec2f* pUV = m_ScreenUVOffsetPool.Get();
		float offset = m_pDeferredFX->GetRenderer().GetRendererInfo().pixelOffset;

		(*pUV)[0] = offset / m_pZPos->GetWidth(0);
		(*pUV)[1] = offset / m_pZPos->GetHeight(0);
		m_IsScreenUVOffsetDirty = FALSE;
	}

	return m_ScreenUVOffsetPool.GetLast();
}

void 
CDEFXParamPool::SetCameraFarPlaneCornerVecs(gmtl::VecA3f* pCorners) 
{ 
	m_pFarPlaneCorners = pCorners; 

	// Get the far plane offsets if needed
	m_IsFarPlaneOffsetDirty = TRUE;
}

void CDEFXParamPool::SetAccumulationBuffers(IRTexture2D* pZPos, IRTexture2D* pNormals, IRTexture2D* pAlbedo)
{
	m_pZPos = pZPos;
	m_pNormals = pNormals;
	m_pAlbedo = pAlbedo;

	_DEBUG_ASSERT(pZPos->GetWidth(0) == pNormals->GetWidth(0));
	_DEBUG_ASSERT(pZPos->GetHeight(0) == pNormals->GetHeight(0));
	_DEBUG_ASSERT(pZPos->GetWidth(0) == pAlbedo->GetWidth(0));
	_DEBUG_ASSERT(pZPos->GetHeight(0) == pAlbedo->GetHeight(0));

	m_IsScreenUVOffsetDirty = TRUE;
}

boolean 
CDEFXParamPool::IsParamAvailable(uint semantic)
{
	return TRUE;
}

boolean 
CDEFXParamPool::IsMorePassesRequired()
{
	return FALSE;
}

void 
CDEFXParamPool::FinishedResolvingEffect()
{
}

void 
CDEFXParamPool::ResetParams()
{
	if(m_ScreenUVOffsetPool.GetNumAllocated() > 1)
	{
		gmtl::Vec2f* pTemp = (m_ScreenUVOffsetPool.GetLast());
		m_ScreenUVOffsetPool.Reset();
		*(m_ScreenUVOffsetPool.Get()) = *pTemp;
	}

	if(m_FarPlaneOffsetStack.GetNumAllocated() > 1)
	{
		FarPlaneOffsets* pTemp = (m_FarPlaneOffsetStack.GetLast());
		m_FarPlaneOffsetStack.Reset();
		*(m_FarPlaneOffsetStack.Get()) = *pTemp;
	}

	if(m_ViewFarPlaneOffsetStack.GetNumAllocated() > 1)
	{
		ViewFarPlaneOffsets* pTemp = m_ViewFarPlaneOffsetStack.GetLast();
		m_ViewFarPlaneOffsetStack.Reset();
		*(m_ViewFarPlaneOffsetStack.Get()) = *pTemp;
	}
}	

_NAMESPACE_END

