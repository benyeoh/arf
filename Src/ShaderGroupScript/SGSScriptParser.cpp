//==============================================================================
//
//		SGSScriptParser.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 13 April, 2014
//
//==============================================================================

#include "ShaderGroupScript.h"

_NAMESPACE_BEGIN

boolean IsNewline(char c)
{
	return (c == '\r' || c == '\n');
}

int CheckMultiNewline(const char* pStr)
{
	if(pStr[0] == '\n' && pStr[1] == '\r')
	{
		return 2;
	}
	else if(pStr[0] == '\r' && pStr[1] == '\n')
	{
		return 2;
	}
	else if(IsNewline(pStr[0]))
	{
		return 1;
	}

	return 0;
}

struct SGSFunctionEntry
{
	const static uint MAX_NUM_PARAMETERS = 8;

	const char* pFnName;
	uint fnNameLength;

	SGSValue parameters[MAX_NUM_PARAMETERS];
	int numParameters;
};

struct SGSParseStack
{
	const static int MAX_STACK = 16;

	const char* pCurStr[MAX_STACK];
	int curStrLength[MAX_STACK];
	int curAdvance[MAX_STACK];

	const char* pCurTag[MAX_STACK];
	int curTagLength[MAX_STACK];

	int curStack;	

	const wchar* pError[MAX_STACK];
	const char* pErrorTag[MAX_STACK];
	int errorTagLength[MAX_STACK];
	int errorLineNum[MAX_STACK];
	int numErrors;

	SGSParseStack()
		: curStack(-1)
		, numErrors(0)
	{
	}

	//SGSParseStack& operator=(const SGSParseStack& other)
	//{
	//	this->curStack = other.curStack;
	//	_LOOPi(other.curStack)
	//	{
	//		this->pCurStr[i] = other.pCurStr[i];
	//		this->curStrLength[i] = other.curStrLength[i];
	//		this->curAdvance[i] = other.curAdvance[i];
	//		this->pCurTag[i] = other.pCurTag[i];
	//		this->curTagLength[i] = other.curTagLength[i];
	//	}

	//	this->numErrors = other.numErrors;
	//	_LOOPi(other.numErrors)
	//	{
	//		this->pError[i] = other.pError[i];
	//		this->pErrorTag[i] = other.pErrorTag[i];
	//		this->errorTagLength[i] = other.errorTagLength[i];
	//		this->errorLineNum[i] = other.errorLineNum[i];
	//	}

	//	return *this;
	//}
};

struct SGSParseContext
{
	DenseChainedHashMap<CRCDataPtrKey, IByteBufferPtr, HashCRCDataPtrKey> defines;
	DenseChainedHashMap<CRCDataPtrKey, IByteBufferPtr, HashCRCDataPtrKey> includes;

	IAppCallback* pAppCallback;
	SGSParseStack stack;

	SGSScript* pTheScript;

	const wchar* pMainFile;

	SGSParseContext()
		: pAppCallback(NULL)
		, pMainFile(NULL)
		, pTheScript(NULL)
	{
		defines.Initialize(8, 0.7f);
		includes.Initialize(8, 0.7f);
	}

	void PushShader(IByteBuffer* pName, IByteBuffer* pData)
	{
		SGSShader shader;
		shader.SetName(pName);
		shader.SetShaderCode(pData);
		pTheScript->AddShader(shader);
	}

	void PushParameter(SGSParameter& param)
	{
		pTheScript->AddParameter(param);
	}

	void PushTechnique(SGSTechnique& tech)
	{
		pTheScript->AddTechnique(tech);
	}

	void PushErrorStr(const wchar* pErrorStr)
	{
		if(stack.curStack >= 0)
		{
			_DEBUG_ASSERT(stack.numErrors < SGSParseStack::MAX_STACK);
			stack.pError[stack.numErrors] = pErrorStr;

			// Recurse until we find a meaningful tag (ie, skip #defines)
			int i = stack.curStack;
			while(stack.pCurTag[i] == NULL && i > 0)
			{
				i--;
			}

			// Find the line num for this error
			int lineNum = 1;
			_LOOPj(stack.curAdvance[i])
			{
				int numChars = CheckMultiNewline(&(stack.pCurStr[i][j]));
				if(numChars > 0)
					lineNum++;

				if(numChars > 1)
					j++;
			}

			stack.errorLineNum[stack.numErrors] = lineNum;

			stack.pErrorTag[stack.numErrors] = stack.pCurTag[i];
			stack.errorTagLength[stack.numErrors] = stack.curTagLength[i];

			stack.numErrors++;
		}
	}

	void PushAndSetCurStr(const char* pStr, int length, const char* pFilePath, int filePathLength)
	{
		if(length > 0)
		{
			stack.curStack++;
			_DEBUG_ASSERT(stack.curStack < SGSParseStack::MAX_STACK);
			if(stack.curStack < SGSParseStack::MAX_STACK)
			{
				stack.curTagLength[stack.curStack]	= filePathLength;
				stack.pCurTag[stack.curStack]		= pFilePath;
				stack.pCurStr[stack.curStack]		= pStr;
				stack.curStrLength[stack.curStack]	= length;
				stack.curAdvance[stack.curStack]	= 0;
			}
		}
	}

	void AdvanceAndPopCurStr(int length)
	{
		stack.curAdvance[stack.curStack] += length;
		_DEBUG_ASSERT(stack.curAdvance[stack.curStack] <= stack.curStrLength[stack.curStack]);
		if(stack.curAdvance[stack.curStack] >= stack.curStrLength[stack.curStack] ||
		   stack.pCurStr[stack.curStack][stack.curAdvance[stack.curStack]] == 0)
			stack.curStack--;
	}

	boolean IsEnd() const
	{
		return GetCurStr() == NULL;
	}

	const char* GetCurStr() const
	{
		if(stack.curStack == -1)
			return NULL;

		return stack.pCurStr[stack.curStack] + stack.curAdvance[stack.curStack];
	}
};

SGSScriptParser::SGSScriptParser()
{
	
}

SGSScriptParser::~SGSScriptParser()
{

}

