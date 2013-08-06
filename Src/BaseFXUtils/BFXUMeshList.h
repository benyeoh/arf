//==============================================================================
//
//		BFXUMeshList.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/24/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct BFXUMeshList : public CRefCounted<IRefCounted>
{
	std::vector<BFXMesh> meshes;
};

_DECLARE_SMARTPTR(BFXUMeshList)

_NAMESPACE_END
