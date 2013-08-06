//==============================================================================
//
//		RendererFeedback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Friday, May 26, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class RendererFeedback : public IAppCallback
{
public:	
	//boolean GetParameter(uint semantic, REffectParam& toFill);
	//uint GetParamSemanticID(const wchar* pSemantic);
	IByteBuffer* GetFileData(const wchar* pFileName);
	void Log(const wchar* pMsg);
};

_NAMESPACE_END