void SGSScriptParser::Initialize()
{
	const static TokenEntry s_KeywordTokenEntries[] =
	{
		// These has to be in order of length of the string
		{ "annotation", SGSScriptParser::ANNOTATION_BLOCK },

		{ "bool",		SGSScriptParser::BOOL_TYPE },
		{ "code",		SGSScriptParser::CODE_BLOCK },

		{ "define",		SGSScriptParser::PREPRO_DEFINE },

		{ "false",		SGSScriptParser::FALSE_BOOL },

		{ "int",		SGSScriptParser::INT_TYPE },
		{ "int2",		SGSScriptParser::INT2_TYPE },
		{ "int3",		SGSScriptParser::INT3_TYPE },
		{ "int4",		SGSScriptParser::INT4_TYPE },
		{ "include",	SGSScriptParser::PREPRO_INCLUDE },

		{ "float",		SGSScriptParser::FLOAT_TYPE },
		{ "float2",		SGSScriptParser::FLOAT2_TYPE },
		{ "float3",		SGSScriptParser::FLOAT3_TYPE },
		{ "float4",		SGSScriptParser::FLOAT4_TYPE },
		{ "float4x4",	SGSScriptParser::FLOAT4X4_TYPE },

		{ "string",		SGSScriptParser::STRING_TYPE },
		{ "shader",		SGSScriptParser::SHADER_BLOCK },

		{ "pass",		SGSScriptParser::PASS_BLOCK },
		{ "parameter",	SGSScriptParser::PARAMETER_BLOCK },

		{ "true",		SGSScriptParser::TRUE_BOOL },
		{ "technique",	SGSScriptParser::TECHNIQUE_BLOCK },
	};

	const static TokenEntry s_OperatorTokenEntries[] =
	{
		{ "#",	SGSScriptParser::PREPROCESSOR_COM },
		{ "{",	SGSScriptParser::BLOCK_BEGIN },
		{ "}",	SGSScriptParser::BLOCK_END },
		{ "(",	SGSScriptParser::GROUP_BEGIN },
		{ ")",	SGSScriptParser::GROUP_END },
		{ "+",	SGSScriptParser::ADD_OP },
		{ "-",	SGSScriptParser::MINUS_OP },

		{ "*",	SGSScriptParser::MUL_OP },
		{ "*/",	SGSScriptParser::COMMENT_BLOCK_END },

		{ "/",	SGSScriptParser::DIV_OP },
		{ "//",	SGSScriptParser::COMMENT_LINE },
		{ "/*",	SGSScriptParser::COMMENT_BLOCK_BEGIN },
	
		{ "|",	SGSScriptParser::OR_OP },
		{ "||", SGSScriptParser::BOOLEAN_OR },

		{ "&",	SGSScriptParser::AND_OP },
		{ "&&", SGSScriptParser::BOOLEAN_AND },

		{ "\"", SGSScriptParser::QUOTE },
	
		{ ">",	SGSScriptParser::GREATER_THAN_OP },
		{ ">=", SGSScriptParser::GREATER_THAN_OR_EQUAL_OP },
	
		{ "<",	SGSScriptParser::LESS_THAN_OP },
		{ "<=", SGSScriptParser::LESS_THAN_OR_EQUAL_OP },
	
		{ "=",	SGSScriptParser::EQUAL_OP },

		{ ";",	SGSScriptParser::STATEMENT_END_OP },

		{ ",",	SGSScriptParser::SEPARATOR_OP },
	};

	memset(m_NumTokensWordMatch, 0, MAX_NUM_SYMBOLS * sizeof(uint));

	uint numEntriesKeyword = sizeof(s_KeywordTokenEntries) / sizeof(TokenEntry);
	_LOOPi(numEntriesKeyword)
	{
		uint index = (uint) s_KeywordTokenEntries[i].pSymbol[0];
		uint strLen = strlen(s_KeywordTokenEntries[i].pSymbol);
		_DEBUG_ASSERT(index < MAX_NUM_SYMBOLS);
		m_WordMatch[index][m_NumTokensWordMatch[index]].tokenEntry = s_KeywordTokenEntries[i];
		m_WordMatch[index][m_NumTokensWordMatch[index]].strLen = strLen;
		m_NumTokensWordMatch[index]++;
		_DEBUG_ASSERT(m_NumTokensWordMatch[index] < MAX_NUM_WORDS);

		_DEBUG_ASSERT(s_KeywordTokenEntries[i].tokenType >= TOKEN_KEYWORD_BEGIN);
		_DEBUG_ASSERT(s_KeywordTokenEntries[i].tokenType < TOKEN_KEYWORD_END);

		m_Tokens[s_KeywordTokenEntries[i].tokenType].tokenEntry = s_KeywordTokenEntries[i];
		m_Tokens[s_KeywordTokenEntries[i].tokenType].strLen = strLen;
	}

	uint numEntriesOps = sizeof(s_OperatorTokenEntries) / sizeof(TokenEntry);
	_LOOPi(numEntriesOps)
	{
		uint index = (uint) s_OperatorTokenEntries[i].pSymbol[0];
		uint strLen = strlen(s_OperatorTokenEntries[i].pSymbol);
		_DEBUG_ASSERT(index < MAX_NUM_SYMBOLS);
		m_WordMatch[index][m_NumTokensWordMatch[index]].tokenEntry = s_OperatorTokenEntries[i];
		m_WordMatch[index][m_NumTokensWordMatch[index]].strLen = strLen;
		m_NumTokensWordMatch[index]++;
		_DEBUG_ASSERT(m_NumTokensWordMatch[index] < MAX_NUM_WORDS);

		_DEBUG_ASSERT(s_OperatorTokenEntries[i].tokenType >= TOKEN_OPERATOR_BEGIN);
		_DEBUG_ASSERT(s_OperatorTokenEntries[i].tokenType < TOKEN_OPERATOR_END);

		m_Tokens[s_OperatorTokenEntries[i].tokenType].tokenEntry = s_OperatorTokenEntries[i];
		m_Tokens[s_OperatorTokenEntries[i].tokenType].strLen = strLen;
	}

	// Add predefined functions
	m_UserFunctionMap.Initialize(8, 0.7f);
	
	CRCDataPtrKey fnKey;
	fnKey.Set(m_FromFile.GetName(), strlen(m_FromFile.GetName()));
	m_UserFunctionMap.Add(fnKey, &m_FromFile);

}

boolean SGSScriptParser::ExecuteFunction(SGSParseContext& context, SGSFunctionEntry& entry, SGSValue* pResults, int& numResults)
{
	CRCDataPtrKey key;
	key.Set(entry.pFnName, entry.fnNameLength);
	ISGSUserFunction** ppUserFn = m_UserFunctionMap.Get(key);
	if(ppUserFn == NULL)
	{
		context.PushErrorStr(_W("Unknown function"));
		return FALSE;
	}

	ISGSUserFunction* pUserFn = *ppUserFn;

	eSGSValueType* pFnParamTypes;
	uint fnNumParams;
	pUserFn->GetParameterSig(&pFnParamTypes, fnNumParams);
	if(entry.numParameters != fnNumParams)
	{
		context.PushErrorStr(_W("Invalid number of parameters for function"));
		return FALSE;
	}

	_LOOPi(fnNumParams)
	{
		if(entry.parameters[i].type != pFnParamTypes[i])
		{
			context.PushErrorStr(_W("Invalid type of parameters for function"));
			return FALSE;
		}
	}
	
	boolean success = pUserFn->Execute(entry.parameters, entry.numParameters, pResults, numResults);
	if(!success)
		context.PushErrorStr(_W("Function failed to execute"));

	return success;
}

