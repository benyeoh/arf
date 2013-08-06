//==============================================================================
//
//		BFXMaterialGroupParser.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/21/2008
//
//==============================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN

void
BFXMaterialGroupParser::Log(const wchar* pStr, ...)
{
	va_list vl;
	wchar traceString[1024];

	va_start(vl, pStr);
	vswprintf_s(traceString, pStr, vl);
	va_end(vl);

	_DEBUG_ASSERT(wcslen(traceString) < 500);
	m_pBase->GetAppCallback().Log(traceString);
}

BFXParserToken 
BFXMaterialGroupParser::GetNextToken(char* pStr)
{
	BFXParserToken token;
	token.pStr = NULL;
	token.tokenType = BFX_TT_NULL;

	std::string tokenStr;
	char* pToken = strtok(pStr, " \t");

	if(pToken)
	{
		token.pStr = pToken;			
		tokenStr = pToken;
		std::transform( tokenStr.begin(), tokenStr.end(), tokenStr.begin(), tolower );

		if(tokenStr == "mat")
		{
			token.tokenType = BFX_TT_MATERIAL;
		}
		else
		if(tokenStr == "=")
		{
			token.tokenType = BFX_TT_EQUAL;
		}
		else
		if(tokenStr == "desc")
		{
			token.tokenType = BFX_TT_DESC;
		}
		else
		if(tokenStr == "//")
		{
			token.tokenType = BFX_TT_COMMENT_LINE;	
		}
		else
		if(tokenStr == "begin_group")
		{
			token.tokenType = BFX_TT_BEGIN_GROUP;	
		}
		else
		if(tokenStr == "end_group")
		{
			token.tokenType = BFX_TT_END_GROUP;	
		}
		else
		if(tokenStr == "define")
		{
			token.tokenType = BFX_TT_DEFINE;
		}
		else
		{
			token.tokenType = BFX_TT_TEXT;
		}
	}

	return token;	
}

boolean
BFXMaterialGroupParser::ParseMatKeyword(uint lineNum)
{	
	BFXParserToken token = GetNextToken(NULL);
	if(token.tokenType != BFX_TT_EQUAL)
	{
		Log(_W("(%d): Error. \"=\" expected."), lineNum);

		return FALSE;
	}

	token = GetNextToken(NULL);
	if(token.tokenType == BFX_TT_NULL)
	{
		Log(_W("(%d): Error. \"[fileName]\" expected."), lineNum);

		return FALSE;
	}

	// Concatenate all tokens into a single string
	wstring fileName = ASCIIToUnicodeAuto(token.pStr);
	token = GetNextToken(NULL);
	while(token.tokenType != BFX_TT_NULL)
	{
		fileName += _W(" ");
		fileName += ASCIIToUnicodeAuto(token.pStr);

		token = GetNextToken(NULL);		
	}

	// Log(_W("Effect Name: %s\n"), fileName.c_str());

	m_MatNames.push_back( fileName );
	m_MatIndices.push_back( m_CurrentBlock );
	
	return TRUE;
}	

boolean
BFXMaterialGroupParser::ParseBeginGroupKeyword(uint lineNum)
{
	BFXParserToken token = GetNextToken(NULL);
	if(token.tokenType == BFX_TT_NULL)
	{
		Log(_W("(%d): Error. \"[group label]\" expected."), lineNum);

		return FALSE;
	}

	wstring name = ASCIIToUnicodeAuto(token.pStr);
	wstring origName = name;
	std::transform( name.begin(), name.end(), name.begin(), tolower );

	CRCDataKey key;
	key.Set(name.c_str(), (uint) name.size() * sizeof(wchar));
	dense_hash_map<CRCDataKey, uint, HashCRCDataKey>::iterator itr = m_Defines.find(key);

	if(itr == m_Defines.end())
	{
		Log(_W("(%d): Error. Invalid group type \"%s\" found."), lineNum, origName.c_str());
		return FALSE;
	}
	
	m_CurrentBlock = itr->second;

	return TRUE;
}

boolean
BFXMaterialGroupParser::ParseEndGroupKeyword(uint lineNum)
{
	m_CurrentBlock = BFX_INVALID;
	return TRUE;
}

