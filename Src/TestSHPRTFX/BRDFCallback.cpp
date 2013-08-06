//================================================================================
//
//		BRDFCallback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/8/2012
//
//================================================================================

#include "stdafx.h"

void BRDFCallback::GetBRDF(const gmtl::VecA3f& viewDir, const gmtl::VecA3f* pSrcLightDirs, float* pDestAlbedo, uint numDirs)
{
	_LOOPi(numDirs)
	{
		pDestAlbedo[i] = EvaluateBRDF( _CAST_VEC3(pSrcLightDirs[i]), _CAST_VEC3(viewDir) );
	}
}

void BRDFCallback::SetBRDFParameters(const BRDFData& brdf)
{
	m_CurData = brdf;
	m_NormScale = 1.0f;
}

float BRDFCallback::EvaluateBRDF(const gmtl::Vec3f& lightDir, const gmtl::Vec3f& viewDir)
{
	// NDotL * diffuseScale + (1 + NDotLScale * (NDotL - 1)) * C(Wi.Wv) * lobeScale

	// Evaluate LaFortuneLobes
	float nDotL = lightDir[1];
	if(nDotL < 0.0f)
		nDotL = 0.0f;

	float nDotV = viewDir[1];
	if(nDotV < 0.0f)
		nDotV = 0.0f;

	float lateralTerm = lightDir[0] * viewDir[0] + lightDir[2] * viewDir[2];
	if(lateralTerm < -1.0f)
		lateralTerm = -1.0f;
	if(lateralTerm > 1.0f)
		lateralTerm = 1.0f;

	float normTerm = nDotL * nDotV;
	if(normTerm < 0.0f)
	{
		_DEBUG_ASSERT(FALSE);
		normTerm = 0.0f;
	}

	if(normTerm > 1.0f)
		normTerm = 1.0f;

	float lobeRes = 0.0f;

	float midTerm = (lateralTerm < 0.0f ? -lateralTerm : lateralTerm) + normTerm;
	if(midTerm > 1.0f)
		midTerm = 1.0f;
	else if(midTerm < 0.0f)
		midTerm = 0.0f;

	_LOOPi(3)
	{

		float lobe = m_CurData.lobe[i].Cx * lateralTerm + m_CurData.lobe[i].Cy * (1.0f - midTerm) + m_CurData.lobe[i].Cz * normTerm;

		if(lobe > 1.0f)
		{
			//_DEBUG_ASSERT(FALSE);
			lobe = 1.0f;
		}
		else if(lobe < 0.0f)
		{
			//_DEBUG_ASSERT(FALSE);
			lobe = 0.0f;
		}

		lobeRes += pow( lobe, m_CurData.lobe[i].n );

		if(lobeRes > 1.0f)
		{
			_DEBUG_ASSERT(FALSE);
		}
	}

	float brdf = m_NormScale* ( nDotL * m_CurData.diffuseScale + (1.0f + m_CurData.nDotLScale * (nDotL - 1.0f)) * m_CurData.lobeScale * lobeRes /* * ((1.0f - metalScale) + metalScale * texAlbedo)*/ );
	return brdf;
}

void BRDFCallback::NormalizeBRDFParameters()
{
	//float viewOut[NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES];

	float totalLightEnergyOut = 0.0f;

	// For each view dir
	_LOOPi(NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES)
	{
		gmtl::Vec3f viewDir;
		viewDir[0] = BRDF_DIRS[i].x;
		viewDir[1] = BRDF_DIRS[i].y;
		viewDir[2] = BRDF_DIRS[i].z;

		float viewOutEnergy = 0.0f;

		// Sum all lights per unit hemisphere to the view output
		_LOOPj(NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES)
		{
			gmtl::Vec3f lightDir;
			lightDir[0] = BRDF_DIRS[j].x;
			lightDir[1] = BRDF_DIRS[j].y;
			lightDir[2] = BRDF_DIRS[j].z;

			viewOutEnergy += EvaluateBRDF(lightDir, viewDir);
		}

		viewOutEnergy *= (1.0f / (NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES));

		// Then sum to the total light energy for all views
		totalLightEnergyOut += viewOutEnergy;
	}

	// Find total light energy out per unit hemisphere
	totalLightEnergyOut *= (1.0f / (NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES));

	// Find the normalization scale
	const static float PERFECT_DIFFUSE_ENERGY_OUT = 0.5f;
	m_NormScale = PERFECT_DIFFUSE_ENERGY_OUT / totalLightEnergyOut;
}

