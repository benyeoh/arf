//==============================================================================
//
//		D3DConversion.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

inline eRCubeFace CubeFaceFromD3D(D3DCUBEMAP_FACES face)
{
	switch(face)
	{
		case D3DCUBEMAP_FACE_POSITIVE_X:	return CF_POS_X;
		case D3DCUBEMAP_FACE_NEGATIVE_X:    return CF_NEG_X;
		case D3DCUBEMAP_FACE_POSITIVE_Y:    return CF_POS_Y;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:	return CF_NEG_Y;
		case D3DCUBEMAP_FACE_POSITIVE_Z:	return CF_NEG_Z;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:    return CF_POS_Z;
		
		default: _DEBUG_ASSERT(FALSE);
	}
	
	return CF_NUM_FACES;
}

inline D3DCUBEMAP_FACES CubeFaceToD3D(eRCubeFace face)
{
	switch(face)
	{
	case CF_POS_X:		return D3DCUBEMAP_FACE_POSITIVE_X;
	case CF_NEG_X:		return D3DCUBEMAP_FACE_NEGATIVE_X;
	case CF_POS_Y:		return D3DCUBEMAP_FACE_POSITIVE_Y;
	case CF_NEG_Y:		return D3DCUBEMAP_FACE_NEGATIVE_Y;
	case CF_POS_Z:		return D3DCUBEMAP_FACE_NEGATIVE_Z;	// Flip Z
	case CF_NEG_Z:		return D3DCUBEMAP_FACE_POSITIVE_Z;

	default: _DEBUG_ASSERT(FALSE);
	}

	return D3DCUBEMAP_FACE_FORCE_DWORD;	
}

inline eRTexFormat TexFormatFromD3D(D3DFORMAT format)
{
	switch(format)
	{
		case D3DFMT_A8R8G8B8:	return TEXF_A8R8G8B8;
		case D3DFMT_X8R8G8B8:	return TEXF_X8R8G8B8;
		case D3DFMT_R32F: 		return TEXF_R32F;
		case D3DFMT_G16R16F:	return TEXF_G16R16F;
		case D3DFMT_G16R16:		return TEXF_G16R16;
		case D3DFMT_DXT1:		return TEXF_DXT1;
		case D3DFMT_DXT2:		return TEXF_DXT2;
		case D3DFMT_DXT3:		return TEXF_DXT3;
		case D3DFMT_DXT4:		return TEXF_DXT4;
		case D3DFMT_DXT5:		return TEXF_DXT5;
		case D3DFMT_L8:			return TEXF_R8;
		case D3DFMT_A8L8:		return TEXF_A8R8;
		case D3DFMT_A32B32G32R32F:	return TEXF_A32B32G32R32F;
		case D3DFMT_G32R32F:	    return TEXF_G32R32F;
		case D3DFMT_A16B16G16R16F:	return TEXF_A16B16G16R16F;
        case D3DFMT_A16B16G16R16:	return TEXF_A16B16G16R16;
        case D3DFMT_A2R10G10B10 :   return TEXF_A2R10G10B10;

		default:	_DEBUG_ASSERT(FALSE);
	}	
	
	return TEXF_END;
}

inline D3DFORMAT TexFormatToD3D(eRTexFormat format)
{
	switch(format)
	{
		case TEXF_A8R8G8B8:		return D3DFMT_A8R8G8B8;
		case TEXF_X8R8G8B8:		return D3DFMT_X8R8G8B8;
		case TEXF_R32F:			return D3DFMT_R32F;
		case TEXF_G16R16F:		return D3DFMT_G16R16F;
		case TEXF_G16R16:		return D3DFMT_G16R16;
		case TEXF_DXT1:			return D3DFMT_DXT1;
		case TEXF_DXT2:			return D3DFMT_DXT2;
		case TEXF_DXT3:			return D3DFMT_DXT3;
		case TEXF_DXT4:			return D3DFMT_DXT4;
		case TEXF_DXT5:			return D3DFMT_DXT5;
		case TEXF_A8R8:			return D3DFMT_A8L8;
		case TEXF_A32B32G32R32F:	return D3DFMT_A32B32G32R32F;		
		case TEXF_G32R32F:			return D3DFMT_G32R32F;
		case TEXF_A16B16G16R16F:	return D3DFMT_A16B16G16R16F;
        case TEXF_A16B16G16R16:	return D3DFMT_A16B16G16R16;
        case TEXF_R8:			return D3DFMT_L8;
		case TEXF_A2R10G10B10: return D3DFMT_A2R10G10B10;

		default:	_DEBUG_ASSERT(FALSE);
	}
	
	return D3DFMT_A8R8G8B8;
}

