//==============================================================================
//
//		ScriptParser.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 13 April, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct SGSFunctionEntry;
struct SGSParseContext;

class SGSScriptParser
{
	enum TokenType
	{
		// Keywords
		TOKEN_KEYWORD_BEGIN,

		PREPRO_DEFINE = TOKEN_KEYWORD_BEGIN,
		PREPRO_INCLUDE,
		SHADER_BLOCK,
		ANNOTATION_BLOCK,
		TECHNIQUE_BLOCK,
		CODE_BLOCK,
		PASS_BLOCK,
		PARAMETER_BLOCK,
		INT_TYPE,
		INT2_TYPE,
		INT3_TYPE,
		INT4_TYPE,
		FLOAT_TYPE,
		FLOAT2_TYPE,
		FLOAT3_TYPE,
		FLOAT4_TYPE,
		FLOAT4X4_TYPE,
		BOOL_TYPE,
		STRING_TYPE,
		TRUE_BOOL,
		FALSE_BOOL,

		TOKEN_KEYWORD_END,
		TOKEN_OPERATOR_BEGIN = TOKEN_KEYWORD_END,

		// Operators
		PREPROCESSOR_COM = TOKEN_OPERATOR_BEGIN,
		BLOCK_BEGIN,
		BLOCK_END,
		GROUP_BEGIN,
		GROUP_END,
		ADD_OP,
		MINUS_OP,
		MUL_OP,
		DIV_OP,
		OR_OP,
		AND_OP,
		BOOLEAN_OR,
		BOOLEAN_AND,
		QUOTE,
		GREATER_THAN_OP,
		GREATER_THAN_OR_EQUAL_OP,
		LESS_THAN_OP,
		LESS_THAN_OR_EQUAL_OP,
		EQUAL_OP,
		COMMENT_LINE,
		COMMENT_BLOCK_BEGIN,
		COMMENT_BLOCK_END,
		STATEMENT_END_OP,
		SEPARATOR_OP, 

		TOKEN_OPERATOR_END,

		// Numbers
		INT_NUM = TOKEN_OPERATOR_END,
		HEX_NUM,
		FLOAT_NUM,

		// Identifier
		IDENTIFIER,

		// String
		STRING_LITERAL,
	};

	struct TokenEntry
	{
		const char* pSymbol;
		TokenType tokenType;
	};

	struct WordMatchEntry
	{
		TokenEntry tokenEntry;
		uint strLen;
	};

	// Ascii
	const static uint MAX_NUM_SYMBOLS = 128;
	const static uint MAX_NUM_WORDS = 32;
	const static uint NUM_KEYWORD_OPERATORS = TOKEN_KEYWORD_END - TOKEN_KEYWORD_BEGIN + TOKEN_OPERATOR_END - TOKEN_OPERATOR_BEGIN;

	const static int MAX_NUM_FUNCTION_RESULTS = 16;

private:
	WordMatchEntry m_WordMatch[MAX_NUM_SYMBOLS][MAX_NUM_WORDS];
	uint m_NumTokensWordMatch[MAX_NUM_SYMBOLS];
	WordMatchEntry m_Tokens[NUM_KEYWORD_OPERATORS];

	DenseChainedHashMap<CRCDataPtrKey, ISGSUserFunction*, HashCRCDataPtrKey> m_UserFunctionMap;

	SGSFunctionFromFile m_FromFile;

public:
	SGSScriptParser();
	~SGSScriptParser();

private:
	boolean ExpectProg(SGSParseContext& context);

	boolean TryExpectTechniqueBlock(SGSParseContext& context);
	boolean TryExpectShaderBlock(SGSParseContext& context);
	boolean TryExpectAnnotBlock(SGSParseContext& context);

	boolean TryExpectPreprocessor(SGSParseContext& context);
	void ExpectIncludeParameter(SGSParseContext& context);
	void ExpectDefineParameter(SGSParseContext& context);

	boolean TryPushDefineParameter(SGSParseContext& context, const char* pIdentifierStr, uint length);

	boolean IsOperator(char c);
	boolean IsWhitespace(char c);
	boolean IsWhitespaceAndNull(char c);
	boolean IsHexNumber(char c);
	boolean IsNumber(char c);
	boolean IsLetterOrUnderscore(char c);
	
	boolean ConsumeMultiNewline(SGSParseContext& context);

	boolean TryMatchToken(const char* pStr, uint length, const WordMatchEntry& token);
	void SkipWhitespaceAndComments(SGSParseContext& context);

	boolean ExecuteFunction(SGSParseContext& context, SGSFunctionEntry& entry, SGSValue* pResults, int& numResults);

	boolean TryParseStringLiteralOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken);
	boolean TryParseStringLiteral(const char* pStr, WordMatchEntry* pMatchedToken);
	boolean TryParseIdentifier(const char* pStr, WordMatchEntry* pMatchedToken);
	boolean TryParseIdentifierOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken);
	boolean TryParseIdentifierOrKeywordOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken);
	boolean TryParseIdentifierOrKeyword(const char* pStr, WordMatchEntry* pMatchedToken);
	boolean TryParseNumberOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken);
	boolean TryParseNumber(const char* pStr, WordMatchEntry* pMatchedToken);
	boolean TryParseIntegerOrFloat(const char* pStr, WordMatchEntry* pMatchedToken);
	boolean TryParseHexInteger(const char* pStr, WordMatchEntry* pMatchedToken);
	boolean TryParseOperatorOrDefine(SGSParseContext& context, WordMatchEntry* pMatchedToken);
	boolean TryParseOperator(const char* pStr, WordMatchEntry* pMatchedToken);

	boolean TryExpectPass(SGSParseContext& context, SGSPass& pass);
	boolean TryExpectCodeBlock(SGSParseContext& context, IByteBuffer** ppCode);
	boolean TryExpectFunction(SGSParseContext& context, SGSFunctionEntry& entry);
	boolean TryExpectParameter(SGSParseContext& context, SGSParameter& parameter);
	boolean TryExpectProperty(SGSParseContext& context, SGSProperty& property);
	boolean TryExpectValue(SGSParseContext& context, SGSValue& val);
	boolean TryExpectExpression(SGSParseContext& context, SGSExpression& expr);

public:
	void Initialize();
	void AddUserFunction(ISGSUserFunction* pFn);

	boolean Compile(SGSScript& dest, IByteBuffer* pData, IAppCallback* pAppCallback, const wchar* pFilePath);
};

_NAMESPACE_END