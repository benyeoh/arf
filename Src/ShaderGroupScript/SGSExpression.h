//==============================================================================
//
//		SGSExpression.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 7 July, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SGSExpression
{
	enum eTokenType
	{
		OP_ADD,
		OP_MUL,
		OP_MINUS,
		OP_DIV,
		
		OP_OR,
		OP_AND,
		OP_LOGICAL_OR,
		OP_LOGICAL_AND,

		OP_EXPR_BEGIN,
		OP_EXPR_END,

		OP_CAST_FLOAT,
		OP_CAST_INT,
		OP_CAST_BOOL,

		VALUE,
	};	

	struct Token
	{
		eTokenType type;
		SGSValue val;
	};

	struct Context
	{
		int tokenIndex;
	};

private:
	std::vector<Token> m_Tokens;

public:
	SGSExpression();
	~SGSExpression();

private:
	Token& GetToken(int index);
	uint GetNumOfTokens();
	void AddToken(Token t);

	SGSValue Add(SGSValue& lhs, SGSValue& rhs);
	SGSValue Mul(SGSValue& lhs, SGSValue& rhs);
	SGSValue Minus(SGSValue& lhs, SGSValue& rhs);
	SGSValue Negate(SGSValue& lhs);
	SGSValue Div(SGSValue& lhs, SGSValue& rhs);
	SGSValue And(SGSValue& lhs, SGSValue& rhs);
	SGSValue Or(SGSValue& lhs, SGSValue& rhs);
	SGSValue LogAnd(SGSValue& lhs, SGSValue& rhs);
	SGSValue LogOr(SGSValue& lhs, SGSValue& rhs);

	SGSValue CastBool(SGSValue& lhs);
	SGSValue CastFloat(SGSValue& lhs);
	SGSValue CastInt(SGSValue& lhs);

	SGSValue ResolveNext(Context& context, uint leftBindingPower);
	SGSValue ResolveInfix(Context& context, SGSValue& lhs);
	SGSValue ResolvePrefix(Context& context);

	uint GetBindingPower(Context& context);
	uint GetBindingPower(eTokenType type);

public:
	void Reset();

	void PushAdd();
	void PushMul();
	void PushMinus();
	void PushDiv();
	void PushAnd();
	void PushOr();
	void PushLogAnd();
	void PushLogOr();
	void PushExprBegin();
	void PushExprEnd();
	void PushValue(const SGSValue& value);
	void PushCast(eSGSValueType from);

	SGSValue Resolve();
};

_NAMESPACE_END