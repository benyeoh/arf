//==============================================================================
//
//		SGSValue.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 7 July, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

enum eSGSValueType
{
	INT_VAL,
	FLOAT_VAL,
	BOOL_VAL,
	STRING_VAL,
	DATA_VAL,
};

struct SGSValue
{
	eSGSValueType type;
	union
	{
		int iVal;
		float fVal;
		boolean bVal;
		IByteBuffer* pData;
	};

	SGSValue()
		: pData(NULL)
		, type(DATA_VAL)
	{
	
	}

	SGSValue(const SGSValue& other) 
	{
		pData = NULL;
		type = DATA_VAL;
		*this = other;
	}

	~SGSValue()
	{
		SetData(NULL);
	}

	void SetData(IByteBuffer* pBuffer) 
	{ 
		if(type == STRING_VAL|| type == DATA_VAL)
		{
			if(pData)
				pData->Release();
		}

		if(pBuffer)
			pBuffer->AddRef();

		pData = pBuffer;
		type = DATA_VAL;
	}

	void SetInt(int val) { SetData(NULL); iVal = val; type = INT_VAL; }
	void SetString(IByteBuffer* pString) { SetData(pString); type = STRING_VAL; }
	void SetFloat(float val) { SetData(NULL); fVal = val; type = FLOAT_VAL; }
	void SetBool(boolean val) { SetData(NULL); bVal = val; type = BOOL_VAL; }

	inline SGSValue& operator =(const SGSValue& rhs)
	{
		switch(rhs.type)
		{
		case INT_VAL:		SetInt(rhs.iVal);		break;
		case FLOAT_VAL:		SetFloat(rhs.fVal);		break;
		case BOOL_VAL:		SetBool(rhs.bVal);		break;
		case STRING_VAL:	SetString(rhs.pData);	break;
		case DATA_VAL:		SetData(rhs.pData);		break;

		default: _DEBUG_ASSERT(FALSE);
		}

		return *this;	
	}
};

_NAMESPACE_END