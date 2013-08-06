//==============================================================================
//
//		STLDefines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/5/2008
//
//==============================================================================

#pragma once

typedef std::basic_string<wchar> wstring;
typedef std::basic_stringstream<wchar> wstringstream;

#define _ITER_STL( type, obj, itr )		for( type::iterator itr = obj.begin(); itr != obj.end(); ++itr )
#define _ITER_STLi( type, obj )			_ITER_STL( type, obj, iitr )
#define _ITER_STLj( type, obj )			_ITER_STL( type, obj, jitr )
#define _ITER_STLk( type, obj )			_ITER_STL( type, obj, kitr )

#define _ITER_STLconst( type, obj, itr )		for( type::const_iterator itr = obj.begin(); itr != obj.end(); ++itr )
#define _ITER_STLconsti( type, obj )			_ITER_STLconst( type, obj, iitr )
#define _ITER_STLconstj( type, obj )			_ITER_STLconst( type, obj, jitr )
#define _ITER_STLconstk( type, obj )			_ITER_STLconst( type, obj, kitr )

#define _STL_ARRAY_UNORDERED_REMOVE( obj, index )		{ obj[index] = obj.back(); obj.pop_back(); }