void SGSScriptParser::SkipWhitespaceAndComments(SGSParseContext& context)
{
	while(true)
	{
		WordMatchEntry matchEntry;
		if(IsWhitespace(context.GetCurStr()[0]))
		{
			context.AdvanceAndPopCurStr(1);
		}
		else if(TryParseOperator(context.GetCurStr(), &matchEntry))
		{
			if(matchEntry.tokenEntry.tokenType == COMMENT_LINE)
			{
				context.AdvanceAndPopCurStr(matchEntry.strLen);
				while(context.GetCurStr()[0] && !IsNewline(context.GetCurStr()[0]))
					context.AdvanceAndPopCurStr(1);

				ConsumeMultiNewline(context);
			}
			else if(matchEntry.tokenEntry.tokenType == COMMENT_BLOCK_BEGIN)
			{
				context.AdvanceAndPopCurStr(matchEntry.strLen);
				while(!context.IsEnd())
				{
					if( TryMatchToken(context.GetCurStr(), m_Tokens[COMMENT_BLOCK_END].strLen, m_Tokens[COMMENT_BLOCK_END]) )
					{
						context.AdvanceAndPopCurStr(m_Tokens[COMMENT_BLOCK_END].strLen);
						break;
					}
					
					context.AdvanceAndPopCurStr(1);
				}
			}
			else
				break;
		}
		else
		{
			break;
		}
	}
}

