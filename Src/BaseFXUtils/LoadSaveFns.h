//==============================================================================
//
//		LoadSaveFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/10/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

uint
SaveMaterialGroup(IBFXMaterialGroup* pMatGroup, IByteBuffer* pBuffer);

uint
LoadMaterialGroup(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, IBFXMaterialGroup** ppGroup);

uint
SaveMaterialGroupTemplate(IBFXMaterialGroupTemplate* pTemplate, IByteBuffer* pBuffer);

uint
LoadMaterialGroupTemplate(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, IBFXMaterialGroupTemplate** ppTemplate);

uint
SaveBFXMesh(BFXMesh* pMesh, IByteBuffer* pBuffer);

uint
LoadBFXMesh(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, BFXMesh& mesh);

uint
SaveBFXUMeshList(BFXUMeshList* pMeshList, IByteBuffer* pBuffer);

uint
LoadBFXUMeshList(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, BFXUMeshList& meshList);

_NAMESPACE_END