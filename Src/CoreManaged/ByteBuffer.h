//==============================================================================
//
//		ByteBuffer.h
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
		public ref class ByteBuffer
		{
		private:
			_NAMESPACE IByteBuffer* m_pBuffer;
		
		public:
			ByteBuffer()
			{
				m_pBuffer = _NEW _NAMESPACE CByteBuffer(128);
				m_pBuffer->AddRef();
			}
			
			~ByteBuffer()
			{
				m_pBuffer->Release();
			}
			
			!ByteBuffer()
			{
				m_pBuffer->Release();
			}
			
		public:
			// TODO: Change these to return IntPtr
			array<byte>^ GetData()
			{
				uint dataLength = m_pBuffer->GetDataLength();
				if(dataLength > 0)
				{
					array<byte>^ hToReturn = gcnew array<byte>(dataLength);
					pin_ptr<byte> pBytes = &hToReturn[0];
					memcpy(pBytes, m_pBuffer->GetData(), dataLength);
					return hToReturn;
				}

				return nullptr;			
			}

			void SetData(array<byte>^ hBytes)
			{
				if(hBytes->Length > 0)
				{
					pin_ptr<byte> pBytes = &hBytes[0];
					AppendData(m_pBuffer, pBytes, hBytes->Length);
				}
			}

			uint GetBufferSize() { return m_pBuffer->GetBufferSize(); }
			uint GetDataLength() { return m_pBuffer->GetDataLength(); }

			_NAMESPACE IByteBuffer* GetImpl() { return m_pBuffer; }
		};
	}
}