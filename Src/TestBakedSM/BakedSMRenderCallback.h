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

public:
    BakedSMRenderCallback()
    {
    }

    ~BakedSMRenderCallback()
    {
    }

private:
    void ComputeOneMesh(uint index, uint lightMapSize, IRTexture2D* pVisSphereTex);

public:
    void Compute();
    void ResetMaterials();

    // During BeginRender, the application should fetch and set up all necessary parameters to correctly render the object
    // IE, loading the correct material for SHPRTCompute, setting up parameters and render containers, rendering the object with any relevant material types etc...
    void Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB);
};