inline eRTexUsage TexUsageFromD3D(DWORD usage)
{
	switch(usage)
	{
		case 0:						return TEXU_DEFAULT;
		case D3DUSAGE_DYNAMIC:		return TEXU_DYNAMIC;
		
		default:	_DEBUG_ASSERT(FALSE);
	}
	
	return TEXU_END;
}

inline D3DFORMAT DepthFormatToD3D(eRDepthFormat format)
{
	switch(format)
	{
		case DF_D16:	return D3DFMT_D16;
		case DF_D32:	return D3DFMT_D32;
		case DF_D24S8:	return D3DFMT_D24S8;
		
		default: _DEBUG_ASSERT(FALSE);
	}
	
	return (D3DFORMAT) 0;
}

inline eRDepthFormat DepthFormatFromD3D(D3DFORMAT format)
{
	switch(format)
	{
		case D3DFMT_D16:	return DF_D16;
		case D3DFMT_D32:	return DF_D32;
		case D3DFMT_D24S8:	return DF_D24S8;

		default: _DEBUG_ASSERT(FALSE);
	}

	return DF_END;
}

inline DWORD TexUsageToD3D(eRTexUsage usage)
{
	switch(usage)
	{
		case TEXU_DEFAULT_STAGING:
		case TEXU_DEFAULT:		return 0;
		case TEXU_DYNAMIC:		return D3DUSAGE_DYNAMIC;
		
		default:	_DEBUG_ASSERT(FALSE);
	}

	return 0;
}

inline DWORD BufUsageToD3D(eRBufferUsage usage)
{
	switch(usage)
	{
		case BUF_DEFAULT_STAGING:
		case BUF_DEFAULT:		return D3DUSAGE_WRITEONLY;
		case BUF_DYNAMIC:		return D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
		
		default:	_DEBUG_ASSERT(FALSE);
	}
	
	return 0;
}

inline D3DPOOL BufUsageToD3DPool(eRBufferUsage usage)
{
	switch(usage)
	{
		case BUF_DEFAULT_STAGING:
		case BUF_DEFAULT:
		case BUF_DYNAMIC:		return D3DPOOL_DEFAULT;
	
		default:	_DEBUG_ASSERT(FALSE);
	}
	
	return D3DPOOL_DEFAULT;
}

inline D3DPRIMITIVETYPE PrimitiveTypeToD3D(eRPrimitiveType prim)
{
	switch(prim)
	{
		case PRIM_TRIANGLE: return D3DPT_TRIANGLELIST;
		case PRIM_LINE:		return D3DPT_LINELIST;
		default: _DEBUG_ASSERT(FALSE);
	}
	
	return D3DPT_TRIANGLELIST;
}

inline D3DMULTISAMPLE_TYPE MultiSampleToD3D(uint multisample)
{
	return (D3DMULTISAMPLE_TYPE) multisample;
}

inline uint MultiSampleFromD3D(D3DMULTISAMPLE_TYPE multisample)
{
	return (uint) multisample;
}

//------------------------------------------------------------------------------
//	GMTL Math Library Conversion Fns - In case we need this...
//------------------------------------------------------------------------------

