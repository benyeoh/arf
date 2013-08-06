//==============================================================================
//
//		CreateFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		22/11/09
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

void CreateSphere(IRRenderer* pRenderer,
				  IRVertexBufferGroup** ppVBGroupDest, 
				  IRIndexBuffer** ppIBDest, 
				  float radius, 
				  uint numThetaSegments, 
				  uint numPhiSegments);

_NAMESPACE_END
