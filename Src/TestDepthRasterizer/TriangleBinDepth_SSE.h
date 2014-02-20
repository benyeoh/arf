//================================================================================
//
//		TriangleBinDepth_SSE.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/1/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

struct TriangleData
{
    uint xy[3];
    float z[3];
};

struct TriangleBin
{
    const static uint MAX_NUM_TRIANGLES_PER_BIN = 1024 * 8;

    TriangleData tris[MAX_NUM_TRIANGLES_PER_BIN];
};

_NAMESPACE_END