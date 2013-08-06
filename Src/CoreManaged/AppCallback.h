//==============================================================================
//
//		AppCallback.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/29/2008
//
//==============================================================================

#pragma once

namespace ARFManaged
{
	namespace CoreManaged
	{	
		private interface struct AppCallbackInterface
		{
			_PURE( ByteBuffer^ GetFileData(System::String^ hFileName) )
			_PURE( void Log(System::String^ msg) )
		};

		class AppCallbackAdapter : public _NAMESPACE IAppCallback
		{
		private:
			gcroot<AppCallbackInterface^> m_hCallback;

		public:
			AppCallbackAdapter() {}
			~AppCallbackAdapter() {}

		public:
			void SetCallback(AppCallbackInterface^ hCallback) { m_hCallback = hCallback; }

			_NAMESPACE IByteBuffer* GetFileData(const wchar* pFileName)
			{
				System::String^ hFileName = gcnew System::String(pFileName);
				ByteBuffer^ hBuffer = m_hCallback->GetFileData(hFileName);
				return hBuffer->GetImpl();
			}

			void Log(const wchar* pMsg)
			{
				m_hCallback->Log(gcnew System::String(pMsg));
			}	
		};
		
		public ref class AppCallback abstract : public AppCallbackInterface
		{
		private:
			AppCallbackAdapter* m_pAdapter;
		
		public:
			AppCallback()
				: m_pAdapter(NULL)
			{
				m_pAdapter = _NEW AppCallbackAdapter;
				m_pAdapter->SetCallback(this);
			}
			
			~AppCallback()
			{
				_DELETE(m_pAdapter);
			}	
			
			!AppCallback()
			{
				_DELETE(m_pAdapter);
			}
			
		public:
			_NAMESPACE IAppCallback* GetImpl() { return m_pAdapter; }
			
			_PURE( ByteBuffer^ GetFileData(System::String^ hFileName) )
			_PURE( void Log(System::String^ msg) )
		};
	}
}