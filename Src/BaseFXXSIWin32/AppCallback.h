//==============================================================================
//
//		AppCallback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/9/2008
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

	~AppCallback()
	{
	}

public: 
	IByteBuffer* GetFileData(const wchar* pFileName);
	void Log(const wchar* pMsg);
};

