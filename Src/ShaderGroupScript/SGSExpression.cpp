//==============================================================================
//
//		SGSExpression.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Wednesday, 9 July, 2014
//
//==============================================================================

#include "ShaderGroupScript.h"

_NAMESPACE_BEGIN

SGSExpression::SGSExpression()
{

}

SGSExpression::~SGSExpression()
{

}

void SGSExpression::PushAdd()
{
	Token t;
	t.type = OP_ADD;
	AddToken(t);
}

void SGSExpression::PushMul()
{
	Token t;
	t.type = OP_MUL;
	AddToken(t);
}

void SGSExpression::PushMinus()
{
	Token t;
	t.type = OP_MINUS;
	AddToken(t);
}

void SGSExpression::PushDiv()
{
	Token t;
	t.type = OP_DIV;
	AddToken(t);
}

void SGSExpression::PushAnd()
{
	Token t;
	t.type = OP_AND;
	AddToken(t);
}

void SGSExpression::PushOr()
{
	Token t;
	t.type = OP_OR;
	AddToken(t);
}

void SGSExpression::PushLogAnd()
{
	Token t;
	t.type = OP_LOGICAL_AND;
	AddToken(t);
}

void SGSExpression::PushLogOr()
{
	Token t;
	t.type = OP_LOGICAL_OR;
	AddToken(t);
}

void SGSExpression::PushExprBegin()
{
	Token t;
	t.type = OP_EXPR_BEGIN;
	AddToken(t);
}

void SGSExpression::PushExprEnd()
{
	Token t;
	t.type = OP_EXPR_END;
	AddToken(t);
}

void SGSExpression::PushValue(const SGSValue& value)
{
	Token t;
	t.type = VALUE;
	t.val = value;
	AddToken(t);
}

void SGSExpression::PushCast(eSGSValueType from)
{
	Token t;
	switch(from)
	{
	case FLOAT_VAL:	t.type = OP_CAST_FLOAT;	break;
	case INT_VAL:	t.type = OP_CAST_INT;	break;
	case BOOL_VAL:	t.type = OP_CAST_BOOL;	break;
	//case STRING_VAL:	break;	// Unsupported for now
	//case DATA_VAL:		break;	// Unsupported for now
	default: _DEBUG_ASSERT(FALSE); break;
	}
	AddToken(t);
}

void SGSExpression::AddToken(Token t)
{
	m_Tokens.push_back(t);
}

SGSExpression::Token& SGSExpression::GetToken(int index)
{
	return m_Tokens[index];
}

uint SGSExpression::GetNumOfTokens()
{
	return m_Tokens.size();
}

void SGSExpression::Reset()
{
	m_Tokens.clear();
}

SGSValue SGSExpression::Add(SGSValue& lhs, SGSValue& rhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetInt(lhs.iVal + rhs.iVal);
					break;
				}
			case FLOAT_VAL:
				{
					toReturn.SetFloat(lhs.iVal + rhs.fVal);
					break;
				}
			}
			break;
		}
	case FLOAT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetFloat(lhs.fVal + rhs.iVal);
					break;
				}
			case FLOAT_VAL:
				{
					toReturn.SetFloat(lhs.fVal + rhs.fVal);
					break;
				}
			}
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::Negate(SGSValue& lhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			toReturn.SetInt(-lhs.iVal);
			break;
		}
	case FLOAT_VAL:
		{
			toReturn.SetFloat(-lhs.fVal);
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::Minus(SGSValue& lhs, SGSValue& rhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetInt(lhs.iVal - rhs.iVal);
					break;
				}
			case FLOAT_VAL:
				{
					toReturn.SetFloat(lhs.iVal - rhs.fVal);
					break;
				}
			}
			break;
		}
	case FLOAT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetFloat(lhs.fVal - rhs.iVal);
					break;
				}
			case FLOAT_VAL:
				{
					toReturn.SetFloat(lhs.fVal - rhs.fVal);
					break;
				}
			}
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::Mul(SGSValue& lhs, SGSValue& rhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetInt(lhs.iVal * rhs.iVal);
					break;
				}
			case FLOAT_VAL:
				{
					toReturn.SetFloat(lhs.iVal * rhs.fVal);
					break;
				}
			}
			break;
		}
	case FLOAT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetFloat(lhs.fVal * rhs.iVal);
					break;
				}
			case FLOAT_VAL:
				{
					toReturn.SetFloat(lhs.fVal * rhs.fVal);
					break;
				}
			}
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::Div(SGSValue& lhs, SGSValue& rhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetInt(lhs.iVal / rhs.iVal);
					break;
				}
			case FLOAT_VAL:
				{
					toReturn.SetFloat(lhs.iVal / rhs.fVal);
					break;
				}
			}
			break;
		}
	case FLOAT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetFloat(lhs.fVal / rhs.iVal);
					break;
				}
			case FLOAT_VAL:
				{
					toReturn.SetFloat(lhs.fVal / rhs.fVal);
					break;
				}
			}
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::Or(SGSValue& lhs, SGSValue& rhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetInt(lhs.iVal | rhs.iVal);
					break;
				}
			}
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::And(SGSValue& lhs, SGSValue& rhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetInt(lhs.iVal & rhs.iVal);
					break;
				}
			}
			break;
		}
	}

	return toReturn;	
}

