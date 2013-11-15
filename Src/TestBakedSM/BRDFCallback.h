//================================================================================
//
//		BRDFCallback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/8/2012
//
//================================================================================

#pragma once

struct LaFortuneLobe
{
	float Cx;
	float Cy;
	float Cz;

	float n;
};

struct BRDFData
{
	LaFortuneLobe lobe[3];
	float nDotLScale;
	float metalScale;
	float lobeScale;

	float diffuseScale;
};

class BRDFCallback : public ISHPRTComputeBRDFCallback
{
private:
	BRDFData	m_CurData;
	float		m_NormScale;

public:
	BRDFCallback()
		: m_NormScale(1.0f)
	{
	}

	~BRDFCallback()
	{
	}

private:
	float EvaluateFullBRDF(const gmtl::Vec3f& lightDir, const gmtl::Vec3f& viewDir);
	float EvaluateBRDFLobe(const gmtl::Vec3f& lightDir, const gmtl::Vec3f& viewDir);

public:
	void SetBRDFParameters(const BRDFData& brdf);
	void NormalizeBRDFParameters();
	float GetDiffuseScale();
	float GetLobeScale();

	// Callback
	void GetBRDF(const gmtl::VecA3f& viewDir, const gmtl::VecA3f* pSrcLightDirs, float* pDestAlbedo, uint numDirs);
};