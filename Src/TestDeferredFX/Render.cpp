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

//	g_pDebugFX->RenderWireSphere(g_PointLight.pos, 1.0f / g_PointLight.invRange, g_PointLight.color);
	g_pDebugFX->Flush();

	//g_pShadowFX->GetDebug().RenderCascSM(gmtl::Vec4f(-0.8f, -0.8f, 0.0f, 0.0f), g_pShadowDirLight->GetShadowMap());
	//g_pShadowFX->GetDebug().RenderPLSM(gmtl::Vec4f(0.1f, -0.8f, 0.8f, -0.4f), g_pShadowPointLight->GetShadowMap());
}		

void RenderAll()
{
	g_CurIndex += g_IncRate * g_TimeDT;
	if(g_CurIndex >= 4.0f)
		g_CurIndex -= 4.0f;

	//gmtl::MatrixA44f viewProj;
	gmtl::MatrixA44f invViewProj;
	gmtl::MatrixA44f invProj;
	gmtl::MatrixA44f invView;

	gmtl::VecA3f zeroEyePos;
	zeroEyePos[0] = zeroEyePos[1] = zeroEyePos[2] = 0.0f;

	//MatMatMult(&viewProj, &g_Proj, &g_View);
	//gmtl::invertFull(invViewProj, viewProj);
	gmtl::invertFull(invProj, g_Proj);
	invView = g_View;
	gmtl::setTrans(_CAST_MAT44(invView), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
	gmtl::transpose(_CAST_MAT44(invView));
	MatMatMult(&invViewProj, &invView, &invProj);
	ComputeCameraFarPlaneCornerVecs(g_CameraFarCornerVecs, zeroEyePos, invViewProj);

	//gmtl::VecA3f eyePos;
	//eyePos[0] = eyePos[1] = eyePos[2] = 0.0f;

	gmtl::invertFull(invProj, g_Proj);
	ComputeCameraFarPlaneCornerVecs(g_ViewFarCornerVecs, zeroEyePos, invProj);

	if(g_pRenderer->BeginRender())
	{	
		g_pBFXParamPool->SetProjMatrix(_CAST_MAT44(g_Proj));
		g_pBFXParamPool->SetViewMatrix(_CAST_MAT44(g_View));
		g_pBFXParamPool->SetCameraProperties(_CAST_VEC3(g_EyePos), NEAR_PLANE, FAR_PLANE);
		//g_pBFXParamPool->SetDirLight(g_DirLight);
		g_pBFXParamPool->SetHemiLight(&g_HemiLight);

		//BFXPointLight* pPL[4];
		//pPL[0] = &g_PointLight;
		//g_pBFXParamPool->SetPointLights((const BFXPointLight**)pPL, 1);
		g_pBFXParamPool->SetZDistanceFromCamera(1.0f);
		g_pDeferredParamPool->SetCameraFarPlaneCornerVecs(g_CameraFarCornerVecs);
		g_pDeferredParamPool->SetViewFarPlaneOffsets((gmtl::Vec3f*) &(g_ViewFarCornerVecs[0]), g_ViewFarCornerVecs[1][0] - g_ViewFarCornerVecs[0][0], g_ViewFarCornerVecs[2][1] - g_ViewFarCornerVecs[0][1]);

		g_pSHFXParamPool->SetCascadedDirLight(g_pShadowDirLight);
		g_pSHFXParamPool->SetCascadedCasterFlag(0xF);

		//ISHFXCubePointLight* pShadowPL[NUM_POINT_LIGHTS];
		//_LOOPi(NUM_POINT_LIGHTS)
		//{
		//	pShadowPL[i] = g_pShadowPointLight[i];
		//}

		//g_pSHFXParamPool->SetCubePointLights((ISHFXCubePointLight**) pShadowPL, NUM_POINT_LIGHTS);

		_LOOPi(NUM_MESHES)
		{
			g_pBFXParamPool->SetWorldMatrix( _CAST_MAT44(g_MeshesWorld[i]) );
			if(g_pMeshes[i])
			{
				_LOOPj(g_pMeshes[i]->meshes.size())
				{
					g_pBaseFX->Render(g_pMeshes[i]->meshes[j], 1.0f, DEFX_MAT_ACCUM, DEFX_MAT_ACCUM, g_ParamContainer, g_RenderContainer);
					g_pBaseFX->Render(g_pMeshes[i]->meshes[j], 1.0f, SHFX_MAT_CAST_DIR_LIGHT_SHADOW, SHFX_MAT_CAST_DIR_LIGHT_SHADOW, g_ParamContainer, g_RenderContainer);

					_LOOP(plIndex, NUM_POINT_LIGHTS)
					{
						_LOOP(face, CF_NUM_FACES)
						{
							g_pSHFXParamPool->SetCasterCubePointLight(g_pShadowPointLight[plIndex], (eRCubeFace) face); 
							g_pBaseFX->Render(g_pMeshes[i]->meshes[j], 1.0f, SHFX_MAT_CAST_CUBE_POINT_LIGHT_SHADOW, SHFX_MAT_CAST_CUBE_POINT_LIGHT_SHADOW, g_ParamContainer, g_RenderContainer);								
						}
					}

					g_pBaseFX->Render(g_pMeshes[i]->meshes[j], 1.0f, BFX_MAT_BASE_LIGHTING, BFX_MAT_BASE_LIGHTING, g_ParamContainer, g_RenderContainer);
				}
			}
		}

		gmtl::MatrixA44f worldFloor;
		gmtl::identity(_CAST_MAT44(worldFloor));
		//gmtl::setScale(_CAST_MAT44(worldFloor), gmtl::Vec3f(5.0f, 5.0f, 5.0f));
		gmtl::setTrans(_CAST_MAT44(worldFloor), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
		g_pBFXParamPool->SetWorldMatrix( _CAST_MAT44(worldFloor) );

		if(g_pRoomMeshList)
		{
			_LOOPi(g_pRoomMeshList->meshes.size())
			{
				g_pBaseFX->Render(g_pRoomMeshList->meshes[i], 1.0f, DEFX_MAT_ACCUM, DEFX_MAT_ACCUM, g_ParamContainer, g_RenderContainer);
				g_pBaseFX->Render(g_pRoomMeshList->meshes[i], 1.0f, SHFX_MAT_CAST_DIR_LIGHT_SHADOW, SHFX_MAT_CAST_DIR_LIGHT_SHADOW, g_ParamContainer, g_RenderContainer);

				_LOOP(plIndex, NUM_POINT_LIGHTS)
				{
					_LOOP(face, CF_NUM_FACES)
					{
						g_pSHFXParamPool->SetCasterCubePointLight(g_pShadowPointLight[plIndex], (eRCubeFace) face); 
						g_pBaseFX->Render(g_pRoomMeshList->meshes[i], 1.0f, SHFX_MAT_CAST_CUBE_POINT_LIGHT_SHADOW, SHFX_MAT_CAST_CUBE_POINT_LIGHT_SHADOW, g_ParamContainer, g_RenderContainer);
					}
				}

				g_pBaseFX->Render(g_pRoomMeshList->meshes[i], 1.0f, BFX_MAT_BASE_LIGHTING, BFX_MAT_BASE_LIGHTING, g_ParamContainer, g_RenderContainer);
			}
		}

		_LOOPi(NUM_POINT_LIGHTS)
		{
			g_pDeferredParamPool->SetCurrentLightGroup(g_pDeferredLightGroup[i]);
			g_pDeferredParamPool->SetPointLight(&g_PointLight[i]);
			g_pBFXParamPool->SetWorldMatrix( _CAST_MAT44(g_PointLightWorld[i]) );	
			ISHFXCubePointLight* pPL = g_pShadowPointLight[i];
			g_pSHFXParamPool->SetCubePointLights(&pPL, 1);

			_LOOPj(g_pPointLightMesh->meshes.size())
			{
				g_pBaseFX->Render(g_pPointLightMesh->meshes[j], 1.0f, DEFX_MAT_LIGHTING, DEFX_MAT_LIGHTING, g_ParamContainer, g_RenderContainer);
			}
		}

		g_pDeferredParamPool->SetCurrentLightGroup(g_pDeferredDirLightGroup);
		g_pDeferredParamPool->SetDirLight(&g_DirLight);
		g_pSHFXParamPool->SetCascadedDirLight(g_pShadowDirLight);

		_LOOPi(g_pDirLightMesh->meshes.size())
		{
			g_pBaseFX->Render(g_pDirLightMesh->meshes[i], 1.0f, DEFX_MAT_LIGHTING, DEFX_MAT_LIGHTING, g_ParamContainer, g_RenderContainer);
		}
		
		//gmtl::VecA3f worldLightPos;
		//gmtl::VecA3f viewLightPos;
		//worldLightPos[0] = g_PointLight.pos[0];
		//worldLightPos[1] = g_PointLight.pos[1];
		//worldLightPos[2] = g_PointLight.pos[2];
		//TransformVecW1(&g_View, &worldLightPos, &viewLightPos);
		//g_PointLight.pos[0] = viewLightPos[0];
		//g_PointLight.pos[1] = viewLightPos[1];
		//g_PointLight.pos[2] = viewLightPos[2];
		
		g_pMainClearGroup->Flush();

		g_pDeferredPipeline->Flush();
		g_pBFXPipeline->FlushOpaqueGroup();
		g_pBFXPipeline->FlushAlphaTestedGroup();

		g_pShadowDirLight->Flush();
		g_pDeferredDirLightGroup->Flush();

		_LOOPi(NUM_POINT_LIGHTS)
		{
			g_pShadowPointLight[i]->Flush();
			g_pDeferredLightGroup[i]->Flush();
		}

		g_pBFXPipeline->FlushAlphaBlendedGroup();

		g_pBFXParamPool->ResetParams();
		g_pDeferredParamPool->ResetParams();
		g_pSHFXParamPool->ResetParams();

		RenderDebugObjects();	
		RenderStats();

		g_pRenderer->EndRender();
	}
}
