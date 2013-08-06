//==============================================================================
//
//		D3D11HelperFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

inline eRCubeFace CubeFaceFromD3D11(D3D11_TEXTURECUBE_FACE face)
{
	switch(face)
	{
	case D3D11_TEXTURECUBE_FACE_POSITIVE_X:		return CF_POS_X;
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_X:		return CF_NEG_X;
	case D3D11_TEXTURECUBE_FACE_POSITIVE_Y:		return CF_POS_Y;
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_Y:		return CF_NEG_Y;
	case D3D11_TEXTURECUBE_FACE_POSITIVE_Z:		return CF_NEG_Z;
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_Z:		return CF_POS_Z;

	default: _DEBUG_ASSERT(FALSE);
	}

	return CF_NUM_FACES;
}

inline D3D11_TEXTURECUBE_FACE CubeFaceToD3D11(eRCubeFace face)
{
	switch(face)
	{
	case CF_POS_X:		return D3D11_TEXTURECUBE_FACE_POSITIVE_X;
	case CF_NEG_X:		return D3D11_TEXTURECUBE_FACE_NEGATIVE_X;
	case CF_POS_Y:		return D3D11_TEXTURECUBE_FACE_POSITIVE_Y;
	case CF_NEG_Y:		return D3D11_TEXTURECUBE_FACE_NEGATIVE_Y;
	case CF_POS_Z:		return D3D11_TEXTURECUBE_FACE_NEGATIVE_Z;	// Flip Z
	case CF_NEG_Z:		return D3D11_TEXTURECUBE_FACE_POSITIVE_Z;

	default: _DEBUG_ASSERT(FALSE);
	}

	return D3DCUBEMAP_FACE_FORCE_DWORD;	
}

inline eRTexFormat TexFormatFromD3D11Typeless(DXGI_FORMAT format)
{
	switch(format)
	{
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:	return TEXF_A8R8G8B8;
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:	return TEXF_X8R8G8B8;
		case DXGI_FORMAT_R32_TYPELESS:		return TEXF_R32F;
		case DXGI_FORMAT_R16G16_TYPELESS:	return TEXF_G16R16F;

		case DXGI_FORMAT_BC1_TYPELESS:		return TEXF_DXT1;
		case DXGI_FORMAT_BC2_TYPELESS:		return TEXF_DXT3;
		case DXGI_FORMAT_BC3_TYPELESS:		return TEXF_DXT5;

		case DXGI_FORMAT_R8_TYPELESS:			return TEXF_R8;
		case DXGI_FORMAT_R8G8_TYPELESS:			return TEXF_A8R8;	// No equivalent
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:	return TEXF_A32B32G32R32F;
		case DXGI_FORMAT_R32G32_TYPELESS:			return TEXF_G32R32F;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:		return TEXF_A16B16G16R16F;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:		return TEXF_A2R10G10B10;

	}

	return TEXF_END;

}

inline eRTexFormat TexFormatFromD3D11(DXGI_FORMAT format)
{
	switch(format)
	{
	case DXGI_FORMAT_B8G8R8A8_UNORM:	return TEXF_A8R8G8B8;	
	case DXGI_FORMAT_B8G8R8X8_UNORM:	return TEXF_X8R8G8B8;
	case DXGI_FORMAT_R32_FLOAT: 		return TEXF_R32F;
	case DXGI_FORMAT_R16G16_FLOAT:		return TEXF_G16R16F;
	case DXGI_FORMAT_R16G16_UNORM:		return TEXF_G16R16;
		
	case DXGI_FORMAT_BC1_UNORM:			return TEXF_DXT1;
	case DXGI_FORMAT_BC2_UNORM:			return TEXF_DXT3;
	case DXGI_FORMAT_BC3_UNORM:			return TEXF_DXT5;
	
	case DXGI_FORMAT_R8_UNORM:			return TEXF_R8;
	case DXGI_FORMAT_R8G8_UNORM:		return TEXF_A8R8;	// No equivalent
	case DXGI_FORMAT_R32G32B32A32_FLOAT:	return TEXF_A32B32G32R32F;
	case DXGI_FORMAT_R32G32_FLOAT:			return TEXF_G32R32F;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:	return TEXF_A16B16G16R16F;
	case DXGI_FORMAT_R10G10B10A2_UNORM:		return TEXF_A2R10G10B10;

	}	

	return TEXF_END;
}

inline DXGI_FORMAT TexFormatToD3D11Typeless(eRTexFormat format)
{
	switch(format)
	{
	case TEXF_A8R8G8B8:		return DXGI_FORMAT_B8G8R8A8_TYPELESS;
	case TEXF_X8R8G8B8:		return DXGI_FORMAT_B8G8R8X8_TYPELESS;
	case TEXF_R32F:			return DXGI_FORMAT_R32_TYPELESS;
	case TEXF_G16R16F:		return DXGI_FORMAT_R16G16_TYPELESS;
	case TEXF_G16R16:		return DXGI_FORMAT_R16G16_TYPELESS;
	case TEXF_DXT1:			return DXGI_FORMAT_BC1_TYPELESS;
	case TEXF_DXT2:			return DXGI_FORMAT_BC2_TYPELESS;
	case TEXF_DXT3:			return DXGI_FORMAT_BC2_TYPELESS;
	case TEXF_DXT4:			return DXGI_FORMAT_BC3_TYPELESS;
	case TEXF_DXT5:			return DXGI_FORMAT_BC3_TYPELESS;
	case TEXF_R8:			return DXGI_FORMAT_R8_TYPELESS;
	case TEXF_A8R8:			return DXGI_FORMAT_R8G8_TYPELESS;
	case TEXF_A32B32G32R32F:	return DXGI_FORMAT_R32G32B32A32_TYPELESS;		
	case TEXF_G32R32F:			return DXGI_FORMAT_R32G32_TYPELESS;
	case TEXF_A16B16G16R16F:	return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case TEXF_A2R10G10B10:		return DXGI_FORMAT_R10G10B10A2_TYPELESS;

	}

	return DXGI_FORMAT_UNKNOWN;
}

