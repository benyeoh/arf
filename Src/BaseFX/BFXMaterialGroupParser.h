//==============================================================================
//
//		BFXMaterialGroupParser.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/21/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

enum eBFXTokenType
{
	BFX_TT_MATERIAL,
	BFX_TT_TEXT,
	BFX_TT_DESC,
	BFX_TT_EQUAL,
	BFX_TT_COMMENT_LINE,
	BFX_TT_BEGIN_GROUP,
	BFX_TT_END_GROUP,
	BFX_TT_DEFINE,
	BFX_TT_NULL,
};

struct BFXParserToken
{
	eBFXTokenType tokenType;
	const char* pStr;
};

class BFXMaterialGroupParser
{
private:
	dense_hash_map<CRCDataKey, uint, HashCRCDataKey> m_Defines;
	
	//std::map<MD5Data, uint> m_Defines;
	std::vector<wstring> m_MatNames;
	std::vector<uint> m_MatIndices;
	wstring m_Desc;
	
	CBFXBaseFX* m_pBase;
	uint m_CurrentBlock;
	
public: 
	BFXMaterialGroupParser()
		: m_pBase(NULL)
	{
		CRCDataKey emptyKey;
		emptyKey.hashVal = 0x00000000;
		emptyKey.pHashData = _NEW CByteBuffer(4);
		AppendData(emptyKey.pHashData, (uint) 0xDEADBEEF);
		
		CRCDataKey deleteKey;
		deleteKey.hashVal = 0xFFFFFFFF;
		deleteKey.pHashData = emptyKey.pHashData;
	
		m_Defines.set_deleted_key(deleteKey);
		m_Defines.set_empty_key(emptyKey);
	}

	virtual ~BFXMaterialGroupParser()
	{
	}

private:
	BFXParserToken GetNextToken(char* pStr);
	char* ParseLine(char* pStr);
	boolean ParseDescKeyword(uint lineNum);
	boolean ParseMatKeyword(uint lineNum);
	boolean ParseBeginGroupKeyword(uint lineNum);
	boolean ParseEndGroupKeyword(uint lineNum);
	boolean ParseDefineKeyword(uint lineNum);
	
	void Log(const wchar* pStr, ...);
	
public: 
	void SetBaseFX(CBFXBaseFX* pBase) { m_pBase = pBase; }
	
	boolean Parse(IByteBuffer* pBuffer);

	uint GetNumOfMats();
	const wchar* GetMatName(uint index);
	uint GetMatIndex(uint index);
	const wchar* GetDescription();
};

_NAMESPACE_END