boolean
BFXMaterialGroupParser::ParseDescKeyword(uint lineNum)
{
	BFXParserToken token = GetNextToken(NULL);
	if(token.tokenType != BFX_TT_EQUAL)
	{
		Log(_W("(%d): Error. \"=\" expected."), lineNum);

		return FALSE;
	}

	token = GetNextToken(NULL);
	if(token.tokenType == BFX_TT_NULL)
	{
		Log(_W("(%d): Error. \"[description]\" expected."), lineNum);

		return FALSE;
	}

	// Concatenate all tokens into a single string
	wstring fileName = ASCIIToUnicodeAuto(token.pStr);
	token = GetNextToken(NULL);
	while(token.tokenType != BFX_TT_NULL)
	{
		fileName += _W(" ");
		fileName += ASCIIToUnicodeAuto(token.pStr);

		token = GetNextToken(NULL);		
	}

	m_Desc = fileName;

	//Log(_W("Desc: %s\n"), m_Desc.c_str());

	return TRUE;	
}

char*
BFXMaterialGroupParser::ParseLine(char* pStr)
{
	return strtok(pStr, "\r\n");
}

boolean 
BFXMaterialGroupParser::Parse(IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pBuffer);
	_DEBUG_ASSERT(pBuffer->GetDataLength() > 0);

	m_CurrentBlock = BFX_INVALID;
	m_MatNames.clear();
	m_MatIndices.clear();
	m_Desc.clear();
	m_Defines.clear();
	
	// Copy the data over to a new buffer since it will be modified
	uint dataLength = pBuffer->GetDataLength();
	IByteBufferPtr pToParse = _NEW CByteBuffer(dataLength+1);
	memcpy(pToParse->GetData(), pBuffer->GetData(), dataLength);
	pToParse->SetDataLength(dataLength+1);

	// Append the end of string character
	*(pToParse->GetData()+dataLength) = 0;

	// Store each line of the effect group file
	char* pStr = (char*) (pToParse->GetData());
	std::vector< char* > lines;
	char* pLine = ParseLine(pStr);
	while(pLine)
	{
		lines.push_back(pLine);
		pLine = ParseLine(NULL);
	}

	_LOOPi( (uint)(lines.size()) )
	{
		BFXParserToken token = GetNextToken(lines[i]);
		switch(token.tokenType)
		{
		case BFX_TT_COMMENT_LINE:
			{
				// Skip the entire line
				break;
			}

		case BFX_TT_MATERIAL:
			{
				if(m_CurrentBlock == BFX_INVALID)
				{
					Log(_W("(%d): Error. \"begin_group\" or \"end_group\" expected."), i+1);
					return FALSE;
				}

				if(!ParseMatKeyword(i+1))
					return FALSE;
				break;
			}

		case BFX_TT_DESC:
			{
				if(!ParseDescKeyword(i+1))
					return FALSE;
				break;
			}

		case BFX_TT_BEGIN_GROUP:
			{
				if(!ParseBeginGroupKeyword(i+1))
					return FALSE;
				break;
			}

		case BFX_TT_END_GROUP:
			{
				if(!ParseEndGroupKeyword(i+1))
					return FALSE;
				break;
			}

		case BFX_TT_DEFINE:
			{
				if(!ParseDefineKeyword(i+1))
					return FALSE;
				break;
			}
			
		default:
			{
				Log(_W("(%d): Error. Invalid command."), i);
				break;
			}
		}
	}

	return TRUE;
}

boolean 
BFXMaterialGroupParser::ParseDefineKeyword(uint lineNum)
{
	BFXParserToken token = GetNextToken(NULL);
	if(token.tokenType == BFX_TT_NULL)
	{		
		Log(_W("(%d): Error. \"[defName]\" expected."), lineNum);
		return FALSE;
	}
	
	wstring defName = ASCIIToUnicodeAuto(token.pStr);
	token = GetNextToken(NULL);
	
	if(token.tokenType == BFX_TT_NULL)
	{
		Log(_W("(%d): Error. \"[int value]\" expected."), lineNum);
		return FALSE;
	}

	uint value = (uint) atoi(token.pStr);
	
	std::transform( defName.begin(), defName.end(), defName.begin(), tolower );

	CRCDataKey id;
	id.Set(defName.c_str(), (uint) defName.size() * sizeof(wchar));
	_DEBUG_ASSERT(m_Defines.find(id) == m_Defines.end());
	m_Defines[id] = value;

	return TRUE;	
}

uint 
BFXMaterialGroupParser::GetNumOfMats()
{
	_DEBUG_ASSERT(m_MatNames.size() == m_MatIndices.size());
	return (uint) m_MatNames.size();
}

const wchar* 
BFXMaterialGroupParser::GetMatName(uint index)
{
	return m_MatNames[index].c_str();
}

uint 
BFXMaterialGroupParser::GetMatIndex(uint index)
{
	return m_MatIndices[index];
}

const wchar* 
BFXMaterialGroupParser::GetDescription()
{
	return m_Desc.c_str();
}

_NAMESPACE_END