boolean SGSScriptParser::TryMatchToken(const char* pStr, uint length, const WordMatchEntry& token)
{
	if(token.strLen == length)
	{
		_LOOPi(length)
		{
			if(pStr[i] == 0)
				return FALSE;

			if(pStr[i] != token.tokenEntry.pSymbol[i])
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::IsLetterOrUnderscore(char c)
{
	if( (c >= 'a' && c <= 'z') || 
		(c >= 'A' && c <= 'Z') || 
		(c == '_') )
	{
		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::IsNumber(char c)
{
	if( (c >= '0' && c <= '9') )
	{
		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::IsHexNumber(char c)
{
	if( (c >= '0' && c <= '9') || 
		(c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F') )
	{
		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::IsWhitespace(char c)
{
	return (c == ' ' || c == '\t' || IsNewline(c));
}

boolean SGSScriptParser::IsWhitespaceAndNull(char c)
{
	return (c == 0 || IsWhitespace(c));
}

boolean SGSScriptParser::IsOperator(char c)
{
	return (!IsLetterOrUnderscore(c) && !IsNumber(c) && !IsWhitespace(c));
}

boolean SGSScriptParser::ConsumeMultiNewline(SGSParseContext& context)
{
	int numChars = CheckMultiNewline(context.GetCurStr());
	context.AdvanceAndPopCurStr(numChars);
	return numChars > 0 ? TRUE : FALSE;
}

boolean SGSScriptParser::TryParseIdentifierOrKeyword(const char* pStr, WordMatchEntry* pMatchedToken)
{
	uint curLength = 0;
	if(IsLetterOrUnderscore(pStr[curLength]))
	{
		// Find the longest unbroken string that counts as an identifier
		curLength++;
		while(IsLetterOrUnderscore(pStr[curLength]) || IsNumber(pStr[curLength]))
		{
			curLength++;
		}

		// Then try to match the string with a token from longest to shortest
		_LOOPi(m_NumTokensWordMatch[pStr[0]])
		{
			uint indexOfWord = m_NumTokensWordMatch[pStr[0]] - i - 1;
			if(m_WordMatch[pStr[0]][indexOfWord].strLen < curLength)
			{
				// Early out
				break;
			}
			else if(TryMatchToken(pStr, curLength, m_WordMatch[pStr[0]][indexOfWord]))
			{
				*pMatchedToken = m_WordMatch[pStr[0]][indexOfWord];
				return TRUE;
			}
		}

		pMatchedToken->tokenEntry.pSymbol = pStr;
		pMatchedToken->tokenEntry.tokenType = IDENTIFIER;;
		pMatchedToken->strLen = curLength;

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryParseIdentifierOrKeywordOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken)
{
	WordMatchEntry defEntry;
	boolean isDefinePushed;
	boolean isIDFound = FALSE;
	do
	{
		isDefinePushed = FALSE;
		isIDFound = TryParseIdentifier(context.GetCurStr(), &defEntry);
		if(isIDFound)
		{
			isDefinePushed = TryPushDefineParameter(context, defEntry.tokenEntry.pSymbol, defEntry.strLen);
			SkipWhitespaceAndComments(context);
		}

	} while(isDefinePushed);

	return TryParseIdentifierOrKeyword(context.GetCurStr(), pMatchedToken);
}


boolean SGSScriptParser::TryParseOperator(const char* pStr, WordMatchEntry* pMatchedToken)
{
	uint curLength = 0;
	if(IsOperator(pStr[curLength]))
	{
		// Find the longest unbroken string that counts as an operator
		curLength++;
		while(IsOperator(pStr[curLength]))
		{
			curLength++;
		}

		uint lengthMatch = curLength;
		uint numTokens = m_NumTokensWordMatch[pStr[0]];
		while(lengthMatch > 0)
		{
			// Then try to match the string with a token from longest to shortest
			_LOOPi(numTokens)
			{
				uint indexOfWord = numTokens - i - 1;
				if(m_WordMatch[pStr[0]][indexOfWord].strLen < lengthMatch)
				{
					// Early out
					break;
				}
				else if(TryMatchToken(pStr, lengthMatch, m_WordMatch[pStr[0]][indexOfWord]))
				{
					*pMatchedToken = (m_WordMatch[pStr[0]][indexOfWord]);
					return TRUE;
				}
			}
		
			lengthMatch--;
		}
	}

	return FALSE;
}

boolean SGSScriptParser::TryParseOperatorOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken)
{
	WordMatchEntry defEntry;
	boolean isDefinePushed;
	boolean isIDFound = FALSE;
	do
	{
		isDefinePushed = FALSE;

		isIDFound = TryParseIdentifier(context.GetCurStr(), &defEntry);
		if(isIDFound)
		{			
			isDefinePushed = TryPushDefineParameter(context, defEntry.tokenEntry.pSymbol, defEntry.strLen);
			SkipWhitespaceAndComments(context);
		}

	} while(isDefinePushed);

	return TryParseOperator(context.GetCurStr(), pMatchedToken);
}

//boolean SGSScriptParser::TryParseInteger(const char* pStr, uint& length, WordMatchEntry* pMatchedToken)
//{
//	uint curLength = 0;
//
//	if(IsNumber(pStr[curLength]))
//	{
//		curLength++;
//		while(!IsWhitespace(pStr[curLength]) && !IsOperator(pStr[curLength]))
//		{
//			if(IsNumber(pStr[curLength]))
//				curLength++;
//			else
//			{
//				return FALSE;
//			}
//		}
//
//		pMatchedToken->strLen = curLength;
//		pMatchedToken->tokenEntry.pSymbol = NULL;
//		pMatchedToken->tokenEntry.tokenType = INT_NUM;
//		return TRUE;
//	}
//
//	return FALSE;
//}

boolean SGSScriptParser::TryParseHexInteger(const char* pStr, WordMatchEntry* pMatchedToken)
{
	uint curLength = 0;

	if(pStr[curLength] == '0')
	{
		curLength++;
		if(pStr[curLength] == 'x' || pStr[curLength] == 'X')
			curLength++;
		else
			return FALSE;

		if(IsHexNumber(pStr[curLength]))
			curLength++;
		else
			return FALSE;

		while(!IsWhitespaceAndNull(pStr[curLength]) && !IsOperator(pStr[curLength]))
		{
			if(IsHexNumber(pStr[curLength]))
				curLength++;
			else
				return FALSE;
		}

		pMatchedToken->strLen = curLength;
		pMatchedToken->tokenEntry.pSymbol = pStr;
		pMatchedToken->tokenEntry.tokenType = HEX_NUM;
		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryParseIntegerOrFloat(const char* pStr, WordMatchEntry* pMatchedToken)
{
	uint curLength = 0;

	if(IsNumber(pStr[curLength]))
	{
		curLength++;
		boolean isDecimalFound = FALSE;
		boolean isFloatFound = FALSE;
		while((!IsWhitespaceAndNull(pStr[curLength]) && !IsOperator(pStr[curLength])) || 
			  (!isDecimalFound && pStr[curLength] == '.') ||
			  (!isFloatFound && (pStr[curLength] == 'f' || pStr[curLength] == 'F')))			   
		{
			if( (!isDecimalFound && pStr[curLength] == '.') )
			{
				isDecimalFound = TRUE;
			}
			else if( (!isFloatFound && (pStr[curLength] == 'f' || pStr[curLength] == 'F')) )
			{
				isFloatFound = TRUE;
			}
			else if(IsNumber(pStr[curLength]))
			{
			
			}
			else
			{
				return FALSE;
			}

			curLength++;
		}

		if(isDecimalFound)
		{
			pMatchedToken->strLen = curLength;
			pMatchedToken->tokenEntry.pSymbol = pStr;
			pMatchedToken->tokenEntry.tokenType = FLOAT_NUM;
			return TRUE;
		}
		else if(!isFloatFound)
		{
			pMatchedToken->strLen = curLength;
			pMatchedToken->tokenEntry.pSymbol = pStr;
			pMatchedToken->tokenEntry.tokenType = INT_NUM;
			return TRUE;
		}
	}

	return FALSE;
}

boolean SGSScriptParser::TryParseNumber(const char* pStr, WordMatchEntry* pMatchedToken)
{	
	if(TryParseIntegerOrFloat(pStr, pMatchedToken))
	{
		return TRUE;
	}
	else if(TryParseHexInteger(pStr, pMatchedToken))
	{
		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryParseNumberOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken)
{
	WordMatchEntry defEntry;
	boolean isDefinePushed;
	boolean isIDFound = FALSE;
	do
	{
		isDefinePushed = FALSE;

		isIDFound = TryParseIdentifier(context.GetCurStr(), &defEntry);
		if(isIDFound)
		{
			isDefinePushed = TryPushDefineParameter(context, defEntry.tokenEntry.pSymbol, defEntry.strLen);
			SkipWhitespaceAndComments(context);
		}

	} while(isDefinePushed);

	return TryParseNumber(context.GetCurStr(), pMatchedToken);
}

boolean SGSScriptParser::TryParseIdentifier(const char* pStr, WordMatchEntry* pMatchedToken)
{
	uint curLength = 0;
	if(IsLetterOrUnderscore(pStr[curLength]))
	{
		// Find the longest unbroken string that counts as an identifier
		curLength++;
		while(IsLetterOrUnderscore(pStr[curLength]) || IsNumber(pStr[curLength]))
		{
			curLength++;
		}

		pMatchedToken->tokenEntry.pSymbol = pStr;
		pMatchedToken->tokenEntry.tokenType = IDENTIFIER;;
		pMatchedToken->strLen = curLength;

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryParseIdentifierOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken)
{
	WordMatchEntry entry;
	boolean isIDFound = FALSE;
	boolean isDefinePushed;
	do
	{
		isDefinePushed = FALSE;

		isIDFound = TryParseIdentifier(context.GetCurStr(), &entry);
		if(isIDFound)
		{
			isDefinePushed = TryPushDefineParameter(context, entry.tokenEntry.pSymbol, entry.strLen);
			SkipWhitespaceAndComments(context);
		}

	} while(isDefinePushed);

	if(isIDFound)
		*pMatchedToken = entry;

	return isIDFound;
}

boolean SGSScriptParser::TryParseStringLiteral(const char* pStr, WordMatchEntry* pMatchedToken)
{
	WordMatchEntry entry;
	uint curLength = 0;
	if(TryParseOperator(pStr + curLength, &entry))
	{
		if(entry.tokenEntry.tokenType == QUOTE)
		{
			curLength += entry.strLen;

			boolean isFound = FALSE;
			while(pStr[curLength] && !IsNewline(pStr[curLength]))
			{
				if(TryMatchToken(pStr + curLength, m_Tokens[QUOTE].strLen, m_Tokens[QUOTE]))
				{
					isFound = TRUE;
					pMatchedToken->strLen = curLength + 1;
					_DEBUG_ASSERT(pMatchedToken->strLen >= 2);
					pMatchedToken->tokenEntry.tokenType = STRING_LITERAL;
					pMatchedToken->tokenEntry.pSymbol = pStr;
					return TRUE;
				}

				curLength++;
			}
		}
	}

	return FALSE;
}

boolean SGSScriptParser::TryParseStringLiteralOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken)
{
	WordMatchEntry defEntry;
	boolean isDefinePushed;
	boolean isIDFound = FALSE;
	do
	{
		isDefinePushed = FALSE;
		isIDFound = TryParseIdentifier(context.GetCurStr(), &defEntry);
		if(isIDFound)
		{
			isDefinePushed = TryPushDefineParameter(context, defEntry.tokenEntry.pSymbol, defEntry.strLen);
			SkipWhitespaceAndComments(context);
		}

	} while(isDefinePushed);

	return TryParseStringLiteral(context.GetCurStr(), pMatchedToken);
}

void SGSScriptParser::ExpectDefineParameter(SGSParseContext& context)
{
	uint curLength = 0;
	WordMatchEntry idEntry;
	if(TryParseIdentifier(context.GetCurStr(), &idEntry))
	{
		curLength = idEntry.strLen;
		
		//context.AdvanceAndPopCurStr(idEntry.strLen);
		//SkipWhitespaceAndComments(context);

		// Parse line
		uint defineLength = 0;
		while(context.GetCurStr()[curLength + defineLength] && !IsNewline(context.GetCurStr()[curLength + defineLength]))
		{
			defineLength++;	
		}

		if(defineLength > 0)
		{
			IByteBufferPtr pDefineData = _NEW CByteBuffer(defineLength+1);
			AppendData(pDefineData, context.GetCurStr() + curLength, defineLength);
			AppendData(pDefineData, (char)0);

			CRCDataPtrKey key;
			key.Set(idEntry.tokenEntry.pSymbol, idEntry.strLen);

			context.defines.Add(key, pDefineData);
		}

		context.AdvanceAndPopCurStr(curLength + defineLength);
	}
	else
	{
		context.PushErrorStr(_W("Expected identifier after define keyword"));
	}
}

boolean SGSScriptParser::TryPushDefineParameter(SGSParseContext& context, const char* pIdentifierStr, uint length)
{
	CRCDataPtrKey key;
	key.Set(pIdentifierStr, length);

	IByteBufferPtr* ppDefineStr = context.defines.Get(key);
	if(ppDefineStr)
	{
		context.AdvanceAndPopCurStr(length);
		context.PushAndSetCurStr((const char*) (*ppDefineStr)->GetData(), (*ppDefineStr)->GetDataLength()-1, NULL, 0);
		return TRUE;
	}

	return FALSE;
}

void SGSScriptParser::ExpectIncludeParameter(SGSParseContext& context)
{
	WordMatchEntry strEntry;
	if(TryParseStringLiteralOrDefine(context, &strEntry))
	{
		// Advance past current string literal
		context.AdvanceAndPopCurStr(strEntry.strLen);

		// Push include file
		wchar filePath[256];
		uint filePathLength = strEntry.strLen - 2;
		_DEBUG_ASSERT(filePathLength < 256);
		_LOOPi(filePathLength)
		{
			filePath[i] = (wchar) strEntry.tokenEntry.pSymbol[i+1];
		}

		filePath[filePathLength] = 0; 

		CRCDataPtrKey key;
		key.Set(filePath, filePathLength * sizeof(wchar));

		IByteBufferPtr* ppIncludeData = context.includes.Get(key);
		if(!ppIncludeData)
		{
			IByteBufferPtr pIncludeData = context.pAppCallback->GetFileData(filePath);
			if(pIncludeData)
			{
				// Append a null string term
				AppendData(pIncludeData, (char)0);
				context.includes.Add(key, pIncludeData);
				ppIncludeData = context.includes.Get(key);
			}
		}
			
		if(ppIncludeData)
		{
			_DEBUG_ASSERT(*ppIncludeData);
			context.PushAndSetCurStr((const char*) (*ppIncludeData)->GetData(), (*ppIncludeData)->GetDataLength(), &strEntry.tokenEntry.pSymbol[1], filePathLength);
		}
		else
		{
			context.PushErrorStr(_W("File from include path not found"));
		}
	}
	else
	{
		context.PushErrorStr(_W("Expected string literal include path"));
	}
}

boolean SGSScriptParser::TryExpectPreprocessor(SGSParseContext& context)
{
	WordMatchEntry matchToken;
	if(TryParseOperator(context.GetCurStr(), &matchToken))
	{
		if(matchToken.tokenEntry.tokenType == PREPROCESSOR_COM)
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);

			if(TryParseIdentifierOrKeyword(context.GetCurStr(), &matchToken))
			{
				context.AdvanceAndPopCurStr(matchToken.strLen);
				SkipWhitespaceAndComments(context);

				switch(matchToken.tokenEntry.tokenType)
				{
				case PREPRO_DEFINE:
					{
						ExpectDefineParameter(context);
						break;
					}
				case PREPRO_INCLUDE:
					{
						ExpectIncludeParameter(context);
						break;
					}
				default:
					{
						// Error
						context.PushErrorStr(_W("Undefined preprocessor token"));
						break;
					}
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

boolean SGSScriptParser::TryExpectFunction(SGSParseContext& context, SGSFunctionEntry& entry)
{
	WordMatchEntry matchToken;
	if(TryParseIdentifierOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == IDENTIFIER))
	{
		SGSParseStack savedStack = context.stack;

		entry.pFnName = matchToken.tokenEntry.pSymbol;
		entry.fnNameLength = matchToken.strLen;
		entry.numParameters = 0;

		context.AdvanceAndPopCurStr(matchToken.strLen);
		SkipWhitespaceAndComments(context);
		if(TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == GROUP_BEGIN))
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);

			while(TryParseIdentifierOrKeywordOrDefine(context, &matchToken) ||
				  TryParseNumberOrDefine(context, &matchToken) ||
				  TryParseStringLiteralOrDefine(context, &matchToken))
			{
				_DEBUG_ASSERT(entry.numParameters < SGSFunctionEntry::MAX_NUM_PARAMETERS);
				
				switch(matchToken.tokenEntry.tokenType)
				{
				case STRING_LITERAL:
					{
						IByteBufferPtr pStrBuf = _NEW CByteBuffer(matchToken.strLen-2+1);
						AppendData(pStrBuf, (const void*)(matchToken.tokenEntry.pSymbol+1), matchToken.strLen-2);
						AppendData(pStrBuf, (char)0);
						entry.parameters[entry.numParameters].SetString(pStrBuf);
						break;
					}

				case FLOAT_NUM:
					entry.parameters[entry.numParameters].SetFloat((float) atof(matchToken.tokenEntry.pSymbol));
					break;

				case INT_NUM:
					entry.parameters[entry.numParameters].SetInt((int) atoi(matchToken.tokenEntry.pSymbol));
					break;

				case TRUE_BOOL:
					entry.parameters[entry.numParameters].SetBool(TRUE);
					break;

				case FALSE_BOOL:
					entry.parameters[entry.numParameters].SetBool(FALSE);
					break;

				default: _DEBUG_ASSERT(FALSE); break;
				}

				entry.numParameters++;
				context.AdvanceAndPopCurStr(matchToken.strLen);
				SkipWhitespaceAndComments(context);
						
				if(TryParseOperatorOrDefine(context, &matchToken))
				{
					if(matchToken.tokenEntry.tokenType == SEPARATOR_OP)
					{
						context.AdvanceAndPopCurStr(matchToken.strLen);
						SkipWhitespaceAndComments(context);
					}
					// else could be function end
				}
				else
				{
					context.PushErrorStr(_W("Function separator expected"));
					return FALSE;
				}
			}
					
			if(TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == GROUP_END))
			{
				context.AdvanceAndPopCurStr(matchToken.strLen);
				//SkipWhitespaceAndComments(context);
				return TRUE;
			}
			else
			{
				context.PushErrorStr(_W("Function end expected"));
			}
		}
		else
		{
			// Restore stack
			context.stack = savedStack;
		}
	}

	return FALSE;
}

boolean SGSScriptParser::TryExpectCodeBlock(SGSParseContext& context, IByteBuffer** ppCode)
{
	WordMatchEntry matchToken;
	if(TryParseIdentifierOrKeywordOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == CODE_BLOCK))
	{
		context.AdvanceAndPopCurStr(matchToken.strLen);
		SkipWhitespaceAndComments(context);
		if(TryParseOperatorOrDefine(context, &matchToken))
		{
			if(matchToken.tokenEntry.tokenType == BLOCK_BEGIN)
			{
				context.AdvanceAndPopCurStr(matchToken.strLen);
				//SkipWhitespaceAndComments(context);

				// Parse block inline
				int numCloses = 0;
				int lengthToCopy = 0;
				IByteBufferPtr pData = _NEW CByteBuffer(1024);
				while(context.GetCurStr())
				{
					// Just in case code uses same block begin/end syntax, we need to parse them to make sure they are equal
					if(TryMatchToken(context.GetCurStr(), m_Tokens[BLOCK_BEGIN].strLen, m_Tokens[BLOCK_BEGIN]))
					{
						numCloses++;
						AppendData(pData, (const void*) context.GetCurStr(), m_Tokens[BLOCK_BEGIN].strLen);
						context.AdvanceAndPopCurStr(m_Tokens[BLOCK_BEGIN].strLen);
					}
					else if(TryMatchToken(context.GetCurStr(), m_Tokens[BLOCK_END].strLen, m_Tokens[BLOCK_END]))
					{
						if(numCloses > 0)
						{
							numCloses--;
							AppendData(pData, (const void*) context.GetCurStr(), m_Tokens[BLOCK_END].strLen);
							context.AdvanceAndPopCurStr(m_Tokens[BLOCK_END].strLen);
						}
						else
							break;
					}
					else
					{
						AppendData(pData, context.GetCurStr()[0]);
						context.AdvanceAndPopCurStr(1);
					}
				}

				if(numCloses == 0 && context.GetCurStr() && TryMatchToken(context.GetCurStr(), m_Tokens[BLOCK_END].strLen, m_Tokens[BLOCK_END]))
				{
					context.AdvanceAndPopCurStr(m_Tokens[BLOCK_END].strLen);
					*ppCode = pData;
					(*ppCode)->AddRef();
				}
				else
				{
					context.PushErrorStr(_W("Invalid number of begin/end in code block"));
				}
				
			}
			else if(matchToken.tokenEntry.tokenType == EQUAL_OP)
			{
				context.AdvanceAndPopCurStr(matchToken.strLen);
				SkipWhitespaceAndComments(context);
	
				SGSExpression expr;
				if(TryExpectExpression(context, expr))
				{
					SGSValue val = expr.Resolve();
					switch(val.type)
					{
					case DATA_VAL:						
					case STRING_VAL:
						{
							*ppCode = val.pData;
							if(val.pData)
								(*ppCode)->AddRef();
	
							break;
						}
					}

					if(*ppCode == NULL)
					{
						context.PushErrorStr(_W("Invalid return type. String or data expected"));
					}
				}
				else
				{
					context.PushErrorStr(_W("Expression expected"));
				}
			}
			else
			{
				context.PushErrorStr(_W("Code block or expression expected"));
			}
		}
		else
		{
			context.PushErrorStr(_W("Code block or expression expected"));
		}

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryExpectExpression(SGSParseContext& context, SGSExpression& expr)
{
	uint isFinished = FALSE;
	int numParsed = 0;
	while(!isFinished)
	{
		SkipWhitespaceAndComments(context);

		WordMatchEntry matchToken;
		SGSValue valEntry;
		SGSFunctionEntry fnEntry;

		if(TryExpectFunction(context, fnEntry))
		{			
			int numRes = 1;
			if(ExecuteFunction(context, fnEntry, &valEntry, numRes))
			{
				if(numRes > 0)
				{
					expr.PushValue(valEntry);
				}
			}
			else
			{
				// Error already in ExecuteFunction
				isFinished = TRUE;
			}

			numParsed++;
		}
		else if(TryExpectValue(context, valEntry))
		{
			expr.PushValue(valEntry);			
			numParsed++;
		}
		else if(TryParseOperatorOrDefine(context, &matchToken))
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);

			switch(matchToken.tokenEntry.tokenType)
			{
			case ADD_OP:	expr.PushAdd();		break;
			case MINUS_OP:	expr.PushMinus();	break;
			case MUL_OP:	expr.PushMul();		break;
			case DIV_OP:	expr.PushDiv();		break;
			case OR_OP:		expr.PushOr();		break;
			case AND_OP:	expr.PushAnd();		break;
			case BOOLEAN_OR:	expr.PushLogOr();	break;
			case BOOLEAN_AND:	expr.PushLogAnd();	break;
			case GROUP_BEGIN:	expr.PushExprBegin(); break;
			case GROUP_END:		expr.PushExprEnd();	break;
			case STATEMENT_END_OP: isFinished = TRUE; break;
				
			default:	context.PushErrorStr(_W("Unknown expression operator")); isFinished = TRUE; break;
			}

			numParsed++;
		}
		else
		{
			// It's an error if we are already parsing the expression
			if(numParsed > 0)
				context.PushErrorStr(_W("Unknown expression. Operator or value or function expected")); 

			isFinished = TRUE;
		}
	}

	if(numParsed > 0)
	{
		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryExpectValue(SGSParseContext& context, SGSValue& val)
{
	WordMatchEntry matchToken;
	if(TryParseNumberOrDefine(context, &matchToken))
	{
		char tempStr[256];
		memcpy(tempStr, matchToken.tokenEntry.pSymbol, matchToken.strLen);
		tempStr[matchToken.strLen] = 0;

		switch(matchToken.tokenEntry.tokenType)
		{
		case FLOAT_NUM:	val.SetFloat((float) atof(tempStr)); context.AdvanceAndPopCurStr(matchToken.strLen); SkipWhitespaceAndComments(context); break;
		case INT_NUM:	val.SetInt(atoi(tempStr)); context.AdvanceAndPopCurStr(matchToken.strLen); SkipWhitespaceAndComments(context); break;
		case HEX_NUM:	val.SetInt(strtoul(tempStr, NULL, 16)); context.AdvanceAndPopCurStr(matchToken.strLen); SkipWhitespaceAndComments(context); break;
		default: break;
		}
	}
	else if(TryParseStringLiteralOrDefine(context, &matchToken))
	{
		IByteBufferPtr pStrBuf = _NEW CByteBuffer(matchToken.strLen-2+1);
		AppendData(pStrBuf, (const void*)(matchToken.tokenEntry.pSymbol+1), matchToken.strLen-2);
		AppendData(pStrBuf, (char)0);
		val.SetString(pStrBuf);

		context.AdvanceAndPopCurStr(matchToken.strLen);
		SkipWhitespaceAndComments(context);
	}
	else if(TryParseIdentifierOrKeywordOrDefine(context, &matchToken))
	{
		switch(matchToken.tokenEntry.tokenType)
		{
		case TRUE_BOOL:		val.SetBool(TRUE);	break;
		case FALSE_BOOL:	val.SetBool(FALSE);	break;

		default: context.PushErrorStr(_W("Unexpected value"));
		}

		context.AdvanceAndPopCurStr(matchToken.strLen);
		SkipWhitespaceAndComments(context);
	}
	else 
	{
		return FALSE;
	}

	return TRUE;
}

boolean SGSScriptParser::TryExpectProperty(SGSParseContext& context, SGSProperty& property)
{
	WordMatchEntry matchToken;
	if(TryParseIdentifierOrKeywordOrDefine(context, &matchToken))
	{
		TokenType curType = matchToken.tokenEntry.tokenType;
		
		if(curType == IDENTIFIER)
		{
			IByteBufferPtr pNameBuffer = _NEW CByteBuffer(matchToken.strLen+1);
			AppendData(pNameBuffer, (const void*) matchToken.tokenEntry.pSymbol, matchToken.strLen);
			AppendData(pNameBuffer, (char)0);

			property.pName = pNameBuffer;

			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);
		}
		else
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);

			if((TryParseIdentifierOrKeywordOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == IDENTIFIER)))
			{
				IByteBufferPtr pNameBuffer = _NEW CByteBuffer(matchToken.strLen+1);
				AppendData(pNameBuffer, (const void*) matchToken.tokenEntry.pSymbol, matchToken.strLen);
				AppendData(pNameBuffer, (char)0);

				property.pName = pNameBuffer;

				context.AdvanceAndPopCurStr(matchToken.strLen);
				SkipWhitespaceAndComments(context);
			}
			else
			{
				context.PushErrorStr(_W("Name expected"));
			}
		}

		if(TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == EQUAL_OP))
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);
		}
		else
		{
			context.PushErrorStr(_W("Assignment expected"));
		}

		SGSExpression& expr = property.expr;
		switch(curType)
		{
		case INT_TYPE:		expr.PushCast(INT_VAL); break;
		case FLOAT_TYPE:	expr.PushCast(FLOAT_VAL); break;
		case BOOL_TYPE:		expr.PushCast(BOOL_VAL); break;
		//case STRING_TYPE:	expr.PushCast(STRING_VAL); break;
		case STRING_TYPE:
		case IDENTIFIER:	break;

		default: context.PushErrorStr(_W("Unsupported type")); break;
		}

		expr.PushExprBegin();
		if(TryExpectExpression(context, expr))
		{
			expr.PushExprEnd();
			SGSValue val = expr.Resolve();			
			if(val.type == DATA_VAL && val.pData == NULL)
			{
				// Error
				context.PushErrorStr(_W("Error resolving expression"));
			}
		}
		else
		{
			expr.Reset();
			context.PushErrorStr(_W("Expression expected"));
		}

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryExpectParameterBlock(SGSParseContext& context)
{
	WordMatchEntry matchToken;
	if(TryParseIdentifierOrKeywordOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == PARAMETER_BLOCK))
	{
		SGSParameter parameter;

		context.AdvanceAndPopCurStr(matchToken.strLen);
		SkipWhitespaceAndComments(context);
		if(TryParseIdentifierOrKeywordOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == IDENTIFIER))
		{
			IByteBufferPtr pNameBuffer = _NEW CByteBuffer(matchToken.strLen+1);
			AppendData(pNameBuffer, (const void*) matchToken.tokenEntry.pSymbol, matchToken.strLen);
			AppendData(pNameBuffer, (char)0);

			parameter.SetName(pNameBuffer);

			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);
		}
		else
		{
			context.PushErrorStr(_W("Identifier expected"));
		}

		if(TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == BLOCK_BEGIN))
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);
			
			IByteBuffer* pCodeBuffer = NULL;
			while(true)
			{
				SkipWhitespaceAndComments(context);
				boolean isEnd = TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == BLOCK_END);
				if(isEnd)
				{
					context.AdvanceAndPopCurStr(matchToken.strLen);
					context.PushParameter(parameter);
					break;
				}

				if(!context.GetCurStr())
				{
					context.PushErrorStr(_W("Parameter block end expected"));
					break;
				}

				SGSProperty property;
				if(TryExpectProperty(context, property))
				{
					parameter.AddProperty(property);
				}
				else if(TryExpectPreprocessor(context))
				{

				}
				else
				{
					context.PushErrorStr(_W("Property expected"));
					break;
				}
			}
		}
		else
		{
			context.PushErrorStr(_W("Parameter block begin expected"));
		}

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryExpectShaderBlock(SGSParseContext& context)
{
	WordMatchEntry matchToken;
	if(TryParseIdentifierOrKeywordOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == SHADER_BLOCK))
	{
		IByteBufferPtr pNameBuffer;

		context.AdvanceAndPopCurStr(matchToken.strLen);
		SkipWhitespaceAndComments(context);
		if(TryParseIdentifierOrDefine(context, &matchToken))
		{
			pNameBuffer = _NEW CByteBuffer(matchToken.strLen+1);
			AppendData(pNameBuffer, (const void*) matchToken.tokenEntry.pSymbol, matchToken.strLen);
			AppendData(pNameBuffer, (char)0);
			
			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);
		}
		else
		{
			context.PushErrorStr(_W("Identifier expected"));
		}

		if(TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == BLOCK_BEGIN))
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);

			IByteBufferPtr pCodeBuffer;
			boolean isCodeBlockDefined = FALSE;
			while(true)
			{
				SkipWhitespaceAndComments(context);
				boolean isEnd = TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == BLOCK_END);
				if(isEnd)
				{
					context.AdvanceAndPopCurStr(m_Tokens[BLOCK_END].strLen);
					context.PushShader(pNameBuffer, pCodeBuffer);
					break;
				}

				if(!context.GetCurStr())
				{
					context.PushErrorStr(_W("Shader block end expected"));
					break;
				}

				IByteBuffer* pTempBuf = NULL;
				if(TryExpectCodeBlock(context, &pTempBuf))
				{
					pCodeBuffer = pTempBuf;
					if(pTempBuf)
						pTempBuf->Release();

					if(isCodeBlockDefined)
					{
						context.PushErrorStr(_W("Code block already defined"));
					}

					isCodeBlockDefined = TRUE;
				}
				else if(TryExpectPreprocessor(context))
				{

				}
				else
				{
					context.PushErrorStr(_W("Code block expected"));
					break;
				}
			}
		}
		else
		{
			context.PushErrorStr(_W("Shader block begin expected"));
		}

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryExpectTechniqueBlock(SGSParseContext& context)
{
	WordMatchEntry matchToken;
	if(TryParseIdentifierOrKeywordOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == TECHNIQUE_BLOCK))
	{
		SGSTechnique tech;
		IByteBufferPtr pNameBuffer;

		context.AdvanceAndPopCurStr(matchToken.strLen);
		SkipWhitespaceAndComments(context);
		if(TryParseIdentifierOrDefine(context, &matchToken))
		{
			pNameBuffer = _NEW CByteBuffer(matchToken.strLen+1);
			AppendData(pNameBuffer, (const void*) matchToken.tokenEntry.pSymbol, matchToken.strLen);
			AppendData(pNameBuffer, (char)0);

			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);

			tech.SetName(pNameBuffer);
		}
		else
		{
			context.PushErrorStr(_W("Identifier expected"));
		}

		if(TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == BLOCK_BEGIN))
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);

			while(true)
			{
				SkipWhitespaceAndComments(context);
				boolean isEnd = TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == BLOCK_END);
				if(isEnd)
				{
					context.AdvanceAndPopCurStr(m_Tokens[BLOCK_END].strLen);
					context.PushTechnique(tech);
					break;
				}

				if(!context.GetCurStr())
				{
					context.PushErrorStr(_W("Technique block end expected"));
					break;
				}

				SGSPass pass;
				if(TryExpectPass(context, pass))
				{
					tech.AddPass(pass);
				}
				else if(TryExpectPreprocessor(context))
				{
					
				}
				else
				{
					context.PushErrorStr(_W("Pass block expected"));
					break;
				}
			}
		}
		else
		{
			context.PushErrorStr(_W("Technique block begin expected"));
		}

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::TryExpectPass(SGSParseContext& context, SGSPass& pass)
{
	WordMatchEntry matchToken;
	if(TryParseIdentifierOrKeywordOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == PASS_BLOCK))
	{
		context.AdvanceAndPopCurStr(matchToken.strLen);
		SkipWhitespaceAndComments(context);
		if(TryParseIdentifierOrDefine(context, &matchToken))
		{
			IByteBufferPtr pNameBuffer = _NEW CByteBuffer(matchToken.strLen+1);
			AppendData(pNameBuffer, (const void*) matchToken.tokenEntry.pSymbol, matchToken.strLen);
			AppendData(pNameBuffer, (char)0);

			context.AdvanceAndPopCurStr(matchToken.strLen);
			SkipWhitespaceAndComments(context);

			pass.SetName(pNameBuffer);
		}
		else
		{
			context.PushErrorStr(_W("Identifier expected"));
		}

		if(TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == BLOCK_BEGIN))
		{
			context.AdvanceAndPopCurStr(matchToken.strLen);

			while(true)
			{
				SkipWhitespaceAndComments(context);
				boolean isEnd = TryParseOperatorOrDefine(context, &matchToken) && (matchToken.tokenEntry.tokenType == BLOCK_END);
				if(isEnd)
				{
					context.AdvanceAndPopCurStr(m_Tokens[BLOCK_END].strLen);
					break;
				}

				if(!context.GetCurStr())
				{
					context.PushErrorStr(_W("Pass block end expected"));
					break;
				}

				SGSProperty prop;
				if(TryExpectProperty(context, prop))
				{
					pass.AddProperty(prop);
				}
				else if(TryExpectPreprocessor(context))
				{

				}
				else
				{
					context.PushErrorStr(_W("Property expected"));
					break;
				}
			}
		}
		else
		{
			context.PushErrorStr(_W("Pass block begin expected"));
		}

		return TRUE;
	}

	return FALSE;
}

