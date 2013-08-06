//==============================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		08/09/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

_DECLARE_BFX_LIB_ID(5, AFX)

// Effect Dynamic Parameter Semantics
enum
{
	AFX_AMBIENT_SH_COEFFS = AFX_EFFECT_PARAM_OFFSET,
	AFX_TEX_R1,
	AFX_TEX_R2,
	AFX_TEX_R3,
	AFX_TEX_R4,
	AFX_TEX_G1,
	AFX_TEX_G2,
	AFX_TEX_G3,
	AFX_TEX_G4,
	AFX_TEX_B1,
	AFX_TEX_B2,
	AFX_TEX_B3,
	AFX_TEX_B4,

	AFX_UNIT_COEFFS,
	AFX_TRANSFER_MATRIX_R,
	AFX_TRANSFER_MATRIX_G,
	AFX_TRANSFER_MATRIX_B,
};

//// Effect Constant Parameter Semantics
//enum
//{
//	AFX_CONSTANT_SEMANTIC_BEGIN = 5001,
//
//	AFX_CONSTANT_SEMANTIC_END = 5005,
//};

const static uint AFX_MAX_SH_BANDS = 4;

_NAMESPACE_END
