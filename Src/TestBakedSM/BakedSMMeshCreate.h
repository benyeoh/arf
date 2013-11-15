//================================================================================
//
//		BakedSMMeshCreate.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2012
//
//================================================================================

#pragma once

BFXMesh CreatePlanePosY(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX=1, uint numSubZ=1);
BFXMesh CreatePlaneNegY(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX=1, uint numSubZ=1);
BFXMesh CreatePlanePosX(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX=1, uint numSubZ=1);
BFXMesh CreatePlaneNegX(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX=1, uint numSubZ=1);
BFXMesh CreatePlanePosZ(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX=1, uint numSubZ=1);
BFXMesh CreatePlaneNegZ(IRRenderer* pRenderer, float sizeX, float sizeZ, uint numSubX=1, uint numSubZ=1);