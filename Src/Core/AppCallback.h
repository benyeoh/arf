//==============================================================================
//
//		AppCallback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/24/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct IAppCallback
{
	//------------------------------------------------------------------------------
	//	GetFileData()
	//		Gets data from the application given a file name
	//		
	//	Returns: ptr	- A buffer object containing the requested
	//					  file data
	//			 NULL	- If the file is not found or invalid
	//------------------------------------------------------------------------------
	_PURE( IByteBuffer* GetFileData(const wchar* pFileName) )

	//------------------------------------------------------------------------------
	//	Log()
	//		Notifies the application of important (error) messages
	//------------------------------------------------------------------------------
	_PURE( void Log(const wchar* pMsg) )
};

_NAMESPACE_END