inline DXGI_FORMAT TexFormatToD3D11(eRTexFormat format)
{
	switch(format)
	{
	case TEXF_A8R8G8B8:		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case TEXF_X8R8G8B8:		return DXGI_FORMAT_B8G8R8X8_UNORM;
	case TEXF_R32F:			return DXGI_FORMAT_R32_FLOAT;
	case TEXF_G16R16F:		return DXGI_FORMAT_R16G16_FLOAT;
	case TEXF_G16R16:		return DXGI_FORMAT_R16G16_UNORM;
	case TEXF_DXT1:			return DXGI_FORMAT_BC1_UNORM;
	case TEXF_DXT2:			return DXGI_FORMAT_BC2_UNORM;
	case TEXF_DXT3:			return DXGI_FORMAT_BC2_UNORM;
	case TEXF_DXT4:			return DXGI_FORMAT_BC3_UNORM;
	case TEXF_DXT5:			return DXGI_FORMAT_BC3_UNORM;
	case TEXF_R8:			return DXGI_FORMAT_R8_UNORM;
	case TEXF_A8R8:			return DXGI_FORMAT_R8G8_UNORM;
	case TEXF_A32B32G32R32F:	return DXGI_FORMAT_R32G32B32A32_FLOAT;		
	case TEXF_G32R32F:			return DXGI_FORMAT_R32G32_FLOAT;
	case TEXF_A16B16G16R16F:	return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case TEXF_A2R10G10B10:		return DXGI_FORMAT_R10G10B10A2_UNORM;

	}

	return DXGI_FORMAT_UNKNOWN;
}

inline DXGI_FORMAT TexFormatToD3D11Full(eRTexFormat format)
{
	DXGI_FORMAT toReturn = TexFormatToD3D11Typeless(format);
	if(toReturn == DXGI_FORMAT)
		toReturn = TexFormatToD3D11(format);

	return toReturn;
}

inline DXGI_FORMAT DepthFormatToD3D11(eRDepthFormat format)
{
	switch(format)
	{
	case DF_D16:	return DXGI_FORMAT_D16_UNORM;
	case DF_D32:	return DXGI_FORMAT_D32_FLOAT;
	case DF_D24S8:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	return DXGI_FORMAT_UNKNOWN;
}

inline DXGI_FORMAT DepthFormatToD3D11Typeless(eRDepthFormat format)
{
	switch(format)
	{
	case DF_D16:	return DXGI_FORMAT_R16_TYPELESS;
	case DF_D32:	return DXGI_FORMAT_R32_TYPELESS;
	case DF_D24S8:	return DXGI_FORMAT_R24G8_TYPELESS;
	}

	return DXGI_FORMAT_UNKNOWN;
}

inline eRDepthFormat DepthFormatFromD3D11(DXGI_FORMAT format)
{
	switch(format)
	{
	case DXGI_FORMAT_D16_UNORM:			return DF_D16;
	case DXGI_FORMAT_D32_FLOAT:			return DF_D32;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:	return DF_D24S8;
	}

	return DF_END;
}

inline eRDepthFormat DepthFormatFromD3D11Typeless(DXGI_FORMAT format)
{
	switch(format)
	{
	case DXGI_FORMAT_R16_TYPELESS:			return DF_D16;
	case DXGI_FORMAT_R32_TYPELESS:			return DF_D32;
	case DXGI_FORMAT_R24G8_TYPELESS:		return DF_D24S8;
	}

	return DF_END;
}

inline eRTexUsage TexUsageFromD3D11(D3D11_USAGE usage)
{
	switch(usage)
	{
	case D3D11_USAGE_DEFAULT:		return TEXU_DEFAULT;
	case D3D11_USAGE_DYNAMIC:		return TEXU_DYNAMIC;
	}

	return TEXU_END;
}

inline D3D11_USAGE TexUsageToD3D11(eRTexUsage usage)
{
	switch(usage)
	{
	case TEXU_DEFAULT:		return D3D11_USAGE_DEFAULT;
	case TEXU_DYNAMIC:		return D3D11_USAGE_DYNAMIC;
	
	default:	_DEBUG_ASSERT(FALSE);
	}

	return D3D11_USAGE_DEFAULT;
}

inline D3D11_USAGE BufUsageToD3D11(eRBufferUsage usage)
{
	switch(usage)
	{
	case BUF_DEFAULT:		return D3D11_USAGE_DEFAULT;
	case BUF_DYNAMIC:		return D3D11_USAGE_DYNAMIC;

	default:	_DEBUG_ASSERT(FALSE);
	}

	return BUF_DEFAULT;
}

_NAMESPACE_END
