
#include "stdafx.h"
#include "../Common.h"

#if EBONY_SHADER_MODEL>=5
#include "../State/RenderContext.h"
#include "../RenderState.h"

#include "../../Base/String.h"
#include "../../RenderInterface/Utility/DrawUtil.h"
#include "../../RenderInterface.h"
#include "../../RenderInterface/Shader.h"
#include "SAOFilter.h"

#if 0
#define _BEGIN_PERF(name)
#define _END_PERF
#else
#define _BEGIN_PERF(name) \
	static GpuClockCounter __beginTime = 0; \
	static GpuClockCounter __endTime = 0; \
	GRC::BeginPerf(name, &__beginTime);

#define _END_PERF \
	GRC::EndPerf(&__endTime);
#endif

#define HOTRELOAD_SAO_SHADERS 1
#if HOTRELOAD_SAO_SHADERS
#include "Ebony/RenderInterface/Utility/ShaderHotReload.h"
#endif

RENDERER_DECLARE_SHADER(SAO, MainVS);
RENDERER_DECLARE_SHADER(SAO, ComposeResultPS);
RENDERER_DECLARE_SHADER(SAO, ComputeAOPS);
RENDERER_DECLARE_SHADER(BilateralBlurCS, BilateralBlurXCS);
RENDERER_DECLARE_SHADER(BilateralBlurCS, BilateralBlurYCS);

