//================================================================================
//
//		CDEFXBRDFTable.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/5/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CDEFXBRDFTable : public CRefCounted<IDEFXBRDFTable>
{
private:
	CDEFXDeferredFX* m_pDeferredFX;
	DEFXLaFortuneBRDFParam m_Param;
	IRTexture2DPtr m_pData;
	float m_LobeLumenNormalizeFactor;

public:
	CDEFXBRDFTable(CDEFXDeferredFX* pDeferredFX)
		: m_pDeferredFX(pDeferredFX)
		, m_LobeLumenNormalizeFactor(0.0f)
	{
	}

	~CDEFXBRDFTable()
	{
	}

private:
	float EvaluateLobes(float x1y1_dot_x2y2, float z1_dot_z2, const DEFXLaFortuneLobe* pSrcLobes, uint numLobes);
	float EvaluateBRDFNoDiffuse(const gmtl::Vec3f& lightDir, const gmtl::Vec3f& viewDir, const DEFXLaFortuneLobe* pLobes, uint numLobes);
	float EvaluateBRDF(const gmtl::Vec3f& lightDir, const gmtl::Vec3f& viewDir, const DEFXLaFortuneLobe* pLobes, uint numLobes);
	float GetNormalizedBRDFScale(const DEFXLaFortuneLobe* pSrcLobes, uint numLobes);
	float GetBRDFOverbrightScale(const DEFXLaFortuneLobe* pLobes, uint numLobes, float maxOverbright);

public:
	const DEFXLaFortuneBRDFParam& GetParams();	
	IRTexture2D* GetData();

	//void NormalizeLobes(DEFXLaFortuneLobe* pDestLobes, const DEFXLaFortuneLobe* pSrcLobes, uint numLobes);
	void GenerateData(const DEFXLaFortuneLobe* pLobes, uint numLobes, const wchar* pDataFileName, float maxOverbrightNormalize=0.0f);

	void SetData(IRTexture2D* pData);
	void SetParams(const DEFXLaFortuneBRDFParam& params);
};

_NAMESPACE_END
