//==============================================================================
//
//		CoreManaged.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/29/2008
//
//==============================================================================

#pragma once

#pragma managed(push, off)
#include <Core\Core.h>
#include <CoreMemory\CoreMemory.h>

#include <windows.h>

#pragma managed(pop)

#include <vcclr.h>

#using <CoreMemoryManaged.dll> as_friend

_USE_NAMESPACE

#include "Main.h"
#include "ByteBuffer.h"
#include "AppCallback.h"
#include "MD5Digest.h"
#include "CRCDataKey.h"