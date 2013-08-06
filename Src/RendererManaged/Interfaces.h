//==============================================================================
//
//		Interfaces.h
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
	namespace RendererManaged
	{
		ref class Renderer;
		
		public enum struct eRVDUsage
		{
			VDU_POSITION		= _NAMESPACE VDU_POSITION,
			VDU_NORMAL			= _NAMESPACE VDU_NORMAL,
			VDU_TEXCOORDF		= _NAMESPACE VDU_TEXCOORDF,
			VDU_BINORMAL		= _NAMESPACE VDU_BINORMAL,
			VDU_TANGENT			= _NAMESPACE VDU_TANGENT,
			VDU_BLENDINDICES	= _NAMESPACE VDU_BLENDINDICES,
			VDU_BLENDWEIGHT		= _NAMESPACE VDU_BLENDWEIGHT,
			VDU_COLOR			= _NAMESPACE VDU_COLOR,
			
			VDU_TEXCOORDF2		= _NAMESPACE VDU_TEXCOORDF2,
			VDU_TEXCOORDF1		= _NAMESPACE VDU_TEXCOORDF1,
			VDU_TEXCOORDF3		= _NAMESPACE VDU_TEXCOORDF3,
			VDU_TEXCOORDF4		= _NAMESPACE VDU_TEXCOORDF4,
	
			VDU_END				= _NAMESPACE VDU_END,
		};

		public enum struct eRTexFormat
		{
			TEXF_A8R8G8B8	= _NAMESPACE TEXF_A8R8G8B8,
			TEXF_X8R8G8B8	= _NAMESPACE TEXF_X8R8G8B8,
			TEXF_R32F		= _NAMESPACE TEXF_R32F,
			TEXF_G16R16F	= _NAMESPACE TEXF_G16R16F,
			TEXF_DXT1		= _NAMESPACE TEXF_DXT1,
			TEXF_DXT2		= _NAMESPACE TEXF_DXT2,
			TEXF_DXT3		= _NAMESPACE TEXF_DXT3,
			TEXF_DXT4		= _NAMESPACE TEXF_DXT4,
			TEXF_DXT5		= _NAMESPACE TEXF_DXT5,
			TEXF_A8R8		= _NAMESPACE TEXF_A8R8,
			TEXF_R8			= _NAMESPACE TEXF_R8,
			TEXF_A2R10G10B10 = _NAMESPACE TEXF_A2R10G10B10,
			TEXF_END		= _NAMESPACE TEXF_END,
		};

		public enum struct eRDepthFormat
		{
			DF_D32		= _NAMESPACE DF_D32,
			DF_D24S8	= _NAMESPACE DF_D24S8,
			DF_D16		= _NAMESPACE DF_D16,
			DF_END		= _NAMESPACE DF_END,
		};

		public enum struct eRTexUsage
		{
			TEXU_DEFAULT	= _NAMESPACE TEXU_DEFAULT,		
			TEXU_DYNAMIC	= _NAMESPACE TEXU_DYNAMIC,
			TEXU_END		= _NAMESPACE TEXU_END,
		};

		public enum struct eRResourceType
		{
			RT_TEXTURE_2D		= _NAMESPACE RT_TEXTURE_2D,
			RT_EFFECT			= _NAMESPACE RT_EFFECT,
			RT_EFFECT_TEMPLATE	= _NAMESPACE RT_EFFECT_TEMPLATE,
			RT_VERTEX_BUFFER	= _NAMESPACE RT_VERTEX_BUFFER,
			RT_INDEX_BUFFER		= _NAMESPACE RT_INDEX_BUFFER,
			RT_DEPTH_BUFFER		= _NAMESPACE RT_DEPTH_BUFFER,
			RT_TEXTURE_RT		= _NAMESPACE RT_TEXTURE_RT,
			RT_TEXTURE_CUBE		= _NAMESPACE RT_TEXTURE_CUBE,

			RT_UNKNOWN			= _NAMESPACE RT_UNKNOWN,
		};

		public enum struct eRBufferUsage
		{
			BUF_DEFAULT		= _NAMESPACE BUF_DEFAULT,
			BUF_DYNAMIC		= _NAMESPACE BUF_DYNAMIC,
		};

		public enum struct eRPrimitiveType
		{
			PRIM_TRIANGLE	= _NAMESPACE PRIM_TRIANGLE,
			PRIM_LINE		= _NAMESPACE PRIM_LINE,
		};
		
		public enum struct eREffectParamType 
		{
			EPT_TEX2D_P		= _NAMESPACE EPT_TEX2D_P,
			EPT_VOID_P		= _NAMESPACE EPT_VOID_P,
			EPT_MAT44_P		= _NAMESPACE EPT_MAT44_P,
			EPT_MAT33_P		= _NAMESPACE EPT_MAT33_P,
			EPT_MAT22_P		= _NAMESPACE EPT_MAT22_P,
			EPT_VEC4_P		= _NAMESPACE EPT_VEC4_P,
			EPT_VEC3_P		= _NAMESPACE EPT_VEC3_P,
			EPT_VEC2_P		= _NAMESPACE EPT_VEC2_P,
			EPT_FLOAT		= _NAMESPACE EPT_FLOAT,
			EPT_INT			= _NAMESPACE EPT_INT,
			EPT_BOOL		= _NAMESPACE EPT_BOOL,
			EPT_STRING		= _NAMESPACE EPT_STRING,
			//EPT_TEXRT_P		= _NAMESPACE EPT_TEXRT_P,
			EPT_TEXCUBE_P	= _NAMESPACE EPT_TEXCUBE_P,

			EPT_END			= _NAMESPACE EPT_END,
		};

		public enum struct eRCubeFace
		{
			CF_POS_X	= _NAMESPACE CF_POS_X,
			CF_NEG_X	= _NAMESPACE CF_NEG_X,
			CF_POS_Y	= _NAMESPACE CF_POS_Y,
			CF_NEG_Y	= _NAMESPACE CF_NEG_Y,
			CF_POS_Z	= _NAMESPACE CF_POS_Z,
			CF_NEG_Z	= _NAMESPACE CF_NEG_Z,

			CF_NUM_FACES	= _NAMESPACE CF_NUM_FACES,
		};

		public value struct DisplayInfo
		{
			IntPtr		hWnd;
			uint		backBufferWidth;
			uint		backBufferHeight;
			bool		fullScreen;	
		};

		public value struct InitInfo
		{
			DisplayInfo presentInfo;
			CoreManaged::AppCallback^ hApplication;
		};

		public value struct RendererInfo
		{
			//int maxShaderModelVersion;
			float pixelOffset;
		};
		
		public ref class EffectParam
		{
		private:
			REffectParam* m_pImpl;
			
		public:
			EffectParam() { m_pImpl = new REffectParam; }
			~EffectParam() { _DELETE(m_pImpl); }
			!EffectParam() { _DELETE(m_pImpl); }
			
		public:
			REffectParam* GetImpl() { return m_pImpl; }
			
			// TODO: Expand later			
		};
		
		_PROXY_CLASS_BEGIN(VertexBuffer, IRVertexBuffer)
			void SetValidRange(uint startVertex, uint numVertices) { m_pImpl->SetValidRange(startVertex, numVertices); }
			uint GetVertexSize()	{ return m_pImpl->GetVertexSize(); }
			uint GetSize()			{ return m_pImpl->GetSize(); }
			uint GetNumVertices()	{ return m_pImpl->GetNumVertices(); }
			uint GetStartVertex()	{ return m_pImpl->GetStartVertex(); }
			eRBufferUsage GetUsage()	{ return (RendererManaged::eRBufferUsage) m_pImpl->GetUsage(); }
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(VertexBufferGroup, IRVertexBufferGroup)
			void AddVertexBuffer(VertexBuffer^ hVB)		{ m_pImpl->AddVertexBuffer(hVB->GetImpl()); }
			void RemoveVertexBuffer(uint index)			{ m_pImpl->RemoveVertexBuffer(index); }
			VertexBuffer^ GetVertexBuffer(uint index)	{ return gcnew VertexBuffer(m_pImpl->GetVertexBuffer(index)); }
			uint GetNumOfVertexBuffers()				{ return m_pImpl->GetNumOfVertexBuffers(); }
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(IndexBuffer, IRIndexBuffer)
			uint GetSize()			{ return m_pImpl->GetSize(); }
			uint GetNumIndices()	{ return m_pImpl->GetNumIndices(); }
			uint GetStartIndex()	{ return m_pImpl->GetStartIndex(); }
			eRBufferUsage GetUsage()	{ return (RendererManaged::eRBufferUsage) m_pImpl->GetUsage(); }
			void SetValidRange(uint startIndex, uint numIndices)	{ m_pImpl->SetValidRange(startIndex, numIndices); }
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(Effect, IREffect)
			uint GetNumOfTechs()			{ return m_pImpl->GetNumOfTechs(); }
			uint GetNumOfParams()			{ return m_pImpl->GetNumOfParams(); }
			String^ GetTemplateFileName()	{ return gcnew String(m_pImpl->GetTemplateFileName()); }
			EffectParam^ GetParam(uint index)	
			{ 
				EffectParam^ hParam = gcnew EffectParam; 
				*(hParam->GetImpl()) = *(m_pImpl->GetParam(index)); 
				return hParam; 
			}
		
			void SetTemplateFileName(String^ hFileName) 
			{
				 pin_ptr<const wchar> pFileName = PtrToStringChars(hFileName);
				 m_pImpl->SetTemplateFileName(pFileName); 
			}	
		_PROXY_CLASS_END
		
		public interface struct Resource
		{
			_PURE( String^ GetResourceName() )
			_PURE( CoreManaged::CRCDataKey^ GetResourceID() )
			_PURE( void OnLostDevice() )
			_PURE( void OnRestoreDevice() )
			_PURE( void OnResetDevice() )
		};

		public interface struct Texture : public Resource
		{
			_PURE( eRTexUsage GetUsage() )
			_PURE( eRTexFormat GetFormat() )
			_PURE( uint GetBitDepth() )
			_PURE( uint GetMipMapLevels() )
			_PURE( void SetFileName(String^ hString) )
			_PURE( String^ GetFileName() )
		};

		_PROXY_CLASS_BEGIN_INHERIT(TextureRT, Texture, IRTextureRT)
			virtual String^ GetResourceName()	{ return gcnew String(GetImpl()->GetResourceName()); }
			virtual CoreManaged::CRCDataKey^ GetResourceID() 
			{ 
				CoreManaged::CRCDataKey^ hToReturn = gcnew CoreManaged::CRCDataKey();
				hToReturn->Set(GetImpl()->GetResourceID());
				return hToReturn;
			}

			virtual void OnLostDevice()	{ GetImpl()->OnLostDevice(); }
			virtual void OnRestoreDevice()	{ GetImpl()->OnRestoreDevice(); }
			virtual void OnResetDevice()	{ GetImpl()->OnResetDevice(); }
			virtual eRTexUsage GetUsage()	{ return (eRTexUsage) GetImpl()->GetUsage(); }
			virtual eRTexFormat GetFormat()	{ return (eRTexFormat) GetImpl()->GetFormat(); }
			virtual uint GetBitDepth()		{ return GetImpl()->GetBitDepth(); }
			virtual uint GetMipMapLevels()	{ return GetImpl()->GetMipMapLevels(); }
			virtual void SetFileName(String^ hString)
			{
				pin_ptr<const wchar> pFileName = PtrToStringChars(hString);
				GetImpl()->SetFileName(pFileName); 
			}	

			virtual String^ GetFileName()	{ return gcnew String(GetImpl()->GetFileName()); }
			
			uint GetWidth()		{ return GetImpl()->GetWidth(); }
			uint GetHeight()	{ return GetImpl()->GetHeight(); }

			void CopyFromTextureRT(TextureRT^ hRT) { GetImpl()->CopyFromTextureRT( hRT->GetImpl() ); }
			bool LockReadOnlyPrepare()	{ return GetImpl()->LockReadOnlyPrepare() ? true : false; } 
			IntPtr LockReadOnly(uint% pitch)	
			{
				uint readOnlyPitch;
				void* pData = GetImpl()->LockReadOnly(readOnlyPitch);
				pitch = readOnlyPitch;

				return (IntPtr) pData;
			}

			bool UnlockReadOnly()	{ return GetImpl()->UnlockReadOnly() ? true : false; }
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN_INHERIT(TextureCube, Texture, IRTextureCube)
			virtual String^ GetResourceName()	{ return gcnew String(GetImpl()->GetResourceName()); }
			virtual CoreManaged::CRCDataKey^ GetResourceID() 
			{ 
				CoreManaged::CRCDataKey^ hToReturn = gcnew CoreManaged::CRCDataKey();
				hToReturn->Set(GetImpl()->GetResourceID());
				return hToReturn;
			}

			virtual void OnLostDevice()	{ GetImpl()->OnLostDevice(); }
			virtual void OnRestoreDevice()	{ GetImpl()->OnRestoreDevice(); }
			virtual void OnResetDevice()	{ GetImpl()->OnResetDevice(); }
			virtual eRTexUsage GetUsage()	{ return (eRTexUsage) GetImpl()->GetUsage(); }
			virtual eRTexFormat GetFormat()	{ return (eRTexFormat) GetImpl()->GetFormat(); }
			virtual uint GetBitDepth()		{ return GetImpl()->GetBitDepth(); }
			virtual uint GetMipMapLevels()	{ return GetImpl()->GetMipMapLevels(); }
			virtual void SetFileName(String^ hString)
			{
				pin_ptr<const wchar> pFileName = PtrToStringChars(hString);
				GetImpl()->SetFileName(pFileName); 
			}	

			virtual String^ GetFileName()	{ return gcnew String(GetImpl()->GetFileName()); }

			uint GetSize(uint level) { return GetImpl()->GetSize(level); }

			IntPtr Lock(uint level, eRCubeFace face, uint% pitch)
			{
				uint lockPitch;
				void* pData = GetImpl()->Lock(level, (_NAMESPACE eRCubeFace) face, lockPitch, NULL);
				pitch = lockPitch;
				return (IntPtr) pData;
			}

			bool Unlock(uint level, eRCubeFace face)	{ return GetImpl()->Unlock(level, (_NAMESPACE eRCubeFace) face) ? true : false; }
			TextureRT^ GetTextureRT(uint level, eRCubeFace face)	{ return gcnew TextureRT( GetImpl()->GetTextureRT(level, (_NAMESPACE eRCubeFace) face) ); }
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN_NO_REF(ResourceMgr, IRResourceMgr)		
			Effect^ LoadEffect(CoreManaged::CRCDataKey^ hKey,
								CoreManaged::ByteBuffer^ hBuffer,
								array<EffectParam^>^ hConstantParams,
								array<uint>^ hSemantics);
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(Renderer, IRRenderer)			
			bool Initialize(InitInfo hInfo);
			bool SetDisplayInfo(DisplayInfo hInfo);
			DisplayInfo GetDisplayInfo();
			bool BeginRender()	{ return GetImpl()->BeginRender() ? true : false; }
			void EndRender()	{ GetImpl()->EndRender(); }
			void DisplayFrame() { GetImpl()->DisplayFrame(); }

			//IRTextureRT* GetBackBufferColor() )
			//IRDepthBuffer* GetBackBufferDepth() )

			//	_PURE( const RRendererInfo& GetRendererInfo() )
			//	_PURE( const RMetrics& GetMetrics() )

			//	//_PURE( IRPipeline&	GetRPipeline() )
			//	_PURE( IRResourceMgr& GetRResourceMgr() )
			//	_PURE( IAppCallback& GetAppCallback() ) 

			ResourceMgr^ GetResourceMgr() { return gcnew ResourceMgr(&(GetImpl()->GetRResourceMgr())); }			
		_PROXY_CLASS_END
				
		//private interface struct RenderCallbackInterface
		//{
		//	_PURE( void BeginRenderGroup(uint groupNum) )
		//	_PURE( void EndRenderGroup(uint groupNum) )

		//	_PURE( void BeginRenderOp(const RRenderOp* pSortData) )
		//	_PURE( void EndRenderOp(const RRenderOp* pSortData) )
		//};
	}
}