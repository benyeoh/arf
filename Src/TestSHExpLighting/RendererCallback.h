//==============================================================================
//
//		RendererCallback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/30/2007
//
//==============================================================================


#pragma once 

class RendererCallback : public IAppCallback
{
private:

public:
	RendererCallback()
	{
	}

	virtual ~RendererCallback()
	{
	}	

public:

	IByteBuffer* GetFileData(const wchar* pFileName);

	void Log(const wchar* pMsg);
};