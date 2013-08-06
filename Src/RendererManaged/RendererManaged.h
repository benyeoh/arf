//==============================================================================
//
//		RendererManaged.h
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
#include <MathUtil\MathUtil.h>
#include <Renderer\Renderer.h>

#include <windows.h>
#pragma managed(pop)

#include <vcclr.h>

#using <System.Windows.Forms.dll>
#using <CoreMemoryManaged.dll> as_friend
#using <CoreManaged.dll> as_friend

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;

#define _PROXY_CLASS_BODY(classname, nativename) \
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

#define _PROXY_CLASS_BODY_NO_REF(classname, nativename) \
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

#define _PROXY_CLASS_BEGIN(classname, nativename) \
	public ref class classname \
	_PROXY_CLASS_BODY(classname, nativename)

#define _PROXY_CLASS_BEGIN_INHERIT(classname, inheritname, nativename) \
	public ref class classname : public inheritname \
	_PROXY_CLASS_BODY(classname, nativename)

#define _PROXY_CLASS_BEGIN_NO_REF(classname, nativename) \
	public ref class classname \
	_PROXY_CLASS_BODY_NO_REF(classname, nativename)

#define _PROXY_CLASS_END };



_USE_NAMESPACE

#include "Interfaces.h"
#include "Main.h"
