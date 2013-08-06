//================================================================================
//
//		RasterInfo.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/1/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

template <uint numInterpolants, uint numLayer>
struct RasterInfo
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
	//__m128 e1RejectTopCCCC;
	//__m128 e2RejectTopCCCC;
	//__m128 e3RejectTopCCCC;

	//__m128 e1RejectMidCCCC;
	//__m128 e2RejectMidCCCC;
	//__m128 e3RejectMidCCCC;

	__m128 e1AcceptLayerOffset[numLayer];
	__m128 e2AcceptLayerOffset[numLayer];
	__m128 e3AcceptLayerOffset[numLayer];
	//__m128 e1AcceptTopOffset;
	//__m128 e2AcceptTopOffset;
	//__m128 e3AcceptTopOffset;

	//__m128 e1AcceptMidOffset;
	//__m128 e2AcceptMidOffset;
	//__m128 e3AcceptMidOffset;

	__m128 e1XLayerAdd[numLayer];
	__m128 e1YLayerAdd[numLayer];
	__m128 e2XLayerAdd[numLayer];
	__m128 e2YLayerAdd[numLayer];
	__m128 e3XLayerAdd[numLayer];
	__m128 e3YLayerAdd[numLayer];

	//__m128 e1XTopAdd;
	//__m128 e2XTopAdd;
	//__m128 e3XTopAdd;

	//__m128 e1YTopAdd;
	//__m128 e2YTopAdd;
	//__m128 e3YTopAdd;

	//__m128 e1XMidAdd;
	//__m128 e2XMidAdd;
	//__m128 e3XMidAdd;

	//__m128 e1YMidAdd;
	//__m128 e2YMidAdd;
	//__m128 e3YMidAdd;

	__m128 e1XFinalAdd;
	__m128 e2XFinalAdd;
	__m128 e3XFinalAdd;

	__m128 e1YFinalAdd;
	__m128 e2YFinalAdd;
	__m128 e3YFinalAdd;


	__m128 depthAAAA;
	__m128 depthBBBB;
	__m128 depthCCCC;
	__m128 depthXFinalAdd;
	__m128 depthYFinalAdd;

	__m128 invWAAAA;
	__m128 invWBBBB;
	__m128 invWCCCC;
	__m128 invWXFinalAdd;
	__m128 invWYFinalAdd;

	__m128 edgeInterpolantsAAAA[numInterpolants > 0 ? numInterpolants : 1];
	__m128 edgeInterpolantsBBBB[numInterpolants > 0 ? numInterpolants : 1];
	__m128 edgeInterpolantsCCCC[numInterpolants > 0 ? numInterpolants : 1];
	__m128 edgeInterpolantsXFinalAdd[numInterpolants > 0 ? numInterpolants : 1];
	__m128 edgeInterpolantsYFinalAdd[numInterpolants > 0 ? numInterpolants : 1];

	_ALIGN(16) float triBB[4 * numLayer];
	_ALIGN(16) float triBBFinal[4];
};

#define _GET_RASTER_INFO_SIZE(numInterpolants, numLayers) sizeof(RasterInfo<numInterpolants, numLayers>)

_NAMESPACE_END