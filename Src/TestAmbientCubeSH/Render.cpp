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
	g_pVerdanaFontFace->BeginRender();
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("%d"), m_LastNumOfFrames), fontPos, 0xFFFFFF90);

	const RMetrics& metric = g_pRenderer->GetMetrics();

	// Renderer	
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

	g_pVerdanaFontFace->EndRender();

	g_pCourierFontFace->BeginRender();

	const CMMetrics& memMetrics = g_GetCoreMemoryMetrics();

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

	g_pCourierFontFace->EndRender();	
}

void RenderDebugObjects()
{
	//if(g_IsDebugRenderObjects)
	//{
	//	// Add debug overlay here
	//	g_pDebugFX->GetParams().SetViewMatrix(*((const gmtl::Matrix44f*)&g_View));
	//	g_pDebugFX->GetParams().SetProjMatrix(*((const gmtl::Matrix44f*)&g_Proj));
	//	g_pDebugFX->BeginRender();	

	//	_LOOPi(MAX_POINT_LIGHTS)
	//	{	
	//		AHPointLightComponent* pPLComp = _CAST_COMP_POINTLIGHT(g_pPointLightEntities[i]);
	//		g_pDebugFX->RenderWireSphere(pPLComp->GetBFXPointLight().pos, 
	//			0.5f, 
	//			pPLComp->GetBFXPointLight().color);	
	//		g_pDebugFX->RenderWireSphere(pPLComp->GetBFXPointLight().pos, 
	//			1.0f / pPLComp->GetBFXPointLight().invRange,
	//			pPLComp->GetBFXPointLight().color);		
	//	}

	//	_LOOPi(g_pVisibleObjects->size())
	//	{		
	//		g_pDebugFX->RenderWireBox((*g_pVisibleObjects)[i].pRendComp->GetWorldOOBox(), gmtl::Vec4f(1.0f, 1.0f, 0.0f, 1.0f));
	//	}

	//	RenderDebugFrustum(g_DebugViewProj, gmtl::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
	//	_LOOPi(4)
	//	{
	//		RenderDebugFrustum(g_SaveCascViewProj[i], gmtl::Vec4f(1.0f, 0.1f, 0.3f, 1.0f));
	//	}

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

	//	_LOOPi(g_pVisibleObjects->size())
	//	{		
	//		g_pDebugFX->RenderWireBox(*(*g_pVisibleObjects)[i].pScreenSpaceBox, gmtl::Vec4f(1.0f, 0.0f, 1.0f, 1.0f));
	//	}

	//	//g_pDebugFX->RenderQuad(gmtl::Vec3f(-0.8f, -0.8f, 0.0f), 
	//	//	gmtl::Vec2f(0.2f, 0.2f), gmtl::Vec2f(0.0f, 0.0f), gmtl::Vec2f(1.0f, 1.0f), g_pBloomFX->GetDownSampledSource());

	//	g_pDebugFX->EndRender();

	//	g_pShadowFX->GetDebug().RenderCascExpSM(gmtl::Vec4f(-0.8f, -0.8f, -0.4f, -0.4f), g_DirLight.pShadow->GetShadowMap());
	//}	
}		

void
RenderAll()
{
	g_CurIndex += g_IncRate * g_TimeDT;
	if(g_CurIndex >= 4.0f)
		g_CurIndex -= 4.0f;

	//int floor = FastFloor(g_CurIndex);
	//int ceiling = FastCeiling(g_CurIndex);
	//if(ceiling >= 4)
	//	ceiling = 0;
	//float frac = g_CurIndex - floor;
	//g_pAmbientFX->LerpAmbientSHCoeff(g_CurCoeff, g_Coeffs[floor], g_Coeffs[ceiling], frac);

	if(g_pRenderer->BeginRender())
	{	
		g_pBaseFX->BeginRender();

		//g_DirLight.pShadow->Update(0.5f, g_DirLight.light.dir, 
		//	_CAST_VEC3(g_EyePos) - g_DirLight.light.dir * 100.0f, _CAST_MAT44(g_View), _CAST_MAT44(g_Proj));

		g_pBaseFX->GetParamPool().SetProjMatrix(_CAST_MAT44(g_Proj));
		g_pBaseFX->GetParamPool().SetViewMatrix(_CAST_MAT44(g_View));
		g_pBaseFX->GetParamPool().SetCameraPos(_CAST_VEC3(g_EyePos));
		g_pBaseFX->GetParamPool().SetDirLight(g_DirLight);
		g_pBaseFX->GetParamPool().SetHemiLight(g_HemiLight);

		gmtl::MatrixA44f g_World;
		gmtl::setTrans(_CAST_MAT44(g_World), gmtl::Vec3f(0.0f, 0.0f, 0.0f));

		g_pBaseFX->GetParamPool().SetWorldMatrix( _CAST_MAT44(g_World) );
		g_pBaseFX->GetParamPool().SetPointLights(NULL, 0);
		g_pBaseFX->GetParamPool().SetZDistanceFromCamera(1.0f);
		
		g_pAmbientFX->GetParamPool().SetAmbientSHCoeff(&g_CurCoeff);
		
		IRTextureCube* pRadianceTex[4*3];
		_LOOPi(4*3)
		{
			pRadianceTex[i] = g_pRadianceCosineTex[i];
		}

		g_pAmbientFX->GetParamPool().SetRadianceCoeff(pRadianceTex, 4);
		g_pAmbientFX->GetParamPool().SetUnitCoeff(g_UnitCoeffs, 4);

		g_Sphere.pMatGroup->SetCurrentMatType(BFX_MAT_BASE_LIGHTING);
		g_pBaseFX->Render(g_Sphere, 1.0f);

		gmtl::MatrixA44f g_World2;
		gmtl::setTrans(_CAST_MAT44(g_World2), gmtl::Vec3f(-10.0f, 0.0f, 0.0f));
		g_pBaseFX->GetParamPool().SetWorldMatrix( _CAST_MAT44(g_World2) );

		g_Bandit.pMatGroup->SetCurrentMatType(BFX_MAT_BASE_LIGHTING);
		g_pBaseFX->Render(g_Bandit, 1.0f);

		gmtl::MatrixA44f g_World3;
		gmtl::setTrans(_CAST_MAT44(g_World3), gmtl::Vec3f(10.0f, 0.0f, 0.0f));
		g_pBaseFX->GetParamPool().SetWorldMatrix( _CAST_MAT44(g_World3) );

		g_Soldier.pMatGroup->SetCurrentMatType(BFX_MAT_BASE_LIGHTING);
		g_pBaseFX->Render(g_Soldier, 1.0f);

		gmtl::MatrixA44f g_WorldFloor;
		gmtl::setTrans(_CAST_MAT44(g_WorldFloor), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
		g_pBaseFX->GetParamPool().SetWorldMatrix( _CAST_MAT44(g_WorldFloor) );

		_LOOPi(g_pRoomSHMeshList->meshes.size())
		{
			g_pRoomSHMeshList->meshes[i].pMatGroup->SetCurrentMatType(BFX_MAT_BASE_LIGHTING);
			g_pBaseFX->Render(g_pRoomSHMeshList->meshes[i], 1.0f);
		}

		g_pBaseFX->EndRender();

		g_pRenderer->GetRPipeline().RenderPipeline();

		RenderDebugObjects();	
		RenderStats();

		g_pRenderer->EndRender();
	}
}
