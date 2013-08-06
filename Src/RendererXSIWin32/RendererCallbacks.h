//==============================================================================
//
//		RendererCallbacks.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/6/2007
//
//==============================================================================

#pragma once

class RendererCallbacks : public IAppCallback
{
private: 

public: 
	RendererCallbacks()
	{
	}

	~RendererCallbacks()
	{
	}

public: 
	IByteBuffer* GetFileData(const wchar* pFileName);
	void Log(const wchar* pMsg);
};

