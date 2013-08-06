//================================================================================
//
//		CDEFXBRDFTableAtlas.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/6/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CDEFXBRDFTableAtlas : public CRefCounted<IDEFXBRDFTableAtlas>
{
private:
	IRTexture2DPtr m_pAtlas;
	IRTexture2DPtr m_pParams;

	CDEFXDeferredFX* m_pDeferredFX;

public:
	CDEFXBRDFTableAtlas(CDEFXDeferredFX* pDeferredFX)
	{
		m_pDeferredFX = pDeferredFX;
	}

	~CDEFXBRDFTableAtlas()
	{
	}

public:
	void GetData(IRTexture2D** ppBRDFAtlas, IRTexture2D** ppBRDFParams);

	void GenerateData(IDEFXBRDFTable** ppTables, uint numTables, const wchar* pAtlasFileName, const wchar* pParamFileName);
	void SetData(IRTexture2D* pBRDFAtlas, IRTexture2D* pBRDFParams);

};

_NAMESPACE_END