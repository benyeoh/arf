/*
Copyright (C) 1999, 2002 Aladdin Enterprises.  All rights reserved.

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

L. Peter Deutsch
ghost@aladdin.com

*/
/* $Id: md5.h,v 1.4 2002/04/13 19:20:28 lpd Exp $ */
/*
Independent implementation of MD5 (RFC 1321).

This code implements the MD5 Algorithm defined in RFC 1321, whose
text is available at
http://www.ietf.org/rfc/rfc1321.txt
The code is derived from the text of the RFC, including the test suite
(section A.5) but excluding the rest of Appendix A.  It does not include
any code or documentation that is identified in the RFC as being
copyrighted.

The original and principal author of md5.h is L. Peter Deutsch
<ghost@aladdin.com>.  Other authors are noted in the change history
that follows (in reverse chronological order):

2002-04-13 lpd Removed support for non-ANSI compilers; removed
references to Ghostscript; clarified derivation from RFC 1321;
now handles byte order either statically or dynamically.
1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5);
added conditionalization for C++ compilation from Martin
Purschke <purschke@bnl.gov>.
1999-05-03 lpd Original version.
*/

#pragma once

_NAMESPACE_BEGIN

/*
* This package supports both compile-time and run-time determination of CPU
* byte order.  If ARCH_IS_BIG_ENDIAN is defined as 0, the code will be
* compiled to run only on little-endian CPUs; if ARCH_IS_BIG_ENDIAN is
* defined as non-zero, the code will be compiled to run only on big-endian
* CPUs; if ARCH_IS_BIG_ENDIAN is not defined, the code will be compiled to
* run on either big- or little-endian CPUs, but will run slightly less
* efficiently on either one than if ARCH_IS_BIG_ENDIAN is defined.
*/

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
	md5_word_t count[2];	/* message length in bits, lsw first */
	md5_word_t abcd[4];		/* digest buffer */
	md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;


#ifdef __cplusplus
extern "C" 
{
#endif

	/* Initialize the algorithm. */
	void md5_init(md5_state_t *pms);

	/* Append a string to the message. */
	void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);

	/* Finish the message and return the digest. */
	void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

struct MD5Data
{
	uint digest[4];
	
	bool operator ==(const MD5Data& rhs) const
	{
		_LOOPi(4)
		{
			uint data = *((uint*)(digest + i));
			uint rhsData =  *((uint*)(rhs.digest + i));
			if(data != rhsData)
				return false;
		}	

		return true;
	}

	bool operator !=(const MD5Data& rhs) const
	{
		return !((*this) == rhs);
	}

	bool operator <(const MD5Data& rhs) const
	{
		_LOOPi(4)
		{
			uint data = *((uint*)(digest + i));
			uint rhsData =  *((uint*)(rhs.digest + i));
			if(data < rhsData)
				return true;
			else
			if(data > rhsData)
				return false;
		}	

		return false;
	}

	bool operator <=(const MD5Data& rhs) const
	{
		_LOOPi(4)
		{
			uint data = *((uint*)(digest + i));
			uint rhsData =  *((uint*)(rhs.digest + i));
			if(data < rhsData)
				return true;
			else
			if(data > rhsData)
				return false;
		}	

		return true;
	}
};

struct MD5Digest
{
	// byte digest[16];
	MD5Data data;
	boolean isFinalized;
	
	MD5Digest()
		: isFinalized(FALSE)
	{
		//md5_init(&state);
	}
	
	MD5Digest(const MD5Digest& other)
	{
		//state = other.state;
		memcpy(data.digest, other.data.digest, 16);
		isFinalized = other.isFinalized;
	}
	
	void Set(const void* pData, uint length);
	
	bool operator ==(const MD5Digest& rhs) const
	{
		if(!isFinalized || !rhs.isFinalized)
			return false;
			
		return data == rhs.data;
	}
	
	bool operator !=(const MD5Digest& rhs) const
	{
		return !((*this) == rhs);
	}
	
	bool operator <(const MD5Digest& rhs) const
	{
		if(!isFinalized || !rhs.isFinalized)
			return false;

		return data < rhs.data;
	}
	
	bool operator <=(const MD5Digest& rhs) const
	{
		if(!isFinalized || !rhs.isFinalized)
			return false;
	
		return data <= data;
	}
		
};


_NAMESPACE_END