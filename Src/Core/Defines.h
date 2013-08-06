//==============================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 05, 2006
//
//==============================================================================

#pragma once

typedef unsigned int	uint;
typedef unsigned long	ulong;
typedef	unsigned char	byte;
typedef unsigned short	ushort;

typedef ushort	wchar;
typedef byte	boolean;

#define FALSE	0
#define TRUE	1
#define NULL	0

// Common utility macros
#define _W(str)				L ## str
#define _DELETE(p)			{ if((p)){delete (p); (p)=0;} }
#define _RELEASE(p)			{ if((p)){(p)->Release();} }
#define _DELETE_ARRAY(p)	{ if((p)){delete[](p); (p)=0;} }

#define _LOOP( i, expr )				for( int i=0; i < (int)(expr); ++i )
#define _LOOPi( expr )					_LOOP( i, expr )
#define _LOOPj( expr )					_LOOP( j, expr )
#define _LOOPk( expr )					_LOOP( k, expr )

#define _PURE( fName )							virtual fName = 0;
#define _CHECK_FLAG( flag, val )				if( ((flag) & (val)) )
#define _MAKE_ID( char1, char2, char3, char4 )	(char1 << 24 | char2 << 16 | char3 << 8 | char4)

// Static allocator
#define _IMPL_STATIC_ALLOC( cName, varName, size ) \
 \
static cName s_##varName##Pool[size]; \
static uint s_##varName##Used = 0; \
 \
inline cName* \
StaticGet##varName##() \
{ \
	_DEBUG_ASSERT(s_##varName##Used < size); \
	s_##varName##Used++; \
	return &(s_##varName##Pool[s_##varName##Used-1]); \
} \
 \
inline void \
StaticClear##varName##() { s_##varName##Used = 0; }


// Memory alignment
template<typename T>
struct TypeStride
{
	T t;
	byte b;
};

#define strideof(T) \
 \
( (sizeof(TypeStride<T>) > sizeof(T)) ? \
	sizeof(TypeStride<T>)-sizeof(T) : sizeof(T) )

template<typename T> uint
GetStride(T* pType)
{
	return ( (sizeof(TypeStride<T>) > sizeof(T)) ? sizeof(TypeStride<T>)-sizeof(T) : sizeof(T) );
}

#ifdef _MSC_VER
	#define _ALIGN(val)	__declspec(align(val))
#elif __GNUC__
	#define _ALIGN(val) __attribute__((aligned(val)))
#else
	#define _ALIGN(val)
#endif

// TODO:
#define _CACHE_LINE_SIZE	64

// Namespace
#define _THE_PROJECT_LABEL _W("ARF")
#define _NAMESPACE_BEGIN namespace ARF {
#define _NAMESPACE_END };

#define _USE_NAMESPACE using namespace ARF;

#define _NAMESPACE ARF::

