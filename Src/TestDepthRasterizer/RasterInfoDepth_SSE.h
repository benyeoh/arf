//================================================================================
//
//		RasterInfoDepth_SSE.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/1/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

template <uint numLayer>
struct RasterInfoDepth
{
    __m128 e1aaaa;
    __m128 e1bbbb;
    __m128 e1cccc;

    __m128 e2aaaa;
    __m128 e2bbbb;
    __m128 e2cccc;

    __m128 e3aaaa;
    __m128 e3bbbb;
    __m128 e3cccc;

    __m128 e1RejectLayerCCCC[numLayer];
    __m128 e2RejectLayerCCCC[numLayer];
    __m128 e3RejectLayerCCCC[numLayer];

    __m128 e1AcceptLayerOffset[numLayer];
    __m128 e2AcceptLayerOffset[numLayer];
    __m128 e3AcceptLayerOffset[numLayer];

    __m128 e1XLayerAdd[numLayer];
    __m128 e1YLayerAdd[numLayer];
    __m128 e2XLayerAdd[numLayer];
    __m128 e2YLayerAdd[numLayer];
    __m128 e3XLayerAdd[numLayer];
    __m128 e3YLayerAdd[numLayer];

    __m128 e1XFinalAdd;
    __m128 e2XFinalAdd;
    __m128 e3XFinalAdd;

    __m128 e1YFinalAdd;
    __m128 e2YFinalAdd;
    __m128 e3YFinalAdd;

    __m128 e1XYFinalAdd;
    __m128 e2XYFinalAdd;
    __m128 e3XYFinalAdd;

    __m128 depthAAAA;
    __m128 depthBBBB;
    __m128 depthCCCC;
    __m128 depthXYFinalAdd;
    __m128 depthXFinalAdd;
    __m128 depthYFinalAdd;
    __m128 depthXLayerAdd[numLayer];
    __m128 depthYLayerAdd[numLayer];

    _ALIGN(16) float triBB[4 * numLayer];
    _ALIGN(16) float triBBFinal[4];
};

#define _GET_RASTER_INFO_SIZE_DEPTH(numLayers) sizeof(RasterInfoDepth<numLayers>)

#define _GET_NUM_TILES_DEPTH(bufferWidth, bufferHeight, tileWidth, tileHeight) \
    ( ((bufferWidth * bufferHeight) / (tileWidth * tileHeight)) )

_NAMESPACE_END