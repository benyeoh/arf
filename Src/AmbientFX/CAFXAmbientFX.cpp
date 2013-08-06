//==============================================================================
//
//		CAFXAmbientFX.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/7/2009
//
//==============================================================================

#include "AmbientFXFramework.h"

_NAMESPACE_BEGIN


boolean	
CAFXAmbientFX::Initialize(IAppCallback& callback)
{
	m_pAppCallback	= &callback;

	m_ResourceMgr.SetAmbientFX(this);
	m_SHToolKit.SetAmbientFX(this);

	return TRUE;
}

void	
CAFXAmbientFX::LerpAmbientSHCoeff(AFXAmbientCoeffs& dest, const AFXAmbientCoeffs& src1, const AFXAmbientCoeffs& src2, float amount)
{
	// TODO: Change this

	gmtl::VecA4f temp1;
	gmtl::VecA4f temp2;
	float src1Lerp = 1.0f - amount;
	VecScalarMult(&temp1, &src1.rTerm1, src1Lerp);
	VecScalarMult(&temp2, &src2.rTerm1, amount);
	VecVecAdd(&dest.rTerm1, &temp1, &temp2);

	VecScalarMult(&temp1, &src1.rTerm2, src1Lerp);
	VecScalarMult(&temp2, &src2.rTerm2, amount);
	VecVecAdd(&dest.rTerm2, &temp1, &temp2);

	VecScalarMult(&temp1, &src1.gTerm1, src1Lerp);
	VecScalarMult(&temp2, &src2.gTerm1, amount);
	VecVecAdd(&dest.gTerm1, &temp1, &temp2);

	VecScalarMult(&temp1, &src1.gTerm2, src1Lerp);
	VecScalarMult(&temp2, &src2.gTerm2, amount);
	VecVecAdd(&dest.gTerm2, &temp1, &temp2);

	VecScalarMult(&temp1, &src1.bTerm1, src1Lerp);
	VecScalarMult(&temp2, &src2.bTerm1, amount);
	VecVecAdd(&dest.bTerm1, &temp1, &temp2);

	VecScalarMult(&temp1, &src1.bTerm2, src1Lerp);
	VecScalarMult(&temp2, &src2.bTerm2, amount);
	VecVecAdd(&dest.bTerm2, &temp1, &temp2);

	VecScalarMult(&temp1, &_CAST_VECA4(src1.rgbTerm), src1Lerp);
	VecScalarMult(&temp2, &_CAST_VECA4(src2.rgbTerm), amount);
	VecVecAdd(&_CAST_VECA4(dest.rgbTerm), &temp1, &temp2);

	//_LOOPi(9)
	//{
	//	dest.color[i][0] = src1.color[i][0] * (1.0f - amount) + src2.color[i][0] * amount;
	//	dest.color[i][1] = src1.color[i][1] * (1.0f - amount) + src2.color[i][1] * amount;
	//	dest.color[i][2] = src1.color[i][2] * (1.0f - amount) + src2.color[i][2] * amount;
	//}
}

_NAMESPACE_END
