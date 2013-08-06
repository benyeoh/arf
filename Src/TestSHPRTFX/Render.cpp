//==============================================================================
//
//		Render.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/11/2009
//
//==============================================================================

#include "stdafx.h"

void RenderStats()
{
	// Add debug overlay here
	// Frame rate counter
	gmtl::Vec2i fontPos(1, 17);
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("%d"), m_LastNumOfFrames), fontPos, 0xFFFFFF90);

	const RMetrics& metric = g_pRenderer->GetMetrics();

	//// Renderer	
	fontPos[1] = 17 * 4;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numDrawCalls: %d"), metric.numDrawCalls), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numPrimitivesRendered: %d"), metric.numPrimitivesRendered), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numVerticesRendered: %d"), metric.numVerticesRendered), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numRenderGroups: %d"), metric.numRenderGroups), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numBeginPass: %d"), metric.numBeginPass), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numBeginTechnique: %d"), metric.numBeginTechnique), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numApplyConstantParam: %d"), metric.numApplyConstantParam), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numApplyDynamicParam: %d"), metric.numApplyDynamicParam), fontPos, 0xFF90FFFF);

	g_pVerdanaFontFace->Flush();

	const CMMetrics& memMetrics = g_pAllocator->GetMetrics();

	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("numChunkAllocs: %d"), memMetrics.numChunkAllocs), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("numPageAllocs: %d"), memMetrics.numPageAllocs), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("numPagesFree: %d"), memMetrics.numPagesFree), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalChunkSizeUsed: %d"), memMetrics.totalChunkSizeUsed), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalHeapSizeUsed: %d"), memMetrics.totalHeapSizeUsed), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalPageSizeUsed: %d"), memMetrics.totalPageSizeUsed), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;

	g_pCourierFontFace->Flush();	
}

void RenderDebugObjects()
{
	g_pDebugFX->GetParams().SetViewMatrix(*((const gmtl::Matrix44f*)&g_View));
	g_pDebugFX->GetParams().SetProjMatrix(*((const gmtl::Matrix44f*)&g_Proj));

	//g_pDebugFX->RenderWireSphere(g_PointLight.pos, 1.0f / g_PointLight.invRange, g_PointLight.color);
	g_pDebugFX->Flush();

	//if(g_IsDebugRenderObjects)
	//{
	//	// Add debug overlay here
	//	g_pDebugFX->GetParams().SetViewMatrix(*((const gmtl::Matrix44f*)&g_View));
	//	g_pDebugFX->GetParams().SetProjMatrix(*((const gmtl::Matrix44f*)&g_Proj));
	//	g_pDebugFX->BeginRender();	

	//	//_LOOPi(g_pVisibleObjects->size())
	//	//{		
	//	//	g_pDebugFX->RenderWireBox((*g_pVisibleObjects)[i].pRendComp->GetWorldOOBox(), gmtl::Vec4f(1.0f, 1.0f, 0.0f, 1.0f));
	//	//}

	//	//RenderDebugFrustum(g_DebugViewProj, gmtl::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
	//	//_LOOPi(4)
	//	//{
	//	//	RenderDebugFrustum(g_SaveCascViewProj[i], gmtl::Vec4f(1.0f, 0.1f, 0.3f, 1.0f));
	//	//}

	//	//g_pDebugFX->RenderLine(gmtl::Vec3f(0.0f, 0.0f, 0.0f), gmtl::Vec3f(1.0f, 0.0f, 0.0f), gmtl::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
	//	//g_pDebugFX->RenderLine(gmtl::Vec3f(0.0f, 0.0f, 0.0f), gmtl::Vec3f(0.0f, 1.0f, 0.0f), gmtl::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
	//	//g_pDebugFX->RenderLine(gmtl::Vec3f(0.0f, 0.0f, 0.0f), gmtl::Vec3f(0.0f, 0.0f, 1.0f), gmtl::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));

	//	g_pDebugFX->EndRender();	

	//	// Render screen space stuff
	//	g_pDebugFX->BeginRender();

	//	gmtl::Matrix44f identity;
	//	gmtl::identity(identity);
	//	g_pDebugFX->GetParams().SetViewMatrix(identity);
	//	g_pDebugFX->GetParams().SetProjMatrix(identity);

	//	//g_pDebugFX->RenderQuad(gmtl::Vec3f(-0.8f, -0.8f, 0.0f), 
	//	//	gmtl::Vec2f(0.2f, 0.2f), gmtl::Vec2f(0.0f, 0.0f), gmtl::Vec2f(1.0f, 1.0f), g_pBloomFX->GetDownSampledSource());

	//	g_pDebugFX->EndRender();
	//}	
}		

