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

//void RenderStats()
//{
//	// Add debug overlay here
//	// Frame rate counter
//	gmtl::Vec2i fontPos(1, 17);
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("%d"), m_LastNumOfFrames), fontPos, 0xFFFFFF90);
//
//	const RMetrics& metric = g_pRenderer->GetMetrics();
//
//	//// Renderer	
//	fontPos[1] = 17 * 4;
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numDrawCalls: %d"), metric.numDrawCalls), fontPos, 0xFF90FFFF);
//	fontPos[1] += 17;
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numPrimitivesRendered: %d"), metric.numPrimitivesRendered), fontPos, 0xFF90FFFF);
//	fontPos[1] += 17;
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numVerticesRendered: %d"), metric.numVerticesRendered), fontPos, 0xFF90FFFF);
//	fontPos[1] += 17;
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numRenderGroups: %d"), metric.numRenderGroups), fontPos, 0xFF90FFFF);
//	fontPos[1] += 17;
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numBeginPass: %d"), metric.numBeginPass), fontPos, 0xFF90FFFF);
//	fontPos[1] += 17;
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numBeginTechnique: %d"), metric.numBeginTechnique), fontPos, 0xFF90FFFF);
//	fontPos[1] += 17;
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numApplyConstantParam: %d"), metric.numApplyConstantParam), fontPos, 0xFF90FFFF);
//	fontPos[1] += 17;
//	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numApplyDynamicParam: %d"), metric.numApplyDynamicParam), fontPos, 0xFF90FFFF);
//
//	g_pVerdanaFontFace->Flush();
//
//	const CMMetrics& memMetrics = g_pAllocator->GetMetrics();
//
//	fontPos[1] += 17;
//	g_pCourierFontFace->RenderString(ToStringAuto(_W("numChunkAllocs: %d"), memMetrics.numChunkAllocs), fontPos, 0xFFA0F07F);
//	fontPos[1] += 17;
//	g_pCourierFontFace->RenderString(ToStringAuto(_W("numPageAllocs: %d"), memMetrics.numPageAllocs), fontPos, 0xFFA0F07F);
//	fontPos[1] += 17;
//	g_pCourierFontFace->RenderString(ToStringAuto(_W("numPagesFree: %d"), memMetrics.numPagesFree), fontPos, 0xFFA0F07F);
//	fontPos[1] += 17;
//	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalChunkSizeUsed: %d"), memMetrics.totalChunkSizeUsed), fontPos, 0xFFA0F07F);
//	fontPos[1] += 17;
//	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalHeapSizeUsed: %d"), memMetrics.totalHeapSizeUsed), fontPos, 0xFFA0F07F);
//	fontPos[1] += 17;
//	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalPageSizeUsed: %d"), memMetrics.totalPageSizeUsed), fontPos, 0xFFA0F07F);
//	fontPos[1] += 17;
//
//	g_pCourierFontFace->Flush();	
//}
//
//void RenderDebugObjects()
//{
//	g_pDebugFX->GetParams().SetViewMatrix(*((const gmtl::Matrix44f*)&g_View));
//	g_pDebugFX->GetParams().SetProjMatrix(*((const gmtl::Matrix44f*)&g_Proj));
//
//	//	g_pDebugFX->RenderWireSphere(g_PointLight.pos, 1.0f / g_PointLight.invRange, g_PointLight.color);
//	g_pDebugFX->Flush();
//
//	//g_pShadowFX->GetDebug().RenderCascSM(gmtl::Vec4f(-0.8f, -0.8f, 0.0f, 0.0f), g_pShadowDirLight->GetShadowMap());
//	//g_pShadowFX->GetDebug().RenderPLSM(gmtl::Vec4f(0.1f, -0.8f, 0.8f, -0.4f), g_pShadowPointLight->GetShadowMap());
//}		

void RenderAll()
{
	if(g_pRenderer->BeginRender())
	{	
		g_pMainClearGroup->Flush();
		//RenderDebugObjects();	
		//RenderStats();

		g_pRenderer->EndRender();
	}
}
