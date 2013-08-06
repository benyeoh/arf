//================================================================================
//
//		AppHelpersManaged.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/21/2012
//
//================================================================================

#pragma once

#pragma managed(push, off)

#include <CoreMemory\CoreMemory.h>
#include <AppHelpers\AppHelpers.h>
#include <windows.h>

#pragma managed(pop)

#include <vcclr.h>

// Include MDX
//#using <Microsoft.DirectX.Direct3D.dll>
//#using <Microsoft.DirectX.Direct3DX.dll>
//#using <System.Windows.Forms.dll>

// Include proprietary managed
#using <CoreManaged.dll> as_friend
#using <CoreMemoryManaged.dll> as_friend
#using <MathUtilManaged.dll> as_friend
#using <RendererManaged.dll> as_friend
//#using <BaseFXManaged.dll> as_friend
//#using <DeferredFXManaged.dll> as_friend
//#using <ShadowFXManaged.dll> as_friend

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace ARFManaged::RendererManaged;
using namespace ARFManaged::CoreManaged;
using namespace ARFManaged::MathUtilManaged;
//using namespace ARFManaged::MaterialGenManaged;

#define _DECL_PROXY_CLASS_IMPL(classname, nativename) \
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

#include "Interfaces.h"
#include "Main.h"