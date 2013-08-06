//================================================================================
//
//		CoreMemoryManaged.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/9/2011
//
//================================================================================

#pragma once

#pragma managed(push, off)
#include <Core\Core.h>
#include <CoreMemory\CoreMemory.h>

#include <windows.h>

#pragma managed(pop)

#include <vcclr.h>

//#using <System.Windows.Forms.dll>
//#using <CoreManaged.dll> as_friend

using namespace System;
using namespace System::Runtime::InteropServices;

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

_USE_NAMESPACE

#include "Interfaces.h"
#include "Main.h"

