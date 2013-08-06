//==============================================================================
//
//		CRCDataKey.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/4/2009
//
//==============================================================================

#pragma once

namespace ARFManaged
{
	namespace CoreManaged
	{
		public ref class CRCDataKey
		{
		private:
			_NAMESPACE CRCDataKey* m_pDataKey;
			
		public:
			CRCDataKey()
			{
				m_pDataKey = _NEW _NAMESPACE CRCDataKey;
			}
			
			~CRCDataKey()
			{
				_DELETE(m_pDataKey);
			}
			
			!CRCDataKey()
			{
				_DELETE(m_pDataKey);
			}
			
		public:
			void Set(array<byte>^ data)
			{
				if(data && data->Length > 0)
				{
					pin_ptr<byte> pBytes = &data[0];
					m_pDataKey->Set(pBytes, data->Length);
				}
			}

			void Set(const _NAMESPACE CRCDataKey& key)
			{
				m_pDataKey->hashVal = key.hashVal;
				if(m_pDataKey->pHashData->GetBufferSize() < key.pHashData->GetBufferSize())
					m_pDataKey->pHashData->ResizeBuffer(key.pHashData->GetBufferSize());

				memcpy(m_pDataKey->pHashData->GetData(), key.pHashData->GetData(), key.pHashData->GetDataLength());
				m_pDataKey->pHashData->SetDataLength(key.pHashData->GetDataLength());
			}

			_NAMESPACE CRCDataKey* GetImpl() { return m_pDataKey; }
		};
	}
}