//inline const D3DXQUATERNION* ToD3D(const gmtl::Quatf& from)
//{
//	return (const D3DXQUATERNION*)(from.getData());
//}
//
//// Quaternion Conversions
//inline D3DXQUATERNION& ToD3D(const gmtl::Quatf& from, D3DXQUATERNION& to)
//{
//	to.x = from[0];
//	to.y = from[1];
//	to.z = from[2];
//	to.w = from[3];
//	return to;
//}
//
//inline gmtl::Quatf& ToGMTL(const D3DXQUATERNION& from, gmtl::Quatf& to)
//{
//	to.set(from.x, from.y, from.z, from.w);
//	return to;
//}
//
//// Matrix Conversions
//inline const D3DXMATRIX* ToD3D(const gmtl::Matrix44f& from)
//{
//	return (const D3DXMATRIX*)(from.getData());
//}
//
//inline D3DXMATRIX&	ToD3D(const gmtl::Matrix44f& from, D3DXMATRIX& to)
//{
//	to._11=from[0][0]; to._12=from[0][1]; to._13=from[0][2]; to._14=from[0][3];
//	to._21=from[1][0]; to._22=from[1][1]; to._23=from[1][2]; to._24=from[1][3];
//	to._31=from[2][0]; to._32=from[2][1]; to._33=from[2][2]; to._34=from[2][3];
//	to._41=from[3][0]; to._42=from[3][1]; to._43=from[3][2]; to._44=from[3][3];
//
//	return to;
//}
//
//inline D3DXMATRIX&	ToD3D(const gmtl::Matrix33f& from, D3DXMATRIX& to)
//{
//	to._11=from[0][0]; to._12=from[0][1]; to._13=from[0][2]; to._14=0.0f;
//	to._21=from[1][0]; to._22=from[1][1]; to._23=from[1][2]; to._24=0.0f;
//	to._31=from[2][0]; to._32=from[2][1]; to._33=from[2][2]; to._34=0.0f;
//	to._41=0.0f;	   to._42=0.0f;		  to._43=0.0f;		 to._44=1.0f;
//	
//	return to;
//}
//
//inline D3DXMATRIX&	ToD3D(const gmtl::Matrix22f& from, D3DXMATRIX& to)
//{
//	to._11=from[0][0]; to._12=from[0][1]; to._13=0.0f; to._14=0.0f;
//	to._21=from[1][0]; to._22=from[1][1]; to._23=0.0f; to._24=0.0f;
//	to._31=0.0f;	   to._32=0.0f;		  to._33=1.0f; to._34=0.0f;
//	to._41=0.0f;	   to._42=0.0f;		  to._43=0.0f; to._44=1.0f;
//	
//	return to;
//}
//
//inline gmtl::Matrix44f&	ToGMTL(const D3DXMATRIX& from, gmtl::Matrix44f& to)
//{
//	to.set(from._11, from._12, from._13, from._14, 
//		   from._21, from._22, from._23, from._24,
//		   from._31, from._32, from._33, from._34,
//		   from._41, from._42, from._43, from._44);
//
//	return to;
//}
//
//// Vector Conversions
//inline const D3DXVECTOR4* ToD3D(const gmtl::Vec4f& from)
//{
//	return (const D3DXVECTOR4*)(from.getData());
//}
//
//inline const D3DXVECTOR3* ToD3D(const gmtl::Vec3f& from)
//{
//	return (const D3DXVECTOR3*)(from.getData());
//}
//
//inline const D3DXVECTOR2* ToD3D(const gmtl::Vec2f& from)
//{
//	return (const D3DXVECTOR2*)(from.getData());
//}
//
//inline D3DXVECTOR4&	ToD3D(const gmtl::Vec4f& from, D3DXVECTOR4& to)
//{
//	to.x = from[0]; to.y = from[1]; to.z = from[2]; to.w = from[3];
//	return to;
//}
//
//inline D3DXVECTOR3&	ToD3D(const gmtl::Vec3f& from, D3DXVECTOR3& to)
//{
//	to.x = from[0]; to.y = from[1]; to.z = from[2];
//	return to;
//}
//
//inline D3DXVECTOR2&	ToD3D(const gmtl::Vec2f& from, D3DXVECTOR2& to)
//{
//	to.x = from[0]; to.y = from[1];
//	return to;
//}
//
//inline gmtl::Vec4f&	ToGMTL(const D3DXVECTOR4& from, gmtl::Vec4f& to)
//{
//	to.set(from.x, from.y, from.z, from.w);
//	return to;
//}
//
//inline gmtl::Vec3f&	ToGMTL(const D3DXVECTOR3& from, gmtl::Vec3f& to)
//{
//	to.set(from.x, from.y, from.z);
//	return to;
//}
//
//inline gmtl::Vec2f&	ToGMTL(const D3DXVECTOR2& from, gmtl::Vec2f& to)
//{
//	to.set(from.x, from.y);
//	return to;
//}
//
//// Point Conversions
//inline const D3DXVECTOR4* ToD3D(const gmtl::Point4f& from)
//{
//	return (const D3DXVECTOR4*)(from.getData());
//}
//
//inline D3DXVECTOR4& ToD3D(const gmtl::Point4f& from, D3DXVECTOR4& to)
//{
//	to.x = from[0]; to.y = from[1]; to.z = from[2]; to.w = from[3];
//	return to;
//}
//
//inline D3DXVECTOR3&	ToD3D(const gmtl::Point3f& from, D3DXVECTOR3& to)
//{
//	to.x = from[0]; to.y = from[1]; to.z = from[2];
//	return to;
//}
//
//inline D3DXVECTOR2&	ToD3D(const gmtl::Point2f& from, D3DXVECTOR2& to)
//{
//	to.x = from[0]; to.y = from[1];
//	return to;
//}
//
//inline gmtl::Point4f&	ToGMTL(const D3DXVECTOR4& from, gmtl::Point4f& to)
//{
//	to.set(from.x, from.y, from.z, from.w);
//	return to;
//}
//
//inline gmtl::Point3f&	ToGMTL(const D3DXVECTOR3& from, gmtl::Point3f& to)
//{
//	to.set(from.x, from.y, from.z);
//	return to;
//}
//
//inline gmtl::Point2f&	ToGMTL(const D3DXVECTOR2& from, gmtl::Point2f& to)
//{
//	to.set(from.x, from.y);
//	return to;
//}

_NAMESPACE_END
