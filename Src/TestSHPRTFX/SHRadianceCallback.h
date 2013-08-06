//================================================================================
//
//		SHRadianceCallback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/8/2012
//
//================================================================================

#pragma once

class SHRadianceCallback : public ISHPRTComputeOccludedCallback
{
private:
	IBFXMaterialGroupTemplatePtr m_pParaboloidRadianceTemplate;
	IBFXMaterialGroupTemplatePtr m_pParaboloidSkyDomeTemplate;
	BFXUMeshListPtr m_pSwappedMeshLists[NUM_MESHES];
	BFXUMeshListPtr m_pSwappedSkyDome;

	gmtl::Matrix44f m_ParaboloidSkyDomeMat;

public:
	SHRadianceCallback()
	{
	}

	~SHRadianceCallback()
	{
	}

public:
	void Compute();

	// During BeginRender, the application should fetch and set up all necessary parameters to correctly render the object
	// IE, loading the correct material for SHPRTCompute, setting up parameters and render containers, rendering the object with any relevant material types etc...
	void Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB);
};