boolean SGSScriptParser::ExpectProg(SGSParseContext& context)
{
	while(context.GetCurStr() && (context.stack.numErrors == 0))
	{
		SkipWhitespaceAndComments(context);
		boolean IsSucceeded = TryExpectPreprocessor(context);
		if(!IsSucceeded) IsSucceeded = TryExpectShaderBlock(context); 
		if(!IsSucceeded) IsSucceeded = TryExpectParameterBlock(context); 
		if(!IsSucceeded) IsSucceeded = TryExpectTechniqueBlock(context); 
		if(!IsSucceeded) context.PushErrorStr(_W("Unknown command in main block"));
	}

	return context.stack.numErrors == 0;
}

void SGSScriptParser::AddUserFunction(ISGSUserFunction* pFn)
{
	CRCDataPtrKey key;
	key.Set(pFn->GetName(), strlen(pFn->GetName()));

	_DEBUG_ASSERT(m_UserFunctionMap.Get(key) == NULL);
	m_UserFunctionMap.Add(key, pFn);
}

boolean SGSScriptParser::Compile(SGSScript& dest, IByteBuffer* pData, IAppCallback* pAppCallback, const wchar* pFilePath)
{
	m_FromFile.SetAppCallback(pAppCallback);

	SGSParseContext context;
	context.pMainFile = pFilePath ? pFilePath : _W("Mem Buffer");
	context.pAppCallback = pAppCallback;
	context.pTheScript = &dest;

	AppendData(pData, (char) 0);
	context.PushAndSetCurStr((const char*) pData->GetData(), pData->GetDataLength(), NULL, 0);
	if(ExpectProg(context))
	{
		return TRUE;
	}
	else
	{
		_LOOPi(context.stack.numErrors)
		{
			pAppCallback->Log(_W("Error: "));
			pAppCallback->Log(context.stack.pError[i]);
			pAppCallback->Log(_W("\n"));

			wstring errorStr;
			if(context.stack.pErrorTag[i] && context.stack.errorTagLength[i] > 0)
			{
				_LOOPj(context.stack.errorTagLength[i])
				{
					errorStr += (wchar) context.stack.pErrorTag[i][j];
				}
			}
			else
			{
				_DEBUG_ASSERT(context.pMainFile);
				errorStr = context.pMainFile;
			}

			pAppCallback->Log(errorStr.c_str());
			errorStr = _W("(");

			char intStr[64];
			itoa(context.stack.errorLineNum[i], intStr, 10);
			int intLen = strlen(intStr);
			_LOOPj(intLen)
			{
				errorStr += (wchar) intStr[j];
			}

			errorStr += _W(")\n");
			pAppCallback->Log(errorStr.c_str());
		}
	}

	return FALSE;
}

_NAMESPACE_END