namespace SQEX { namespace Ebony {
namespace Renderer {

using namespace Math;
using namespace RenderInterface;
using RenderInterface::DrawUtil;

#if HOTRELOAD_SAO_SHADERS
static ShaderHotReload s_hotReloader;
#endif


SAOFilter::SAOFilter() :
	pDepthTexture_( nullptr ),
	pNormalTexture_( nullptr ),
	pLinearDepthTextureWithMipMap_( nullptr ),
	pAOTexture_( nullptr )
{
	vertexShader_.Mapping(RENDERER_SHADER(SAO, MainVS));
	pixelShaderCompose_.Mapping(RENDERER_SHADER(SAO, ComposeResultPS));
	pixelShaderAO_.Mapping(RENDERER_SHADER(SAO, ComputeAOPS));

	computeShaderBilateralBlurX_.Mapping(RENDERER_SHADER(BilateralBlurCS, BilateralBlurXCS));
	computeShaderBilateralBlurY_.Mapping(RENDERER_SHADER(BilateralBlurCS, BilateralBlurYCS));

#if HOTRELOAD_SAO_SHADERS

	s_hotReloader.Init();
	s_hotReloader.AddShader(&pixelShaderCompose_, "..\\prog\\ebony\\src\\Ebony\\Renderer\\ScreenFilter\\Shader\\SAO.fx", "ComposeResultPS");
	s_hotReloader.AddShader(&pixelShaderAO_, "..\\prog\\ebony\\src\\Ebony\\Renderer\\ScreenFilter\\Shader\\SAO.fx", "ComputeAOPS");
	s_hotReloader.AddShader(&computeShaderBilateralBlurX_, "..\\prog\\ebony\\src\\Ebony\\Renderer\\ScreenFilter\\Shader\\BilateralBlurCS.fx", "BilateralBlurXCS");
	s_hotReloader.AddShader(&computeShaderBilateralBlurY_, "..\\prog\\ebony\\src\\Ebony\\Renderer\\ScreenFilter\\Shader\\BilateralBlurCS.fx", "BilateralBlurYCS");

#endif

	InitializeParameter();
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SAOFilter::~SAOFilter()
{
#if HOTRELOAD_SAO_SHADERS
	s_hotReloader.DeInit();
#endif

	vertexShader_.Release();
	pixelShaderCompose_.Release();
	pixelShaderAO_.Release();

	computeShaderBilateralBlurX_.Release();
	computeShaderBilateralBlurY_.Release();
}

void SAOFilter::InitializeParameter()
{
	intensity_ = 1.5f;
	contrast_ = 1.8f;
	bias_ = 5.0f;
	falloffRadius_ = 7.0f;
	spiralTurns_ = 9.4f;
	screenMipRadiusFactor_ = 9.0f;
	numSamples_ = 32;
	sampleOffset_ = 0.25f;
	occlusionMax_ = 10.0f;
		
	nearFalloffThreshold_ = 0.17f;
	maxScreenRadius_ = 0.25f;

	blurFalloff_ = 5.0f;
	blurDistFalloff_ = 0.01f;
	blurThreshold_ = 1.0f;
	blurThresholdNormal_ = 0.6f;
}

//-----------------------------------------------------------------------------
///	
//-----------------------------------------------------------------------------
void SAOFilter::ChangeRenderTarget(RenderTexture* target, int index)
{
	GRC::EndScene();
	GRC::SetRenderTarget(0, target->GetColorSurface(0, index));
	//GRC::SetRenderTarget(1, NULL);
	//GRC::SetRenderTarget(2, NULL);
	//GRC::SetRenderTarget(3, NULL);
	GRC::BeginScene(true);
}

void SAOFilter::SetAOTextures( RenderTexture* pAOTexture, RenderTexture* pAOTextureTemp )
{
	pAOTexture_ = pAOTexture;
	pAOTextureTemp_ = pAOTextureTemp;
}

void SAOFilter::SetLinearDepthTextureWithMipMap( RenderTexture *pLinearDepthTextureWithMipMap )
{
	pLinearDepthTextureWithMipMap_ = pLinearDepthTextureWithMipMap;
}

void SAOFilter::SetDepthTexture( RenderTexture  *pDepthTexture )
{
	pDepthTexture_ = pDepthTexture;
}

void SAOFilter::SetDepthSurface( Surface *pDepth )
{
	pDepthSurface_ = pDepth;
}

void SAOFilter::SetNormalTexture( Texture *pNormalTexture )
{
	pNormalTexture_ = pNormalTexture;
}

void SAOFilter::ComputeAOPass(const RenderState *pRenderState, float* pViewFarOffsets)
{
	ChangeRenderTarget( pAOTexture_, 0 );
	//GRC::SetDepthStencil(pDepthSurface_);
	GRC::Clear(RenderInterface::ClearFlags::Color, Color4B::GetWhite(), 0.0f, 0);

	bool oldDepthTestEnabled = GRC::IsDepthTestEnable();
	bool oldDepthWriteEnabled = GRC::IsDepthWriteEnable();
	GRC::SetDepthTestEnable(false);
	GRC::SetDepthWriteEnable(false);

	VertexShader *vs = &vertexShader_;
	PixelShader *ps = &pixelShaderAO_;

	EBONY_ALIGNED_BEGIN(16) struct SAOParams
	{
		float g_ViewFarPlaneUVOffset[4];
		float g_ProjParams[4];	// x = proj[0][0], y = proj[1][1], z = screenWidth, w = screenHeight

		float g_Intensity;
		float g_Contrast;
		float g_Bias;
		float g_SampleOffset;
		
		float g_OcclusionMax;
		float g_FalloffRadius;
		float g_SpiralTurns;
		float g_ScreenMipRadiusFactor;		// Higher value forces higher LOD sampling

		float g_FarPlane;
		float g_NearFalloffThreshold;
		float g_MaxScreenRadius;
		int	g_NumSamples;
	} EBONY_ALIGNED_END(16);

	UniformTemporary uniform( sizeof(SAOParams) );
	SAOParams *pBuffer = (SAOParams*)uniform.Lock();

	pBuffer->g_Intensity				= 2.0f * intensity_;
	pBuffer->g_Contrast					= contrast_;
	pBuffer->g_Bias						= bias_;
	pBuffer->g_FalloffRadius			= falloffRadius_;
	pBuffer->g_SpiralTurns				= spiralTurns_ * 2.0f * 3.142f;
	pBuffer->g_SampleOffset				= sampleOffset_;
	pBuffer->g_OcclusionMax				= occlusionMax_;

	float zNear, zFar;
	const MatrixA& matrix = pRenderState->GetProjectionMatrix();
	matrix.GetZNearFar(zNear, zFar);

	pBuffer->g_ViewFarPlaneUVOffset[0] = pViewFarOffsets[0];
	pBuffer->g_ViewFarPlaneUVOffset[1] = pViewFarOffsets[1];
	pBuffer->g_ViewFarPlaneUVOffset[2] = pViewFarOffsets[2];
	pBuffer->g_ViewFarPlaneUVOffset[3] = pViewFarOffsets[3];
	pBuffer->g_FarPlane		= zFar;

	pBuffer->g_ProjParams[0] = matrix.GetElem(0, 0);
	pBuffer->g_ProjParams[1] = matrix.GetElem(1, 1);
	pBuffer->g_ProjParams[2] = pAOTexture_->GetWidth();
	pBuffer->g_ProjParams[3] = pAOTexture_->GetHeight();

	//pBuffer->g_ProjParams.SetElem(0, matrix.GetElem(0, 0));
	//pBuffer->g_ProjParams.SetElem(1, matrix.GetElem(1, 1));
	//pBuffer->g_ProjParams.SetElem(2, pAOTexture_->GetWidth());
	//pBuffer->g_ProjParams.SetElem(3, pAOTexture_->GetHeight());

	//pBuffer->g_ScreenMipRadiusFactor	= pBuffer->g_ProjParams.GetElem(2) / screenMipRadiusFactor_;
	pBuffer->g_ScreenMipRadiusFactor	= pBuffer->g_ProjParams[2] / screenMipRadiusFactor_;
	pBuffer->g_NearFalloffThreshold     = nearFalloffThreshold_;
	pBuffer->g_MaxScreenRadius          = maxScreenRadius_;
	pBuffer->g_NumSamples = numSamples_;

	uniform.Unlock();

	ps->SetUniformTemporaryByName("SAOParams", &uniform);

	ShaderParameter param = ps->GetSamplerParameterByName("g_SamplePoint");
	ps->SetSampler(param, TextureFilter::Point, TextureAddress::Clamp);
	param = ps->GetSamplerParameterByName("g_SampleLinear");
	ps->SetSampler(param, TextureFilter::Linear, TextureAddress::Clamp);
	param = ps->GetSamplerParameterByName("g_SamplePointMipLinear");
	ps->SetSamplerDetail(param, TextureFilter::Point, TextureFilter::Point, TextureFilter::Linear, TextureAddress::_Clamp, TextureAddress::Clamp, TextureAddress::Clamp);

	param = ps->GetSamplerParameterByName("g_DepthTex");
	ps->SetTexture(param, pLinearDepthTextureWithMipMap_->GetColor());
	param = ps->GetSamplerParameterByName("g_NormalTex");
	ps->SetTexture(param, pNormalTexture_);

	GRC::SetShader(vs, ps);

	_BEGIN_PERF("AO Pass")
	DrawUtil::DrawFillScreen();
	_END_PERF

	GRC::SetDepthTestEnable(oldDepthTestEnabled);
	GRC::SetDepthWriteEnable(oldDepthWriteEnabled);
	GRC::ResetShader();

}

void SAOFilter::ComputeBlurPass(const RenderState* pRenderState, float* pViewFarOffsets, RenderTexture* pDest)
{
	GRC::EndScene();
	GRC::ResetStates();
	GRC::BeginScene();

	// 1080p
	const static int NUM_THREADS_X = 192;
	const static int NUM_THREADS_Y = 192;

	// X blur pass
	ComputeShader* cs = &computeShaderBilateralBlurX_;
	ShaderParameter param = cs->GetSamplerParameterByName("g_BlurInput");
	cs->SetTexture(param, pAOTexture_->GetColor());
	param = cs->GetSamplerParameterByName("g_ZInput");
	cs->SetTexture(param, pLinearDepthTextureWithMipMap_->GetColor());

	param = cs->GetSamplerParameterByName("g_SamplePoint");
	cs->SetSampler(param, TextureFilter::Point, TextureAddress::ClampToEdge);
	param = cs->GetSamplerParameterByName("g_SampleLinear");
	cs->SetSampler(param, TextureFilter::Linear, TextureAddress::ClampToEdge);

	EBONY_ALIGNED_BEGIN(16) struct BilateralBlurParams
	{
		float g_ViewFarPlaneUVOffset[4];

		float g_TextureRes[2];
		float g_TextureInvRes[2];

		float g_BlurFalloff;
		float g_BlurDistFalloff;
		float g_BlurThreshold;
		float g_BlurThresholdNormal;
		
		float g_FarPlane;
	} EBONY_ALIGNED_END(16);

	UniformTemporary uniform( sizeof(BilateralBlurParams) );
	BilateralBlurParams *pBuffer = (BilateralBlurParams*)uniform.Lock();

	pBuffer->g_BlurFalloff		= 1.44269504f / (2.0f * blurFalloff_ * blurFalloff_);
	pBuffer->g_BlurDistFalloff	= pBuffer->g_BlurFalloff * blurDistFalloff_;

	float zNear, zFar;
	const MatrixA& matrix = pRenderState->GetProjectionMatrix();
	matrix.GetZNearFar(zNear, zFar);

	pBuffer->g_ViewFarPlaneUVOffset[0] = pViewFarOffsets[0];
	pBuffer->g_ViewFarPlaneUVOffset[1] = pViewFarOffsets[1];
	pBuffer->g_ViewFarPlaneUVOffset[2] = pViewFarOffsets[2];
	pBuffer->g_ViewFarPlaneUVOffset[3] = pViewFarOffsets[3];

	pBuffer->g_FarPlane		    = zFar;
	pBuffer->g_BlurThreshold	= blurThreshold_; //(2.0f * 0.832554611f * (1.0f / blurThreshold_)) / 255000.0f;
	pBuffer->g_BlurThresholdNormal	= blurThresholdNormal_ * 2.0f - 1.0f;
	pBuffer->g_TextureRes[0]	= pAOTexture_->GetWidth();
	pBuffer->g_TextureRes[1]	= pAOTexture_->GetHeight();
	pBuffer->g_TextureInvRes[0]	= 1.0f / pAOTexture_->GetWidth();
	pBuffer->g_TextureInvRes[1]	= 1.0f / pAOTexture_->GetHeight();

	uniform.Unlock();
	cs->SetUniformTemporaryByName("BilateralBlurParams", &uniform);

	cs->SetTargetTextureByName("g_Output", pAOTextureTemp_->GetColor());

	GRC::SetComputeShader(cs);

	EBONY_ASSERT( pAOTextureTemp_->GetWidth() == pAOTexture_->GetWidth() );
	EBONY_ASSERT( pAOTextureTemp_->GetHeight() == pAOTexture_->GetHeight() );

	int numGroupsX = pAOTextureTemp_->GetWidth() / NUM_THREADS_X;
	if((pAOTextureTemp_->GetWidth() % NUM_THREADS_X) != 0)
		numGroupsX++;
	int numGroupsY = pAOTextureTemp_->GetHeight();

	{
		_BEGIN_PERF("Blur X Pass")
		GRC::DispatchComputeShader(numGroupsX, numGroupsY);
		_END_PERF
	}

	cs->SetTargetTextureByName("g_Output", NULL);

	param = cs->GetSamplerParameterByName("g_BlurInput");
	cs->SetTexture(param, NULL);
	param = cs->GetSamplerParameterByName("g_ZInput");
	cs->SetTexture(param, NULL);
	
	
	// Y blur pass
	cs = &computeShaderBilateralBlurY_;
	param = cs->GetSamplerParameterByName("g_BlurInput");
	cs->SetTexture(param, pAOTextureTemp_->GetColor());
	param = cs->GetSamplerParameterByName("g_ZInput");
	cs->SetTexture(param, pLinearDepthTextureWithMipMap_->GetColor());

	param = cs->GetSamplerParameterByName("g_SamplePoint");
	cs->SetSampler(param, TextureFilter::Point, TextureAddress::ClampToEdge);
	param = cs->GetSamplerParameterByName("g_SampleLinear");
	cs->SetSampler(param, TextureFilter::Linear, TextureAddress::ClampToEdge);

	pBuffer = (BilateralBlurParams*)uniform.Lock();

	pBuffer->g_BlurFalloff		= 1.44269504f / (2.0f * blurFalloff_ * blurFalloff_);
	pBuffer->g_BlurDistFalloff	= pBuffer->g_BlurFalloff * blurDistFalloff_;

	pBuffer->g_ViewFarPlaneUVOffset[0] = pViewFarOffsets[0];
	pBuffer->g_ViewFarPlaneUVOffset[1] = pViewFarOffsets[1];
	pBuffer->g_ViewFarPlaneUVOffset[2] = pViewFarOffsets[2];
	pBuffer->g_ViewFarPlaneUVOffset[3] = pViewFarOffsets[3];

	pBuffer->g_FarPlane		= zFar;
	pBuffer->g_BlurThreshold		= blurThreshold_; //(2.0f * 0.832554611f * (1.0f / blurThreshold_)) / 255000.0f;
	pBuffer->g_BlurThresholdNormal	= blurThresholdNormal_ * 2.0f - 1.0f;

	pBuffer->g_TextureRes[0]	= pDest->GetWidth();
	pBuffer->g_TextureRes[1]	= pDest->GetHeight();
	pBuffer->g_TextureInvRes[0]	= 1.0f / pDest->GetWidth();
	pBuffer->g_TextureInvRes[1]	= 1.0f / pDest->GetHeight();

	uniform.Unlock();
	cs->SetUniformTemporaryByName("BilateralBlurParams", &uniform);
	cs->SetTargetTextureByName("g_OutputSingle", pDest->GetColor());

	//GRC::SetColorWriteEnable(ColorWriteBits::None);
	GRC::SetComputeShader(cs);
	//GRC::SetBlendEnable(true);
	//GRC::SetBlendFunc(BlendFunc::Zero, BlendFunc::SrcColor, BlendFunc::Zero, BlendFunc::One);
	
	EBONY_ASSERT( pAOTextureTemp_->GetWidth() == pDest->GetWidth() );
	EBONY_ASSERT( pAOTextureTemp_->GetHeight() == pDest->GetHeight() );

	numGroupsX = pDest->GetHeight() / NUM_THREADS_Y;
	if((pDest->GetHeight() % NUM_THREADS_Y) != 0)
		numGroupsX++;
	numGroupsY = pDest->GetWidth();

	{
		_BEGIN_PERF("Blur Y Pass")
		GRC::DispatchComputeShader(numGroupsX, numGroupsY);
		_END_PERF
	}

	param = cs->GetSamplerParameterByName("g_BlurInput");
	cs->SetTexture(param, NULL);
	param = cs->GetSamplerParameterByName("g_ZInput");
	cs->SetTexture(param, NULL);

	cs->SetTargetTextureByName("g_OutputSingle", NULL);

	GRC::ResetShader();
	//GRC::SetColorWriteEnable(ColorWriteBits::RGBA);
}

void SAOFilter::ComposeResultPass(const RenderState *pRenderState, RenderTexture *pDest)
{
	ChangeRenderTarget( pDest, 0 );
	bool oldDepthTestEnabled = GRC::IsDepthTestEnable();
	bool oldDepthWriteEnabled = GRC::IsDepthWriteEnable();
	GRC::SetDepthTestEnable(false);
	GRC::SetDepthWriteEnable(false);

	//GRC::SetBlendEnable(true);
	//GRC::SetBlendFunc(BlendFunc::Zero, BlendFunc::SrcColor, BlendFunc::Zero, BlendFunc::One);

	VertexShader *vs = &vertexShader_;
	PixelShader *ps = &pixelShaderCompose_;

	ShaderParameter param = ps->GetSamplerParameterByName("g_SamplePoint");
	ps->SetSampler(param, TextureFilter::Point, TextureAddress::Clamp);

	param = ps->GetSamplerParameterByName("g_AOTex");
	ps->SetTexture(param, pAOTexture_->GetColor());

	//GRC::SetColorWriteEnable( ColorWriteBits::Alpha );

	GRC::SetShader(vs, ps);

	DrawUtil::DrawFillScreen();

	GRC::SetBlendEnable(false);
	GRC::SetBlendFunc();

	GRC::SetDepthTestEnable(oldDepthTestEnabled);
	GRC::SetDepthWriteEnable(oldDepthWriteEnabled);
	//GRC::SetColorWriteEnable( ColorWriteBits::RGBA );
	GRC::ResetShader();
}

void SAOFilter::GetViewFarOffsets(const RenderState* pRenderState, float *pDest)
{
	MatrixA invProj = pRenderState->GetProjectionMatrix().Inverse();

	VectorA topLeft(-1.0f, 1.0f, 1.0f, 1.0f);
	VectorA bottomRight(1.0f, -1.0f, 1.0f, 1.0f);

	VectorA viewTopLeft = invProj.Transform(topLeft);
	viewTopLeft.DivInPlace(viewTopLeft.GetElem(3));
	VectorA viewBottomRight = invProj.Transform(bottomRight);
	viewBottomRight.DivInPlace(viewBottomRight.GetElem(3));

	pDest[0] = viewTopLeft[0];
	pDest[1] = viewTopLeft[1];
	pDest[2] = viewBottomRight[0] - viewTopLeft[0];
	pDest[3] = viewBottomRight[1] - viewTopLeft[1];
}

void SAOFilter::Draw(const RenderState *pRenderState, RenderTexture* pDest)
{
#if HOTRELOAD_SAO_SHADERS
	s_hotReloader.Check();
#endif

	float viewFarOffsets[4];
	GetViewFarOffsets(pRenderState, viewFarOffsets);

	ComputeAOPass(pRenderState, viewFarOffsets);
	ComputeBlurPass(pRenderState, viewFarOffsets, pAOTexture_);
	ComposeResultPass(pRenderState, pDest);
}

void SAOFilter::DrawNoCompose(const RenderState *pRenderState, RenderTexture* pDest )
{
#if HOTRELOAD_SAO_SHADERS
	s_hotReloader.Check();
#endif

	float viewFarOffsets[4];
	GetViewFarOffsets(pRenderState, viewFarOffsets);

	_BEGIN_PERF("SAO Filter Begin")
	ComputeAOPass(pRenderState, viewFarOffsets);
	ComputeBlurPass(pRenderState, viewFarOffsets, pDest);
	_END_PERF
}

}
}}

#undef _BEGIN_PERF
#undef _END_PERF

#endif
