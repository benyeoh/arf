//==============================================================================
//
//		Ops.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/27/2009
//
//==============================================================================

#pragma once

namespace ARFManaged
{
	namespace MathUtilManaged
	{
		public ref class Ops
		{
		public:
			static void VecCrossNormalize(VecA3f^ hResult, VecA3f^ hV1, VecA3f^ hV2)
			{
				_NAMESPACE VecCrossNormalize(hResult->GetImpl(), hV1->GetImpl(), hV2->GetImpl());
			}
			
			static void VecScalarMult(VecA4f^ hResult, VecA4f^ hV1, float val)
			{
				_NAMESPACE VecScalarMult(hResult->GetImpl(), hV1->GetImpl(), val);
			}
			
			static void VecVecMult(VecA4f^ hResult, VecA4f^ hV1, VecA4f^ hV2)
			{
				_NAMESPACE VecVecMult(hResult->GetImpl(), hV1->GetImpl(), hV2->GetImpl());
			}

			static void VecVecDiv(VecA4f^ hResult, VecA4f^ hV1, VecA4f^ hV2)
			{
				_NAMESPACE VecVecDiv(hResult->GetImpl(), hV1->GetImpl(), hV2->GetImpl());		
			}

			static void VecVecAdd(VecA4f^ hResult, VecA4f^ hV1, VecA4f^ hV2)
			{
				_NAMESPACE VecVecAdd(hResult->GetImpl(), hV1->GetImpl(), hV2->GetImpl());
			}
			
			static void VecVecSub(VecA4f^ hResult, VecA4f^ hV1, VecA4f^ hV2)
			{
				_NAMESPACE VecVecSub(hResult->GetImpl(), hV1->GetImpl(), hV2->GetImpl());
			}

			static void VecVecDot(float% res, VecA3f^ hV1, VecA3f^ hV2)
			{
				float val;
				_NAMESPACE VecVecDot(&val, hV1->GetImpl(), hV2->GetImpl());
				res = val;				
			}

			static VecA3f^ NormalizeVec(VecA3f^ hResult, VecA3f^ hIn)
			{
				_NAMESPACE NormalizeVec(hResult->GetImpl(), hIn->GetImpl());
				return hResult;
			}

			static VecA3f^ NormalizeVec(VecA3f^ hVec)
			{
				_NAMESPACE NormalizeVec(hVec->GetImpl(), hVec->GetImpl());
				return hVec;
			}

			static void VecLength(float% res, VecA3f^ hVec)
			{
				float val;
				_NAMESPACE VecLength(&val, hVec->GetImpl());
				res = val;
			}

			static void NormalizeVecLength(float% length, VecA3f^ hResult, VecA3f^ hIn)
			{
				float val;
				_NAMESPACE NormalizeVecLength(&val, hResult->GetImpl(), hIn->GetImpl());
				length = val;
			}

			static void VecVecSlerp(VecA3f^ hResult, VecA3f^ hV1, VecA3f^ hV2, float t)
			{
				_NAMESPACE VecVecSlerp(hResult->GetImpl(), hV1->GetImpl(), hV2->GetImpl(), t);
			}

			static VecA4f^ TransformVec(MatrixA44f^ hMat, VecA4f^ hVecIn, VecA4f^ hVecOut)
			{
				_NAMESPACE TransformVec(hMat->GetImpl(), hVecIn->GetImpl(), hVecOut->GetImpl());
				return hVecOut;
			}

			static VecA3f^ TransformVec(MatrixA44f^ hMat, VecA3f^ hVecIn, VecA3f^ hVecOut)
			{
				_NAMESPACE TransformVec(hMat->GetImpl(), hVecIn->GetImpl(), hVecOut->GetImpl());
				return hVecOut;
			}

			static MatrixA44f^ MatMatMult(MatrixA44f^ hResult, MatrixA44f^ hLHS, MatrixA44f^ hRHS)
			{
				_NAMESPACE MatMatMult(hResult->GetImpl(), hLHS->GetImpl(), hRHS->GetImpl());
				return hResult;
			}
		};
	}
}