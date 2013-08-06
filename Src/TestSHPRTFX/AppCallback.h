//==============================================================================
//
//		AppCallback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/30/2007
//
//==============================================================================

#pragma once 

class AppCallback : public IAppCallback
{
private:

public:
	AppCallback()
	{
	}

	virtual ~AppCallback()
	{
	}	

public:

	IByteBuffer* GetFileData(const wchar* pFileName);

	void Log(const wchar* pMsg);
};