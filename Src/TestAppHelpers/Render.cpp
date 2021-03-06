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

	double time = 0.0;
	double timeUpdate = 0.0;
	_LOOPi(NUM_TIMES_AVERAGE)
	{
		time += g_RenderTimes[i];
		timeUpdate += g_UpdateTimes[i];
	}

	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("Render Time: %f ms"), time * 1000.0 / NUM_TIMES_AVERAGE), fontPos, 0xFFFF00FF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("Update Time: %f ms"), timeUpdate * 1000.0 / NUM_TIMES_AVERAGE), fontPos, 0xFFFF50AA);

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

void GetFrustumAA(const gmtl::MatrixA44f& viewProj, AABox& dest)
{
	gmtl::MatrixA44f invViewProj;
	gmtl::invert(invViewProj, viewProj);

	gmtl::VecA4f screenSpaceBounds[8];
	screenSpaceBounds[0][0] = -1.0f; screenSpaceBounds[0][1] = -1.0f; screenSpaceBounds[0][2] = 0.0f; screenSpaceBounds[0][3] = 1.0f; // near bottom left
	screenSpaceBounds[1][0] = -1.0f; screenSpaceBounds[1][1] = 1.0f; screenSpaceBounds[1][2] = 0.0f; screenSpaceBounds[1][3] = 1.0f; // near top left
	screenSpaceBounds[2][0] = 1.0f; screenSpaceBounds[2][1] = -1.0f; screenSpaceBounds[2][2] = 0.0f; screenSpaceBounds[2][3] = 1.0f; // near bottom right
	screenSpaceBounds[3][0] = 1.0f; screenSpaceBounds[3][1] = 1.0f; screenSpaceBounds[3][2] = 0.0f; screenSpaceBounds[3][3] = 1.0f; // near top right

	screenSpaceBounds[4][0] = -1.0f; screenSpaceBounds[4][1] = -1.0f; screenSpaceBounds[4][2] = 1.0f; screenSpaceBounds[4][3] = 1.0f; // far bottom left
	screenSpaceBounds[5][0] = -1.0f; screenSpaceBounds[5][1] = 1.0f; screenSpaceBounds[5][2] = 1.0f; screenSpaceBounds[5][3] = 1.0f; // far top left
	screenSpaceBounds[6][0] = 1.0f; screenSpaceBounds[6][1] = -1.0f; screenSpaceBounds[6][2] = 1.0f; screenSpaceBounds[6][3] = 1.0f; // far bottom right
	screenSpaceBounds[7][0] = 1.0f; screenSpaceBounds[7][1] = 1.0f; screenSpaceBounds[7][2] = 1.0f; screenSpaceBounds[7][3] = 1.0f; // far top right

	gmtl::VecA4f frustumPtsWorldSpace[8];
	BatchTransformAndProjectVecW1(&invViewProj, screenSpaceBounds, frustumPtsWorldSpace, 8);

	PointsToAABox((gmtl::VecA3f*) frustumPtsWorldSpace, 8, &dest);
}

void RenderDebugObjects()
{
#ifdef _USE_PARABOLOID_DEBUG
	gmtl::VecA3f eyeDir;
	NormalizeVec(&eyeDir, &g_EyeDir);
	IRTexture2D* pPRTRes = g_pAHPRTCompute->ComputeDebug(g_pSceneContainer, g_EyePos, eyeDir, 100.0f, 30.0f);

	gmtl::Matrix44f identity;
	gmtl::identity(identity);
	g_pDebugFX->GetParams().SetViewMatrix(identity);
	g_pDebugFX->GetParams().SetProjMatrix(identity);
	g_pDebugFX->RenderQuad(gmtl::Vec3f(-0.6f, -0.6f, 0.0f), gmtl::Vec2f(0.5f, 0.5f), gmtl::Vec2f(0.0f, 0.0f), gmtl::Vec2f(1.0f, 1.0f), pPRTRes);
	g_pDebugFX->Flush();
#endif

	g_pDebugFX->GetParams().SetViewMatrix(*((const gmtl::Matrix44f*)&g_View));
	g_pDebugFX->GetParams().SetProjMatrix(*((const gmtl::Matrix44f*)&g_Proj));

	// Frustum cull
	gmtl::MatrixA44f viewProj;
	MatMatMult(&viewProj, &g_Proj, &g_View);
	AABox frustumAA;
	GetFrustumAA(viewProj, frustumAA);

	if(g_IsDebugRenderLights)
	{
		AHPointLightComponent* pPLComp[1024];
		uint numComp = g_pSceneContainer->QueryInFrustumPersp((void**) pPLComp, NULL, 1024, frustumAA, viewProj, NEAR_PLANE, FAR_PLANE, SDB_FILTER_POINTLIGHT_COMP);
		_DEBUG_ASSERT(numComp <= 1024);
	
		_LOOPi(numComp)
		{
			g_pDebugFX->RenderWireSphere(pPLComp[i]->GetDEFXPointLight().pos, 1.0f / pPLComp[i]->GetDEFXPointLight().invRange, pPLComp[i]->GetDEFXPointLight().color);
		}
	}

	if(DB_USAGE != USE_ARRAY && g_IsDebugRenderSceneDB)
	{
		ISDBSparseLooseOctTree* pOct = ((ISDBSparseLooseOctTree*) g_pSceneContainer.GetPtr());
		uint numCells = pOct->QueryCellInfo(g_pSparseGridCells, MAX_GRID_DEBUG, frustumAA);
		_LOOPi(numCells)
		{
			g_pDebugFX->RenderWireBox(g_pSparseGridCells[i], gmtl::Vec4f(1.0f, 0.0f, 1.0f, 1.0f));
		}
	}

	g_pDebugFX->Flush();

	//g_pShadowFX->GetDebug().RenderCascSM(gmtl::Vec4f(-0.8f, -0.8f, 0.0f, 0.0f), g_pShadowDirLight->GetShadowMap());
	//g_pShadowFX->GetDebug().RenderPLSM(gmtl::Vec4f(0.1f, -0.8f, 0.8f, -0.4f), g_pShadowPointLight->GetShadowMap());
}		


void RenderAll()
{
	g_CurIndex += g_IncRate * g_TimeDT;
	if(g_CurIndex >= 4.0f)
		g_CurIndex -= 4.0f;

	if(g_pRenderer->BeginRender())
	{	
		g_pMainClearGroup->Flush();
		
		double renderStart = g_pPlatform->GetTimer().GetTime();
		
		g_pSceneRenderPhase->Update();
		
		g_RenderTimes[g_TimeCounter] = g_pPlatform->GetTimer().GetTime() - renderStart;
		g_TimeCounter++;
		g_TimeCounter %= NUM_TIMES_AVERAGE;

		g_pSceneRenderPhase->Flush();


		RenderDebugObjects();
		
        if(g_IsShowDebugText)
		    RenderStats();

		g_pRenderer->EndRender();
	}
}
