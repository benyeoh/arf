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

	//fontPos[1] += 34;
	//g_pCourierFontFace->RenderString(ToStringAuto(_W("g_NumSamples: %d"), g_NumSamples), fontPos, 0xFF5A50FA);
	//fontPos[1] += 17;
	//g_pCourierFontFace->RenderString(ToStringAuto(_W("g_AlchemyAOParams: %f, %f, %f"), g_AlchemyAOParams[0], g_AlchemyAOParams[1], g_AlchemyAOParams[2]), fontPos, 0xFF5A50FA);
	//fontPos[1] += 17;
	//g_pCourierFontFace->RenderString(ToStringAuto(_W("g_DistributedSampleParams: %f, %f, %f"), g_DistributedSampleParams[0], g_DistributedSampleParams[1], g_DistributedSampleParams[2]), fontPos, 0xFF5A50FA);

	fontPos[1] += 34;
	g_pCourierFontFace->RenderString(g_pAOParamEntries[g_CurAOParamEntry]->GetString(), fontPos, 0xFFAAA236);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(g_pTechEntry->GetString(), fontPos, 0xFFEEA236);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(g_pTechDownsampleEntry->GetString(), fontPos, 0xFFEE55A2);

	g_pCourierFontFace->Flush();	
}

void RenderDebugObjects()
{
	gmtl::Matrix44f identity;
	gmtl::identity(identity);
	g_pDebugFX->GetParams().SetViewMatrix(identity);
	g_pDebugFX->GetParams().SetProjMatrix(identity);

	g_pDebugFX->RenderQuad(gmtl::Vec3f(-1.0f / g_pAOBuffer->GetWidth(0), 1.0f / g_pAOBuffer->GetHeight(0), 0.0f), gmtl::Vec2f(2.0f, 2.0f), gmtl::Vec2f(0.0f, 0.0f), gmtl::Vec2f(1.0f, 1.0f), g_pAOBuffer);
	g_pDebugFX->Flush();
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

	g_CameraPosWithViewFar[0] = g_EyePos[0];
	g_CameraPosWithViewFar[1] = g_EyePos[1];
	g_CameraPosWithViewFar[2] = g_EyePos[2];
	g_CameraPosWithViewFar[3] = FAR_PLANE;
	
	g_FarPlaneOffsets.offset[0][0] = g_CameraFarCornerVecs[0][0];
	g_FarPlaneOffsets.offset[0][1] = g_CameraFarCornerVecs[0][1];
	g_FarPlaneOffsets.offset[0][2] = g_CameraFarCornerVecs[0][2];

	gmtl::VecA3f temp = g_CameraFarCornerVecs[1] - g_CameraFarCornerVecs[0];
	g_FarPlaneOffsets.offset[1][0] = temp[0];
	g_FarPlaneOffsets.offset[1][1] = temp[1];
	g_FarPlaneOffsets.offset[1][2] = temp[2];

	temp = g_CameraFarCornerVecs[2] - g_CameraFarCornerVecs[0];
	g_FarPlaneOffsets.offset[2][0] = temp[0];
	g_FarPlaneOffsets.offset[2][1] = temp[1];
	g_FarPlaneOffsets.offset[2][2] = temp[2];

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
					
					if(g_pMeshes[i]->meshes[j].pMatGroup->GetTemplate()->GetMaterialByType(SHFX_MAT_CAST_DIR_LIGHT_SHADOW) != NULL)
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
				if(g_pRoomMeshList->meshes[i].pMatGroup->GetTemplate()->GetMaterialByType(SHFX_MAT_CAST_DIR_LIGHT_SHADOW) != NULL)
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

		g_pMainClearGroup->Flush();
		g_pZPosClearGroup->Flush();
		g_pNormalsClearGroup->Flush();

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

		RVarData user;
		user.iVal = 0;

		_LOOPi(NUM_Z_MIPS)
		{
			REffectParam downsampleParams[16];
			InitializeEffect(*g_pDownsampleEffect[i], g_CurDownsampleTechnique, downsampleParams);
			g_pDownsampleGroups[i]->AddDrawOp(g_ZDownsampleMesh[i].GetQuadVertices(), g_ZDownsampleMesh[i].GetQuadIndices(), g_pDownsampleEffect[i], downsampleParams, g_CurDownsampleTechnique, user);
			g_pDownsampleGroups[i]->Flush();
			g_pDownsampleGroups[i]->Reset();
		}
		
		REffectParam aoParams[16];
		InitializeEffect(*g_pAOEffect, g_CurTechnique, aoParams);
		g_pAOGroup->AddDrawOp(g_AOScreenMesh.GetQuadVertices(), g_AOScreenMesh.GetQuadIndices(), g_pAOEffect, aoParams, g_CurTechnique, user);
		g_pAOGroup->Flush();
		g_pAOGroup->Reset();

		g_pBFXParamPool->ResetParams();
		g_pDeferredParamPool->ResetParams();
		g_pSHFXParamPool->ResetParams();

		RenderDebugObjects();	
		RenderStats();

		g_pRenderer->EndRender();
	}
}
