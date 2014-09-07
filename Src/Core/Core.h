//==============================================================================
//
//		Core.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 05, 2006
//
//==============================================================================

#ifndef _CORE_H
#define _CORE_H

#pragma warning(push)
#pragma warning(disable:4530)

#include <stdlib.h>
#include <stdint.h>

#include "Defines.h"
#include "MemoryBindings.h"

#include <memory>                   
#include <string>
//#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
//#include <utility> 
#include <functional>                
//#include <hash_map>
#include <assert.h>
#include <iterator>           

#pragma warning(pop)

#include "STLDefines.h"
#include "Debug.h"
#include "RefCounted.h"
#include "RefCountSmartPointer.h"
#include "ByteBuffer.h"
#include "CommonUtiltyFns.h"
#include "CRC32.h"
#include "MD5.h"
#include "CRCDataKey.h"
#include "HashFn.h"
#include "DenseHashMap.h"
#include "DenseChainedHashMap.h"
#include "AppCallback.h"
#include "ObjectPool.h"
#include "ObjectPoolAtomic.h"
#include "ObjectPoolGrowable.h"
#include "ObjectPoolResizable.h"
#include "SaveLoadHelperFns.h"
#include "AtomicFns.h"
#include "Mutex.h"
#include "Module.h"

#endif