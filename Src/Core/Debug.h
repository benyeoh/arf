//==============================================================================
//
//		Debug.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 05, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

#define _DUMP_LEAKS						((void*)0)
#define _DEBUG_ASSERT( ... )			((void*)0)
#define _DEBUG_TRACE( ... ) 			((void*)0)
#define _DEBUG_LOG( ... )				((void*)0)

//#define _DEBUG_COMPILE_ASSERT( ... )	((void*)0)
#define _DEBUG_COMPILE_ASSERT(expr)	{enum { __CompileTimeAssertFail = 1/((int) !!(expr)) };}

#define _W2(x)			_W(x)
#define __WFILE__		_W2(__FILE__)

#define _TRACE_NOARG	___TraceNoArg
#define _TRACE			___Trace
#define _VARARG			___VarArg

#if defined(_WINDOWS_)
	inline void __cdecl ___TraceNoArg(const wchar* pStr)
	{
		OutputDebugString(pStr);
	}

	inline void __cdecl ___Trace(const wchar *sFormat, ...)
	{
		va_list vl;
		wchar sTraceString[1024];

		va_start(vl, sFormat);
		vswprintf_s(sTraceString, sFormat, vl);
		va_end(vl);
		_DEBUG_ASSERT(wcslen(sTraceString) < 500);
		___TraceNoArg(sTraceString);
	}
#else
	inline void __cdecl ___TraceNoArg(const wchar* pStr)
	{
		fwprintf(stderr, pStr);
	}
	
	inline void __cdecl ___Trace(const wchar *sFormat, ...)
	{
		va_list vl;
		wchar sTraceString[1024];

		va_start(vl, sFormat);
		vswprintf_s(sTraceString, sFormat, vl);
		va_end(vl);
		_DEBUG_ASSERT(wcslen(sTraceString) < 500);
		___TraceNoArg(sTraceString);
	}
#endif

#if defined(_DEBUG)

	#define _BREAK_HERE		_asm { int 3 }

	#undef	_DEBUG_ASSERT
	#undef	_DEBUG_TRACE
	#undef	_DEBUG_LOG
	
	#define _DEBUG_TRACE	___Trace
	
	#if defined(_WINDOWS_)
		#define _DEBUG_ASSERT( expr )  \
		{ \
			if(!(expr)) \
			{ \
				wchar errStr[1024]; \
				swprintf( errStr, sizeof(errStr)-1, _W("%s\nFile: %s\nLine:%d\n\nBreak Now?"), _W(#expr), __WFILE__, __LINE__ ); \
				if( ::MessageBox(NULL, errStr, _W("ASSERTION FAILED!"), MB_OKCANCEL | MB_ICONERROR) == IDOK ) \
				{ \
				_BREAK_HERE; \
				} \
			} \
		}

		#include <crtdbg.h>
		#undef	_DUMP_LEAKS	
		#define _DUMP_LEAKS			_CrtMemDumpAllObjectsSince( NULL );

	#else
		#define _DEBUG_ASSERT( expr )  \
		{ \
			if(!(expr)) \
			{ \
				_BREAK_HERE; \
			} \
		}
	#endif
		
	//#undef _DEBUG_COMPILE_ASSERT
	//#define _DEBUG_COMPILE_ASSERT(expr)	{enum { __CompileTimeAssertFail = 1/((int) !!(expr)) };}

#endif


_NAMESPACE_END