SGSValue SGSExpression::LogOr(SGSValue& lhs, SGSValue& rhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetBool( (boolean) (lhs.iVal || rhs.iVal) );
					break;
				}
			case BOOL_VAL:
				{
					toReturn.SetBool( ((boolean) lhs.iVal) || rhs.bVal );
					break;
				}
			}
			break;
		}
	case BOOL_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetBool( (boolean) (lhs.bVal || ((boolean) rhs.iVal)) );
					break;
				}
			case BOOL_VAL:
				{
					toReturn.SetBool( (boolean) (rhs.bVal || lhs.bVal) );
					break;
				}
			}
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::LogAnd(SGSValue& lhs, SGSValue& rhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case INT_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetBool( (boolean) (lhs.iVal && rhs.iVal) );
					break;
				}
			case BOOL_VAL:
				{
					toReturn.SetBool( ((boolean) lhs.iVal) && rhs.bVal );
					break;
				}
			}
			break;
		}
	case BOOL_VAL:
		{
			switch(rhs.type)
			{
			case INT_VAL:
				{
					toReturn.SetBool( (boolean) (lhs.bVal && ((boolean) rhs.iVal)) );
					break;
				}
			case BOOL_VAL:
				{
					toReturn.SetBool( (boolean) (rhs.bVal && lhs.bVal) );
					break;
				}
			}
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::CastFloat(SGSValue& lhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case FLOAT_VAL:
		{
			toReturn.SetFloat(lhs.fVal);
			break;
		}
	case INT_VAL:
		{
			toReturn.SetFloat((float) lhs.iVal);
			break;
		}
	case BOOL_VAL:
		{
			toReturn.SetFloat((float) lhs.bVal);
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::CastInt(SGSValue& lhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case FLOAT_VAL:
		{
			toReturn.SetInt((int) lhs.fVal);
			break;
		}
	case INT_VAL:
		{
			toReturn.SetInt(lhs.iVal);
			break;
		}
	case BOOL_VAL:
		{
			toReturn.SetInt((int) lhs.bVal);
			break;
		}
	}

	return toReturn;
}

SGSValue SGSExpression::CastBool(SGSValue& lhs)
{
	SGSValue toReturn;
	switch(lhs.type)
	{
	case FLOAT_VAL:
		{
			toReturn.SetBool((boolean)(lhs.fVal > 0.0f));
			break;
		}
	case INT_VAL:
		{
			toReturn.SetBool((boolean) lhs.iVal);
			break;
		}
	case BOOL_VAL:
		{
			toReturn.SetBool(lhs.bVal);
			break;
		}
	}

	return toReturn;
}

uint SGSExpression::GetBindingPower(Context& context)
{
	if(context.tokenIndex >= (int) GetNumOfTokens())
	{
		return 0;
	}

	return GetBindingPower(GetToken(context.tokenIndex).type);
}

uint SGSExpression::GetBindingPower(eTokenType type)
{
	switch(type)
	{
	case OP_ADD:
	case OP_MINUS:	return 50;
	case OP_MUL:	
	case OP_DIV:	return 100;

	case OP_OR:		
	case OP_AND:	return 20;

	case OP_LOGICAL_OR:		
	case OP_LOGICAL_AND: return 10;
	}

	return 0;
}

SGSValue SGSExpression::ResolvePrefix(Context& context)
{
	int curIndex = context.tokenIndex;
	context.tokenIndex++;

	if(curIndex < (int) GetNumOfTokens())
	{
		switch(GetToken(curIndex).type)
		{
		case VALUE:		return GetToken(curIndex).val;
		case OP_ADD:	return ResolveNext(context, 100);
		case OP_MINUS:	return Negate(ResolveNext(context, 100));
		case OP_CAST_FLOAT:	return CastFloat(ResolveNext(context, 100));
		case OP_CAST_INT:	return CastInt(ResolveNext(context, 100));
		case OP_CAST_BOOL:	return CastBool(ResolveNext(context, 100));
		case OP_EXPR_BEGIN:	
			{
				SGSValue toReturn = ResolveNext(context, 0);

				if(context.tokenIndex < (int) GetNumOfTokens())
				{
					// Skip end token
					if(GetToken(context.tokenIndex).type != OP_EXPR_END)
						break;

					context.tokenIndex++;
				}
				else
					toReturn = SGSValue();

				return toReturn;
			}
		}
	}

	return SGSValue();
}

SGSValue SGSExpression::ResolveInfix(Context& context, SGSValue& lhs)
{
	int curIndex = context.tokenIndex;
	context.tokenIndex++;

	switch(GetToken(curIndex).type)
	{
	case OP_ADD:	return Add(lhs, ResolveNext(context, GetBindingPower(OP_ADD)));
	case OP_MINUS:	return Minus(lhs, ResolveNext(context, GetBindingPower(OP_MINUS)));
	case OP_DIV:	return Div(lhs, ResolveNext(context, GetBindingPower(OP_DIV)));
	case OP_MUL:	return Mul(lhs, ResolveNext(context, GetBindingPower(OP_MUL)));
	case OP_OR:		return Or(lhs, ResolveNext(context, GetBindingPower(OP_OR)));
	case OP_AND:	return And(lhs, ResolveNext(context, GetBindingPower(OP_AND)));
	case OP_LOGICAL_OR:		return LogOr(lhs, ResolveNext(context, GetBindingPower(OP_LOGICAL_OR)));
	case OP_LOGICAL_AND:	return LogAnd(lhs, ResolveNext(context, GetBindingPower(OP_LOGICAL_AND)));
	}

	return SGSValue();
}

SGSValue SGSExpression::ResolveNext(Context& context, uint leftBindingPower)
{
	SGSValue lhs = ResolvePrefix(context);
	uint rightBindingPower = GetBindingPower(context);
	while(leftBindingPower < rightBindingPower)
	{
		lhs = ResolveInfix(context, lhs);
		rightBindingPower = GetBindingPower(context);
	}

	return lhs;
}

SGSValue SGSExpression::Resolve()
{
	Context context;
	context.tokenIndex = 0;
	return ResolveNext(context, 0);
}

_NAMESPACE_END