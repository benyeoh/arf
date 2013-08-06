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
	namespace BaseFXManaged
	{
		public enum struct ParamSemantic
		{	
			// Effect Dynamic Parameter Semantics
			BFX_WORLD						= _NAMESPACE BFX_WORLD,
			BFX_VIEW						= _NAMESPACE BFX_VIEW,
			BFX_PROJ						= _NAMESPACE BFX_PROJ,
			BFX_VIEW_PROJ					= _NAMESPACE BFX_VIEW_PROJ,
			BFX_WORLD_VIEW_PROJ				= _NAMESPACE BFX_WORLD_VIEW_PROJ,
			BFX_DIR_LIGHT					= _NAMESPACE BFX_DIR_LIGHT,
			BFX_HEMI_LIGHT					= _NAMESPACE BFX_HEMI_LIGHT,
			BFX_POINT_LIGHT1				= _NAMESPACE BFX_POINT_LIGHT1,
			BFX_POINT_LIGHT2				= _NAMESPACE BFX_POINT_LIGHT2,
			BFX_POINT_LIGHT3				= _NAMESPACE BFX_POINT_LIGHT3 ,
			BFX_POINT_LIGHT4				= _NAMESPACE BFX_POINT_LIGHT4,
			BFX_SPOT_LIGHT1					= _NAMESPACE BFX_SPOT_LIGHT1,
			BFX_SPOT_LIGHT2					= _NAMESPACE BFX_SPOT_LIGHT2,
			BFX_SPOT_LIGHT3					= _NAMESPACE BFX_SPOT_LIGHT3,
			BFX_SPOT_LIGHT4					= _NAMESPACE BFX_SPOT_LIGHT4,
			BFX_CAMERA_POS_WITH_VIEW_FAR	= _NAMESPACE BFX_CAMERA_POS_WITH_VIEW_FAR,
			BFX_WORLD_VIEW					= _NAMESPACE BFX_WORLD_VIEW,
			BFX_CAMERA_POS_WITH_VIEW_NEAR	= _NAMESPACE BFX_CAMERA_POS_WITH_VIEW_NEAR,

			// Effect Constant Parameter Semantics
			BFX_DIFFUSE_TEX					= _NAMESPACE BFX_DIFFUSE_TEX,
			BFX_GLOSS_MAP_TEX				= _NAMESPACE BFX_GLOSS_MAP_TEX,
			BFX_NORMAL_MAP_TEX				= _NAMESPACE BFX_NORMAL_MAP_TEX,
			BFX_CUBE_ENV_MAP_TEX			= _NAMESPACE BFX_CUBE_ENV_MAP_TEX,
			BFX_PARALLAX_THICKNESS			= _NAMESPACE BFX_PARALLAX_THICKNESS,
			BFX_ALPHA_MODE					= _NAMESPACE BFX_ALPHA_MODE,
			BFX_FRESNEL						= _NAMESPACE BFX_FRESNEL,
			BFX_ENV_MAP_TINT				= _NAMESPACE BFX_ENV_MAP_TINT,
			BFX_EFFECT_TEXT_DESC			= _NAMESPACE BFX_EFFECT_TEXT_DESC,
			BFX_EFFECT_VERTEX_DESC1			= _NAMESPACE BFX_EFFECT_VERTEX_DESC1,
			BFX_EFFECT_VERTEX_DESC2			= _NAMESPACE BFX_EFFECT_VERTEX_DESC2,
			BFX_EFFECT_VERTEX_DESC3			= _NAMESPACE BFX_EFFECT_VERTEX_DESC3,
			BFX_EFFECT_VERTEX_DESC4			= _NAMESPACE BFX_EFFECT_VERTEX_DESC4,
			BFX_EFFECT_VERTEX_DESC5			= _NAMESPACE BFX_EFFECT_VERTEX_DESC5,

			// Utility parameters
			BFXU_RT1						= _NAMESPACE BFXU_RT1,
			BFXU_RT2						= _NAMESPACE BFXU_RT2,
			BFXU_FILTER_TEXTURE_OFFSETS		= _NAMESPACE BFXU_FILTER_TEXTURE_OFFSETS,
			BFXU_FILTER_WEIGHTS				= _NAMESPACE BFXU_FILTER_WEIGHTS,
			BFXU_COLOR_FILL					= _NAMESPACE BFXU_COLOR_FILL,
			BFXU_SCREEN_PIXEL_OFFSET		= _NAMESPACE BFXU_SCREEN_PIXEL_OFFSET,
			BFXU_RT1_SIZE					= _NAMESPACE BFXU_RT1_SIZE,
			BFXU_RT2_SIZE					= _NAMESPACE BFXU_RT2_SIZE,

			BFXU_RT3						= _NAMESPACE BFXU_RT3,
			BFXU_RT4						= _NAMESPACE BFXU_RT4,
			BFXU_RT5						= _NAMESPACE BFXU_RT5,
		};

		ref class MaterialGroup;

		private interface struct MaterialCreateCallbackInterface
		{
			_PURE( Effect^ CreateEffect(MaterialGroup^ hGroup, uint type, uint availability) ) 
		};

		class MaterialCreateCallbackAdapter : public _NAMESPACE IBFXMaterialCreateCallback
		{
		private:
			gcroot<MaterialCreateCallbackInterface^> m_hCallback;

		public:
			MaterialCreateCallbackAdapter() {}
			~MaterialCreateCallbackAdapter() {}

		public:
			void SetCallback(MaterialCreateCallbackInterface^ hCallback) { m_hCallback = hCallback; }

			_NAMESPACE IREffect* CreateEffect(_NAMESPACE IBFXMaterialGroup* pGroup, uint type, uint availability);
		};

		public ref class MaterialCreateCallback abstract : public MaterialCreateCallbackInterface
		{
		private:
			MaterialCreateCallbackAdapter* m_pAdapter;

		public:
			MaterialCreateCallback()
				: m_pAdapter(NULL)
			{
				m_pAdapter = _NEW MaterialCreateCallbackAdapter;
				m_pAdapter->SetCallback(this);
			}

			~MaterialCreateCallback()
			{
				_DELETE(m_pAdapter);
			}	

			!MaterialCreateCallback()
			{
				_DELETE(m_pAdapter);
			}

		public:
			_NAMESPACE IBFXMaterialCreateCallback* GetImpl() { return m_pAdapter; }

			_PURE( Effect^ CreateEffect(MaterialGroup^ hGroup, uint type, uint availability) ) 
		};

		public interface struct RenderCallbackInterface
		{
			_PURE( IBFXRenderCallback* GetRenderCallbackImpl() )
		};

		public interface struct ParamCallbackInterface
		{
			_PURE( IBFXParamCallback * GetParamCallbackImpl() )
		};

		_PROXY_CLASS_BEGIN_OWNER(RenderContainer, BFXRenderContainer)
			void SetRenderCallback(RenderCallbackInterface^ hCallback, uint libIndex) { m_pImpl->SetRenderCallback(hCallback->GetRenderCallbackImpl(), libIndex); }
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN_OWNER(ParamContainer, BFXParamContainer)
			void SetParamCallback(ParamCallbackInterface^ hCallback, uint libIndex) { m_pImpl->SetParamCallback(hCallback->GetParamCallbackImpl(), libIndex); }
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN(Material, IBFXMaterial)
			uint GetNumOfConstantVolatileParams()	{ return m_pImpl->GetNumOfConstantVolatileParams(); }
			uint GetNumOfDynamicVolatileParams()	{ return m_pImpl->GetNumOfDynamicVolatileParams(); }

			uint GetConstantVolatileParamSemantic(uint index)	{ return m_pImpl->GetConstantVolatileParamSemantic(index); }	
			uint GetDynamicVolatileParamSemantic(uint index)	{ return m_pImpl->GetDynamicVolatileParamSemantic(index); }

			//IREffectTemplate* GetEffectTemplate(uint paramAvailability) )
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(MaterialGroupTemplate, IBFXMaterialGroupTemplate)
			String^ GetDesc()		{ return gcnew String(m_pImpl->GetDesc()); }
			String^ GetFileName()	{ return gcnew String(m_pImpl->GetFileName()); }
			
			uint GetNumOfMaterials()					{ return m_pImpl->GetNumOfMaterials(); }
			Material^ GetMaterialByIndex(uint index)	{ return gcnew Material( m_pImpl->GetMaterialByIndex(index) ); }
			Material^ GetMaterialByType(uint type)		{ return gcnew Material( m_pImpl->GetMaterialByType(type) ) ; } 
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN(MaterialGroup, IBFXMaterialGroup)
			Effect^ CreateEffect(uint type, uint availabilityFlag) { return gcnew Effect(m_pImpl->CreateEffect(type, availabilityFlag)); }
			Effect^ GetEffect(uint type, uint availabilityFlag) { return gcnew Effect(m_pImpl->GetEffect(type, availabilityFlag)); }

			MaterialGroupTemplate^ GetTemplate()	{ return gcnew MaterialGroupTemplate(m_pImpl->GetTemplate()); }
			
			uint GetNumOfParams()	{ return m_pImpl->GetNumOfParams(); }
			//_PURE( void GetParam(uint index, REffectParam& param) )
			uint GetSemantic(uint index)	{ return m_pImpl->GetSemantic(index); }
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN_OWNER(Mesh, BFXMesh)
			void SetVBGroup(VertexBufferGroup^ hVBGroup)	{ m_pImpl->pVBGroup = hVBGroup->GetImpl(); }
			void SetIB(IndexBuffer^ hIB)					{ m_pImpl->pIB = hIB->GetImpl(); }
			void SetMatGroup(MaterialGroup^ hMatGroup)		{ m_pImpl->pMatGroup = hMatGroup->GetImpl(); }
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN_OWNER(HemiLight, BFXHemiLight)
			void SetSkyColor(float r, float g, float b) 
			{ 
				m_pImpl->skyColor[0] = r; 
				m_pImpl->skyColor[1] = g;
				m_pImpl->skyColor[2] = b;
			}
			void SetGroundColor(float r, float g, float b)
			{
				m_pImpl->groundColor[0] = r; 
				m_pImpl->groundColor[1] = g;
				m_pImpl->groundColor[2] = b;				
			}
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN_OWNER(DirLight, BFXDirLight)			
			void SetDir(float x, float y, float z)
			{
				m_pImpl->dir[0] = x; 
				m_pImpl->dir[1] = y;
				m_pImpl->dir[2] = z;								
			}
			void SetColor(float r, float g, float b)
			{
				m_pImpl->color[0] = r;
				m_pImpl->color[1] = g;
				m_pImpl->color[2] = b;
				m_pImpl->color[3] = 1.0f;
			}
			void SetInvSize(float invSize)	{ m_pImpl->invSize = invSize; }
		_PROXY_CLASS_END

		_PROXY_CLASS_BEGIN_OWNER(PointLight, BFXPointLight)			
			void SetPos(float x, float y, float z) { m_pImpl->pos[0] = x; m_pImpl->pos[1] = y; m_pImpl->pos[2] = z; }
			void SetColor(float r, float g, float b)	
			{
				m_pImpl->color[0] = r; m_pImpl->color[1] = g; m_pImpl->color[2] = b; m_pImpl->color[3] = 1.0f;
			}
			void SetInvSize(float invSize)		{ m_pImpl->invSize = invSize; }
			void SetInvRange(float invRange)	{ m_pImpl->invRange = invRange; }
		_PROXY_CLASS_END

		//_PROXY_CLASS_BEGIN_OWNER(SpotLight, BFXSpotLight)			
		//	void SetPos(float x, float y, float z) { m_pImpl->pos[0] = x; m_pImpl->pos[1] = y; m_pImpl->pos[2] = z; }
		//	void SetColor(float r, float g, float b) 
		//	{ 
		//		m_pImpl->color[0] = r; m_pImpl->color[1] = g; m_pImpl->color[2] = b; m_pImpl->color[3] = 1.0f;
		//	}
		//	void SetInvSize(float invSize)		{ m_pImpl->invSize = invSize; }
		//	void SetInvRange(float invRange)	{ m_pImpl->invRange = invRange; }
		//	//void SetTexture
		//_PROXY_CLASS_END
				
		_PROXY_CLASS_BEGIN_NO_REF(ResourceMgr, IBFXResourceMgr)
			MaterialGroupTemplate^ CreateMaterialGroupTemplate(CoreManaged::CRCDataKey^ hKey, 
															CoreManaged::ByteBuffer^ hBuffer, 
															String^ hFileName);
			MaterialGroup^ CreateMaterialGroup(CoreManaged::CRCDataKey^ hKey, 
											MaterialGroupTemplate^ hTemplate,
											array<EffectParam^>^ hConstantParams,
											array<uint>^ hSemantics);													
		_PROXY_CLASS_END

		public ref class ParamPool : public ParamCallbackInterface
		{
			_DECL_PROXY_CLASS_IMPL(ParamPool, IBFXParamPool)

			virtual IBFXParamCallback* GetParamCallbackImpl() { return m_pImpl; }

			void SetViewMatrix(MatrixA44f^ hView)	{ m_pImpl->SetViewMatrix( _CAST_MAT44( *(hView->GetImpl()) ) ); }
			void SetProjMatrix(MatrixA44f^ hProj)	{ m_pImpl->SetProjMatrix( _CAST_MAT44( *(hProj->GetImpl()) ) ); }
			void SetWorldMatrix(MatrixA44f^ hWorld)	{ m_pImpl->SetWorldMatrix( _CAST_MAT44( *(hWorld->GetImpl()) ) ); }
			void SetZDistanceFromCamera(float dist) { m_pImpl->SetZDistanceFromCamera(dist); }
			void SetCameraProperties(VecA3f^ hCamPos, float nearPlane, float farPlane)	{ m_pImpl->SetCameraProperties( _CAST_VEC3( *(hCamPos->GetImpl()) ), nearPlane, farPlane ); } 
			void SetHemiLight(HemiLight^ hHemiLight)	{ m_pImpl->SetHemiLight( (hHemiLight->GetImpl()) ); }
			void SetDirLight(DirLight^ hDirLight)		{ m_pImpl->SetDirLight( (hDirLight->GetImpl()) ); }
			//void SetSpotLights(array<SpotLight^>^ hSpotLights)	
			//{ 
			//	BFXSpotLight* pSpotLights[32];
			//	_LOOPi(hSpotLights->Length)
			//	{
			//		pSpotLights[i] = hSpotLights[i]->GetImpl();
			//	}

			//	m_pImpl->SetSpotLights(pSpotLights, hSpotLights->Length);
			//}
			void SetPointLights(array<PointLight^>^ hPointLights)
			{
				BFXPointLight* pPointLights[32];
				_LOOPi(hPointLights->Length)
				{
					pPointLights[i] = hPointLights[i]->GetImpl();
				}

				m_pImpl->SetPointLights( (const BFXPointLight**) pPointLights, hPointLights->Length);
			}

			float GetZDistanceFromCamera() { return m_pImpl->GetZDistanceFromCamera(); }	
		};

		public ref class Pipeline : public RenderCallbackInterface
		{
			_DECL_PROXY_CLASS_IMPL(Pipeline, IBFXPipeline)

			virtual IBFXRenderCallback* GetRenderCallbackImpl() { return m_pImpl; }

			//_PURE( void SetViewport(uint width, uint height) )
			//	_PURE( void AddRenderTarget(IRTextureRT* pTexRT) )
			//	_PURE( void ClearRenderTargets() )

			//	_PURE( void SetDepthBuffer(IRDepthBuffer* pDepth) )

			//	_PURE( void FlushOpaqueGroup() )
			//	_PURE( void FlushAlphaTestedGroup() )
			//	_PURE( void FlushAlphaBlendedGroup() )
		};

		//_PROXY_CLASS_BEGIN_NO_REF(Debug, IBFXDebug)
		//	void SetWireFrame(boolean isEnabled) { m_pImpl->SetWireFrame(isEnabled); }
		//_PROXY_CLASS_END
				
		_PROXY_CLASS_BEGIN(BaseFX, IBFXBaseFX)
		private:
			ResourceMgr^	m_hResource;

		public:
			ResourceMgr^ GetResourceMgr()	{ return m_hResource; }
		
			boolean Initialize(RendererManaged::Renderer^ hRenderer, 
							MaterialGenManaged::MaterialGen^ hMatGen, 
							CoreManaged::AppCallback^ hCallback);
			
			void Render(Mesh^ hMesh, float detailLevel, uint matType, uint libIndex, ParamContainer^ hParamContainer, RenderContainer^ hRenderContainer, MaterialCreateCallback^ hCreateCallback);

		_PROXY_CLASS_END				
	}
}