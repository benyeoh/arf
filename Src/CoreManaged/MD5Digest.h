//==============================================================================
//
//		MD5Digest.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/30/2008
//
//==============================================================================

#pragma once

namespace ARFManaged
{
	namespace CoreManaged
	{
		public ref class MD5Digest
		{
		private:
			_NAMESPACE MD5Digest* m_pMD5;
			
		public:
			MD5Digest()
			{
				m_pMD5 = _NEW _NAMESPACE MD5Digest;
			}
			
			~MD5Digest()
			{
				_DELETE(m_pMD5);
			}
			
			!MD5Digest()
			{
				_DELETE(m_pMD5);
			}
			
		public:
			void Set(array<byte>^ data)
			{
				if(data && data->Length > 0)
				{
					pin_ptr<byte> pBytes = &data[0];
					m_pMD5->Set(pBytes, data->Length);
				}
			}

			_NAMESPACE MD5Digest* GetImpl() { return m_pMD5; }
		};
	}
}