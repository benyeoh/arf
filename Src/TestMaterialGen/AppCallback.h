//==============================================================================
//
//		AppCallback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/10/2008
//
//==============================================================================

#pragma once 

class AppCallback : public _NAMESPACE IAppCallback
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
	_NAMESPACE IByteBuffer* GetFileData(const wchar* pFileName);

	void Log(const wchar* pMsg);
};