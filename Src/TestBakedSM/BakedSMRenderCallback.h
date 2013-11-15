//================================================================================
//
//		BakedSMRenderCallback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/12/2012
//
//================================================================================

#pragma once

class BakedSMRenderCallback : public IBakedSMComputeRenderCallback
{
private:
    IBFXMaterialGroupTemplatePtr m_pParaboloidOcclTemplate;
    //BFXUMeshListPtr m_pSwappedMeshLists[NUM_MESHES];

public:
    BakedSMRenderCallback()
    {
    }

    ~BakedSMRenderCallback()
    {
    }

public:
    void Compute();

    // During BeginRender, the application should fetch and set up all necessary parameters to correctly render the object
    // IE, loading the correct material for SHPRTCompute, setting up parameters and render containers, rendering the object with any relevant material types etc...
    void Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB);
};