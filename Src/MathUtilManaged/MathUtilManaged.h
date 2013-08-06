//==============================================================================
//
//		MathUtilManaged.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/26/2009
//
//==============================================================================

#pragma once

#pragma managed(push, off)
#include <Core\Core.h>
#include <CoreMemory\CoreMemory.h>
#include <MathUtil\MathUtil.h>

#include <windows.h>

#pragma managed(pop)

#include <vcclr.h>

// Include proprietary managed
#using <CoreMemoryManaged.dll> as_friend
#using <CoreManaged.dll> as_friend

using namespace System;
using namespace System::Runtime::InteropServices;
//using namespace System::Windows::Forms;

//using namespace Microsoft::DirectX;
//using namespace Microsoft::DirectX::Direct3D;

#define _PROXY_CLASS_BEGIN(classname, nativename) \
	public ref class classname \
	{ \
	private: \
		nativename* m_pImpl; \
		\
	public: \
		classname(nativename* pImpl) : m_pImpl(pImpl) { m_pImpl->AddRef(); } \
		~classname() { _RELEASE(m_pImpl); } \
		!classname() { _RELEASE(m_pImpl); } \
		\
	public: \
		nativename* GetImpl() { return m_pImpl; } \
		bool IsEqual(classname^ other) { return m_pImpl == other->GetImpl(); } \

#define _PROXY_CLASS_END };

#define _PROXY_CLASS_BEGIN_NO_REF(classname, nativename) \
	public ref class classname \
	{ \
	private: \
		nativename* m_pImpl; \
		\
	public: \
		classname(nativename* pImpl) : m_pImpl(pImpl) {} \
		~classname() { } \
		\
	public: \
		nativename* GetImpl() { return m_pImpl; } \
		bool IsEqual(classname^ other) { return m_pImpl == other->GetImpl(); } \

#define _PROXY_CLASS_BEGIN_OWNER(classname, nativename) \
	public ref class classname \
	{ \
	private: \
		nativename* m_pImpl; \
		\
	public: \
		classname() { m_pImpl = _NEW nativename; } \
		~classname() { _DELETE(m_pImpl); } \
		!classname() { _DELETE(m_pImpl); } \
		\
	public: \
		nativename* GetImpl() { return m_pImpl; } \
		bool IsEqual(classname^ other) { return m_pImpl == other->GetImpl(); } \

_USE_NAMESPACE

#include "Main.h"
#include "Interfaces.h"
#include "Ops.h"