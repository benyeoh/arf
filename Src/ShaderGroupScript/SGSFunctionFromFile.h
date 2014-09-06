//==============================================================================
//
//		SGSFunctionFromFile.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 21 August, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SGSFunctionFromFile : public ISGSUserFunction
{
	const static uint NUM_PARAMS = 1;

private:
	IAppCallback* m_pAppCallback;
	eSGSValueType m_ParamSigTypes[NUM_PARAMS];

public:
	SGSFunctionFromFile()
		: m_pAppCallback(NULL)
	{
		m_ParamSigTypes[0] = STRING_VAL;
	}

	~SGSFunctionFromFile()
	{

	}

public:
	void SetAppCallback(IAppCallback* pCallback) { m_pAppCallback = pCallback; }

	const char* GetName() { return "FromFile"; }

	void GetParameterSig(eSGSValueType** ppParamTypes, uint& numParams) { *ppParamTypes = m_ParamSigTypes, numParams = NUM_PARAMS; }
	boolean Execute(SGSValue* parameters, int numParameters, SGSValue* pResults, int& numResults);
};

_NAMESPACE_END