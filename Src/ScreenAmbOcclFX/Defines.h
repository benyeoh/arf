//==============================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		08/12/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

// Effect Dynamic Parameter Semantics
enum
{
    SAOFX_DYNAMIC_SEMANTIC_BEGIN = 340,

    SAOFX_DYNAMIC_SEMANTIC_END	= 360,
};

// Effect Constant Parameter Semantics
enum
{
    SAOFX_CONSTANT_SEMANTIC_BEGIN = 5280,

   // SAOFX_ZPOS_RT = SAOFX_CONSTANT_SEMANTIC_BEGIN,

    SAOFX_CONSTANT_SEMANTIC_END = 5300,
};

// Render Group ID
enum
{
    SAOFX_GROUP_AO         = 30000,
    SAOFX_GROUP_BLURH      = 30001,
    SAOFX_GROUP_BLURV      = 30002,
    SAOFX_GROUP_UPSAMPLE   = 30003,
};

_NAMESPACE_END
