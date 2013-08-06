//================================================================================
//
//		CDEFXBRDFTable.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/5/2012
//
//================================================================================

#include "DeferredFXFramework.h"
#include "BRDFDirs.h"

_NAMESPACE_BEGIN

const DEFXLaFortuneBRDFParam& CDEFXBRDFTable::GetParams()
{
	return m_Param;
}

IRTexture2D* CDEFXBRDFTable::GetData()
{
	return m_pData;
}

float CDEFXBRDFTable::EvaluateLobes(float x1y1_dot_x2y2, float z1_dot_z2, const DEFXLaFortuneLobe* pLobes, uint numLobes)
{
	if(x1y1_dot_x2y2 < -1.0f)
		x1y1_dot_x2y2 = -1.0f;
	if(x1y1_dot_x2y2 > 1.0f)
		x1y1_dot_x2y2 = 1.0f;

	if(z1_dot_z2 < 0.0f)
		z1_dot_z2 = 0.0f;
	if(z1_dot_z2 > 1.0f)
		z1_dot_z2 = 1.0f;

	float anisotropicTerm = (x1y1_dot_x2y2 < 0.0f ? -x1y1_dot_x2y2 : x1y1_dot_x2y2) + z1_dot_z2;
	if(anisotropicTerm > 1.0f)
		anisotropicTerm = 1.0f;
	if(anisotropicTerm < 0.0f)
		anisotropicTerm = 0.0f;

	float lobeRes = 0.0f;
	_LOOPi(numLobes)
	{
		float lobe = pLobes[i].Cx * x1y1_dot_x2y2 + pLobes[i].Cy * (1.0f - anisotropicTerm) + pLobes[i].Cz * z1_dot_z2;

		if(lobe > 1.0f)
			lobe = 1.0f;

		if(lobe < 0.0f)
			lobe = 0.0f;
		
		lobeRes += pow( lobe, pLobes[i].n );
	}

	return lobeRes;
}

float CDEFXBRDFTable::EvaluateBRDFNoDiffuse(const gmtl::Vec3f& lightDir, const gmtl::Vec3f& viewDir, const DEFXLaFortuneLobe* pLobes, uint numLobes)
{
	// Evaluate LaFortuneLobes
	float nDotL = lightDir[1];
	//nDotL = (nDotL + m_Param.nDotLScale) / (1.0f + m_Param.nDotLScale);
	if(nDotL < 0.0f)
		nDotL = 0.0f;
	if(nDotL > 1.0f)
		nDotL = 1.0f;

	float nDotV = viewDir[1];
	if(nDotV < 0.0f)
		nDotV = 0.0f;

	float lateralTerm = lightDir[0] * viewDir[0] + lightDir[2] * viewDir[2];
	float normTerm = nDotL * nDotV;

	float lobeRes = EvaluateLobes(lateralTerm, normTerm, pLobes, numLobes);

	// We clamp the lobe results here because we do not use HDR data format for storage
	if(lobeRes > 1.0f)
		lobeRes = 1.0f;
	if(lobeRes < 0.0f)
		lobeRes = 0.0f;

	float brdf = ( (1.0f + m_Param.nDotLScale * (nDotL - 1.0f)) /*nDotL*/ * m_Param.lobeScale * lobeRes /* * ((1.0f - metalScale) + metalScale * texAlbedo)*/ );
	return brdf;
}

float CDEFXBRDFTable::EvaluateBRDF(const gmtl::Vec3f& lightDir, const gmtl::Vec3f& viewDir, const DEFXLaFortuneLobe* pLobes, uint numLobes)
{
	// NDotL * diffuseScale + (1 + NDotLScale * (NDotL - 1)) * C(Wi.Wv) * lobeScale

	// Evaluate LaFortuneLobes
	float nDotL = lightDir[1];
	//nDotL = (nDotL + m_Param.nDotLScale) / (1.0f + m_Param.nDotLScale);
	if(nDotL < 0.0f)
		nDotL = 0.0f;
	if(nDotL > 1.0f)
		nDotL = 1.0f;

	float nDotV = viewDir[1];
	if(nDotV < 0.0f)
		nDotV = 0.0f;

	float lateralTerm = lightDir[0] * viewDir[0] + lightDir[2] * viewDir[2];
	float normTerm = nDotL * nDotV;

	float lobeRes = EvaluateLobes(lateralTerm, normTerm, pLobes, numLobes);
	
	// We clamp the lobe results here because we do not use HDR data format for storage
	if(lobeRes > 1.0f)
		lobeRes = 1.0f;
	if(lobeRes < 0.0f)
		lobeRes = 0.0f;

	float brdf = ( nDotL * m_Param.diffuseScale + (1.0f + m_Param.nDotLScale * (nDotL - 1.0f)) /*nDotL*/ * m_Param.lobeScale * lobeRes /* * ((1.0f - metalScale) + metalScale * texAlbedo)*/ );
	return brdf;
}

