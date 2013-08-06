//==============================================================================
//
//		ExportMeshFn.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/9/2007
//
//==============================================================================

#pragma once 

void
ExportMesh(XSI::X3DObject& obj, 
		   const RVertexDesc* pDesc,
		   IRVertexBufferGroup** ppVBGroup,
		   IRIndexBuffer** ppIB);