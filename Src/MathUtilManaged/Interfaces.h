//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/26/2009
//
//==============================================================================

#pragma once 

namespace ARFManaged
{
	namespace MathUtilManaged
	{
		_PROXY_CLASS_BEGIN_OWNER(VecA3f, gmtl::VecA3f)
			property float X { float get() { return (*m_pImpl)[0]; } void set(float val) { (*m_pImpl)[0] = val; } }
			property float Y { float get() { return (*m_pImpl)[1]; } void set(float val) { (*m_pImpl)[1] = val; } }
			property float Z { float get() { return (*m_pImpl)[2]; } void set(float val) { (*m_pImpl)[2] = val; } }
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN_OWNER(VecA4f, gmtl::VecA4f)
			property float X { float get() { return (*m_pImpl)[0]; } void set(float val) { (*m_pImpl)[0] = val; } }
			property float Y { float get() { return (*m_pImpl)[1]; } void set(float val) { (*m_pImpl)[1] = val; } }
			property float Z { float get() { return (*m_pImpl)[2]; } void set(float val) { (*m_pImpl)[2] = val; } }
			property float W { float get() { return (*m_pImpl)[3]; } void set(float val) { (*m_pImpl)[3] = val; } }
		_PROXY_CLASS_END
		
		_PROXY_CLASS_BEGIN_OWNER(MatrixA44f, gmtl::MatrixA44f)
			property float _11 { float get() { return (*m_pImpl)[0][0]; } void set(float val) { (*m_pImpl)[0][0] = val; } }
			property float _12 { float get() { return (*m_pImpl)[0][1]; } void set(float val) { (*m_pImpl)[0][1] = val; } }
			property float _13 { float get() { return (*m_pImpl)[0][2]; } void set(float val) { (*m_pImpl)[0][2] = val; } }
			property float _14 { float get() { return (*m_pImpl)[0][3]; } void set(float val) { (*m_pImpl)[0][3] = val; } }

			property float _21 { float get() { return (*m_pImpl)[1][0]; } void set(float val) { (*m_pImpl)[0][0] = val; } }
			property float _22 { float get() { return (*m_pImpl)[1][1]; } void set(float val) { (*m_pImpl)[1][1] = val; } }
			property float _23 { float get() { return (*m_pImpl)[1][2]; } void set(float val) { (*m_pImpl)[1][2] = val; } }
			property float _24 { float get() { return (*m_pImpl)[1][3]; } void set(float val) { (*m_pImpl)[1][3] = val; } }

			property float _31 { float get() { return (*m_pImpl)[2][0]; } void set(float val) { (*m_pImpl)[2][0] = val; } }
			property float _32 { float get() { return (*m_pImpl)[2][1]; } void set(float val) { (*m_pImpl)[2][1] = val; } }
			property float _33 { float get() { return (*m_pImpl)[2][2]; } void set(float val) { (*m_pImpl)[2][2] = val; } }
			property float _34 { float get() { return (*m_pImpl)[2][3]; } void set(float val) { (*m_pImpl)[2][3] = val; } }

			property float _41 { float get() { return (*m_pImpl)[3][0]; } void set(float val) { (*m_pImpl)[3][0] = val; } }
			property float _42 { float get() { return (*m_pImpl)[3][1]; } void set(float val) { (*m_pImpl)[3][1] = val; } }
			property float _43 { float get() { return (*m_pImpl)[3][2]; } void set(float val) { (*m_pImpl)[3][2] = val; } }
			property float _44 { float get() { return (*m_pImpl)[3][3]; } void set(float val) { (*m_pImpl)[3][3] = val; } }
		_PROXY_CLASS_END

		//_PROXY_CLASS_BEGIN_OWNER(MatrixA33f, gmtl::MatrixA33f)
		//	property float _11 { float get() { return (*m_pImpl)[0][0]; } void set(float val) { (*m_pImpl)[0][0] = val; } }
		//	property float _12 { float get() { return (*m_pImpl)[0][1]; } void set(float val) { (*m_pImpl)[0][1] = val; } }
		//	property float _13 { float get() { return (*m_pImpl)[0][2]; } void set(float val) { (*m_pImpl)[0][2] = val; } }

		//	property float _21 { float get() { return (*m_pImpl)[1][0]; } void set(float val) { (*m_pImpl)[0][0] = val; } }
		//	property float _22 { float get() { return (*m_pImpl)[1][1]; } void set(float val) { (*m_pImpl)[1][1] = val; } }
		//	property float _23 { float get() { return (*m_pImpl)[1][2]; } void set(float val) { (*m_pImpl)[1][2] = val; } }

		//	property float _31 { float get() { return (*m_pImpl)[2][0]; } void set(float val) { (*m_pImpl)[2][0] = val; } }
		//	property float _32 { float get() { return (*m_pImpl)[2][1]; } void set(float val) { (*m_pImpl)[2][1] = val; } }
		//	property float _33 { float get() { return (*m_pImpl)[2][2]; } void set(float val) { (*m_pImpl)[2][2] = val; } }
		//_PROXY_CLASS_END
	}
}