float CDEFXBRDFTable::GetBRDFOverbrightScale(const DEFXLaFortuneLobe* pLobes, uint numLobes, float maxOverbright)
{
	std::vector<float> maxOverbrightVals;

	// For each view dir
	_LOOPi(NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES)
	{
		gmtl::Vec3f viewDir;
		viewDir[0] = BRDF_DIRS[i].x;
		viewDir[1] = BRDF_DIRS[i].y;
		viewDir[2] = BRDF_DIRS[i].z;

		// Sum all lights per unit hemisphere to the view output
		_LOOPj(NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES)
		{
			gmtl::Vec3f lightDir;
			lightDir[0] = BRDF_DIRS[j].x;
			lightDir[1] = BRDF_DIRS[j].y;
			lightDir[2] = BRDF_DIRS[j].z;

			float outEnergy = EvaluateBRDFNoDiffuse(lightDir, viewDir, pLobes, numLobes);
			if(outEnergy > maxOverbright)
			{
				maxOverbrightVals.push_back(outEnergy);
			}
		}
	}

	float scaleRes = 1.0f;

	if(maxOverbrightVals.size() > 0)
	{
		// Sort
		std::sort(maxOverbrightVals.begin(), maxOverbrightVals.end(), std::greater<float>());

		// Find the top 30% median value and scale according to it
		uint index = (uint) (maxOverbrightVals.size() * 0.35f);
		float val = maxOverbrightVals[index];
		scaleRes = maxOverbright / val;
	}

	return scaleRes;
}

float CDEFXBRDFTable::GetNormalizedBRDFScale(const DEFXLaFortuneLobe* pLobes, uint numLobes)
{
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

			viewOutEnergy += EvaluateBRDF(lightDir, viewDir, pLobes, numLobes);
		}

		viewOutEnergy *= (1.0f / (NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES));

		// Then sum to the total light energy for all views
		totalLightEnergyOut += viewOutEnergy;
	}

	// Find total light energy out per unit hemisphere
	totalLightEnergyOut *= (1.0f / (NUM_BRDF_SAMPLES * NUM_BRDF_SAMPLES));

	// Find the normalization scale
	const static float PERFECT_DIFFUSE_ENERGY_OUT = 0.5f;
	float normScale = PERFECT_DIFFUSE_ENERGY_OUT / totalLightEnergyOut;

	return normScale;
}

void CDEFXBRDFTable::GenerateData(const DEFXLaFortuneLobe* pLobes, uint numLobes, const wchar* pDataFileName, float maxOverbrightNormalize)
{
	float normScale = 1.0f;
	if(maxOverbrightNormalize > 0.0f)
	{
		normScale = GetNormalizedBRDFScale(pLobes, numLobes);
		m_Param.diffuseScale *= normScale;
		m_Param.lobeScale *= normScale;

		float overBrightScale = GetBRDFOverbrightScale(pLobes, numLobes, maxOverbrightNormalize);
		m_Param.lobeScale *= overBrightScale;
	}

	IRTexture2D* pTableData = m_pDeferredFX->GetRenderer().GetRResourceMgr().CreateTexture2D(NULL, DEFX_BRDF_TABLE_SIZE, DEFX_BRDF_TABLE_SIZE, 1, TEXF_R8, TEXU_DEFAULT);
	if(pDataFileName)
		pTableData->SetFileName(pDataFileName);

	uint pitch;
	byte* pData = pTableData->Lock(0, pitch, NULL);

	_LOOPi(DEFX_BRDF_TABLE_SIZE)
	{
		float z1_dot_z2 = ((float)i) / (DEFX_BRDF_TABLE_SIZE - 1);
		
		_LOOPj(DEFX_BRDF_TABLE_SIZE)
		{
			float x1y1_dot_x2y2 = ((((float)j) / (DEFX_BRDF_TABLE_SIZE - 1)) * 2.0f) - 1.0f;
			
			float lobeRes = EvaluateLobes(x1y1_dot_x2y2, z1_dot_z2, pLobes, numLobes);
			
			if(lobeRes > 1.0f)
				lobeRes = 1.0f;
			if(lobeRes < 0.0f)
				lobeRes = 0.0f;

			pData[j] = (byte) FastRound(lobeRes * 255.0f);
		}

		pData += pitch;
	}

	pTableData->Unlock(0);

	SetData(pTableData);
}

void CDEFXBRDFTable::SetData(IRTexture2D* pData)
{
	m_pData = pData;
}

void CDEFXBRDFTable::SetParams(const DEFXLaFortuneBRDFParam& params)
{
	m_Param = params;
}

_NAMESPACE_END