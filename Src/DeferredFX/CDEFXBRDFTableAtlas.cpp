//================================================================================
//
//		CDEFXBRDFTableAtlas.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/6/2012
//
//================================================================================

#include "DeferredFXFramework.h"

_NAMESPACE_BEGIN

void CDEFXBRDFTableAtlas::GetData(IRTexture2D** ppBRDFAtlas, IRTexture2D** ppBRDFParams)
{
	*ppBRDFAtlas = m_pAtlas;
	*ppBRDFParams = m_pParams;
}

void CDEFXBRDFTableAtlas::GenerateData(IDEFXBRDFTable** ppTables, uint numTables, const wchar* pAtlasFileName, const wchar* pParamFileName)
{
	m_pAtlas = m_pDeferredFX->GetRenderer().GetRResourceMgr().CreateTexture2D(NULL, DEFX_BRDF_TABLE_SIZE * DEFX_NUM_TABLES_PER_SIDE, DEFX_BRDF_TABLE_SIZE * DEFX_NUM_TABLES_PER_SIDE, 1, TEXF_R8, TEXU_DEFAULT);
	if(pAtlasFileName)
		m_pAtlas->SetFileName(pAtlasFileName);

	m_pParams = m_pDeferredFX->GetRenderer().GetRResourceMgr().CreateTexture2D(NULL, DEFX_MAX_NUM_TABLES, 1, 1, TEXF_A8R8G8B8, TEXU_DEFAULT);
	if(pParamFileName)
		m_pParams->SetFileName(pParamFileName);

	uint pitch;
	byte* pAtlasData = m_pAtlas->Lock(0, pitch, NULL);

	gmtl::Vec4i readOnly;
	readOnly[0] = 0;
	readOnly[1] = 0;
	readOnly[2] = 0;
	readOnly[3] = 0;

	_LOOPi(numTables)
	{
		IRTexture2D* pTableTex = ppTables[i]->GetData();

		uint tableDataPitch;
		byte* pTableData = pTableTex->Lock(0, tableDataPitch, &readOnly);
		
		uint xOffset = i % DEFX_NUM_TABLES_PER_SIDE;
		uint yOffset = i / DEFX_NUM_TABLES_PER_SIDE;

		byte* pDest = pAtlasData + yOffset * DEFX_BRDF_TABLE_SIZE * pitch + xOffset * DEFX_BRDF_TABLE_SIZE;
		_LOOPj(DEFX_BRDF_TABLE_SIZE)
		{
			memcpy(pDest, pTableData, DEFX_BRDF_TABLE_SIZE);
			pTableData += tableDataPitch;
			pDest += pitch;
		}

		pTableTex->Unlock(0);
	}

	m_pAtlas->Unlock(0);

	byte* pParamData = m_pParams->Lock(0, pitch, NULL);

	_LOOPi(numTables)
	{
		const DEFXLaFortuneBRDFParam& param = ppTables[i]->GetParams();
		
		// Range encode diffuse and lobe scale
		const static float SCALE_RANGE = 16.0f;
		_DEBUG_ASSERT(param.diffuseScale < SCALE_RANGE);
		_DEBUG_ASSERT(param.lobeScale < SCALE_RANGE);

		byte diffuseScaleRanged = (byte) FastRound( (param.diffuseScale / SCALE_RANGE) * 255.0f );
		byte lobeScaleRanged = (byte) FastRound( (param.lobeScale / SCALE_RANGE) * 255.0f );
		byte metalScale = (byte) FastRound( param.metalScale * 255.0f );
		byte nDotLScale = (byte) FastRound( param.nDotLScale * 255.0f );
		
		pParamData[i*4] = metalScale;			// b
		pParamData[i*4+1] = lobeScaleRanged;	// g
		pParamData[i*4+2] = diffuseScaleRanged;	// r
		pParamData[i*4+3] = nDotLScale;			// a
	}

	m_pParams->Unlock(0);
}

void CDEFXBRDFTableAtlas::SetData(IRTexture2D* pBRDFAtlas, IRTexture2D* pBRDFParams)
{
	m_pAtlas = pBRDFAtlas;
	m_pParams = pBRDFParams;
}

_NAMESPACE_END