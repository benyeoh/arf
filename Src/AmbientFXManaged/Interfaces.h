//================================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/17/2012
//
//================================================================================

#pragma once 

namespace ARFManaged
{
	namespace AmbientFXManaged
	{
		public enum struct ParamSemantic
		{	
			// Effect Dynamic Parameter Semantics
			AFX_AMBIENT_SH_COEFFS = _NAMESPACE AFX_AMBIENT_SH_COEFFS,
			AFX_TEX_R1	= _NAMESPACE AFX_TEX_R1,
			AFX_TEX_R2	= _NAMESPACE AFX_TEX_R2,
			AFX_TEX_R3	= _NAMESPACE AFX_TEX_R3,
			AFX_TEX_R4	= _NAMESPACE AFX_TEX_R4,
			AFX_TEX_G1	= _NAMESPACE AFX_TEX_G1,
			AFX_TEX_G2	= _NAMESPACE AFX_TEX_G2,
			AFX_TEX_G3	= _NAMESPACE AFX_TEX_G3,
			AFX_TEX_G4	= _NAMESPACE AFX_TEX_G4,
			AFX_TEX_B1	= _NAMESPACE AFX_TEX_B1,
			AFX_TEX_B2	= _NAMESPACE AFX_TEX_B2,
			AFX_TEX_B3	= _NAMESPACE AFX_TEX_B3,
			AFX_TEX_B4	= _NAMESPACE AFX_TEX_B4,
			AFX_UNIT_COEFFS	= _NAMESPACE AFX_UNIT_COEFFS,
			AFX_TRANSFER_MATRIX_R = _NAMESPACE AFX_TRANSFER_MATRIX_R,
			AFX_TRANSFER_MATRIX_G = _NAMESPACE AFX_TRANSFER_MATRIX_G,
			AFX_TRANSFER_MATRIX_B = _NAMESPACE AFX_TRANSFER_MATRIX_B,

		};

		_PROXY_CLASS_BEGIN(AmbientFX, IAFXAmbientFX)
			private:
				//ResourceMgr^	m_hResource;

			public:
				//ResourceMgr^	GetResourceMgr()	{ return m_hResource; }

				//boolean Initialize(RendererManaged::Renderer^ hRenderer, 
				//	MaterialGenManaged::MaterialGen^ hMatGen, 
				//	CoreManaged::AppCallback^ hCallback);

				//array<uint>^ Render(Mesh^ hMesh, float detailLevel, uint matType, 
				//	ParamContainer^ hParamContainer, RenderContainer^ hRenderContainer);

		_PROXY_CLASS_END		
	}
}