void RenderAll()
{
	g_CurIndex += g_IncRate * g_TimeDT;
	if(g_CurIndex >= 4.0f)
		g_CurIndex -= 4.0f;

	gmtl::MatrixA44f viewProj;
	gmtl::MatrixA44f invViewProj;
	MatMatMult(&viewProj, &g_Proj, &g_View);
	gmtl::invertFull(invViewProj, viewProj);
	gmtl::Matrix44f testy = _CAST_MAT44(invViewProj) * _CAST_MAT44(viewProj);

	ComputeCameraFarPlaneCornerVecs(g_CameraFarCornerVecs, g_EyePos, invViewProj);

	if(g_pRenderer->BeginRender())
	{	
		g_pBFXParamPool->SetProjMatrix(_CAST_MAT44(g_Proj));
		g_pBFXParamPool->SetViewMatrix(_CAST_MAT44(g_View));
		g_pBFXParamPool->SetCameraProperties(_CAST_VEC3(g_EyePos), NEAR_PLANE, FAR_PLANE);
		g_pBFXParamPool->SetDirLight(&g_DirLight);
		g_pBFXParamPool->SetHemiLight(&g_HemiLight);

		BFXPointLight* pPL[4];
		pPL[0] = &g_PointLight;
		g_pBFXParamPool->SetPointLights((const BFXPointLight**)pPL, 1);
		g_pBFXParamPool->SetZDistanceFromCamera(1.0f);
		
		g_pAFXParamPool->SetAmbientSHCoeff(&g_AmbientCoeffs);
		g_pAFXParamPool->SetTransferMatrixRGB(&g_AmbientRed, &g_AmbientGreen, &g_AmbientBlue);

		_LOOPi(NUM_MESHES)
		{
			g_pBFXParamPool->SetWorldMatrix( _CAST_MAT44(g_MeshesWorld[i]) );
			if(g_pMeshes[i])
			{
				_LOOPj(g_pMeshes[i]->meshes.size())
				{
					g_pBaseFX->Render(g_pMeshes[i]->meshes[j], 1.0f, BFX_MAT_BASE_LIGHTING, BFX_MAT_BASE_LIGHTING, g_ParamContainer, g_RenderContainer);
				}
			}
		}

		gmtl::Matrix44f skyDomeMat;
		gmtl::identity(skyDomeMat);
		gmtl::Vec4f skyDomeTrans;
		skyDomeTrans[0] = g_EyePos[0];
		skyDomeTrans[1] = g_EyePos[1];
		skyDomeTrans[2] = g_EyePos[2];
		skyDomeTrans[3] = 1.0f;
		gmtl::setTrans(skyDomeMat, skyDomeTrans);
		g_pBFXParamPool->SetWorldMatrix(skyDomeMat);
		_LOOPj(g_pSkyDome->meshes.size())
		{
			g_pBaseFX->Render(g_pSkyDome->meshes[j], 1.0f, BFX_MAT_BASE_LIGHTING, BFX_MAT_BASE_LIGHTING, g_ParamContainer, g_RenderContainer);
		}

		//gmtl::MatrixA44f g_WorldFloor;
		//gmtl::setTrans(_CAST_MAT44(g_WorldFloor), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
		//g_pBFXParamPool->SetWorldMatrix( _CAST_MAT44(g_WorldFloor) );

		//if(g_pRoomMeshList)
		//{
		//	_LOOPi(g_pRoomMeshList->meshes.size())
		//	{
		//		uint uncachedFlags[4];			
		//		uint numUncached = 4;
		//		g_pBaseFX->Render(g_pRoomMeshList->meshes[i], 1.0f, BFX_MAT_BASE_LIGHTING, g_ParamContainer, 
		//			g_RenderContainer, uncachedFlags, numUncached);

		//		_LOOPj(numUncached)
		//		{
		//			g_pRoomMeshList->meshes[i].pMatGroup->CreateEffect(BFX_MAT_BASE_LIGHTING, uncachedFlags[j]);
		//		}
		//	}
		//}

		g_pMainClearGroup->Flush();

		g_pBFXPipeline->FlushOpaqueGroup();
		g_pBFXPipeline->FlushAlphaTestedGroup();
		g_pBFXPipeline->FlushAlphaBlendedGroup();

		g_pBFXParamPool->ResetParams();
		g_pAFXParamPool->ResetParams();

		RenderDebugObjects();	
		RenderStats();

		g_pRenderer->EndRender();
	}
}
